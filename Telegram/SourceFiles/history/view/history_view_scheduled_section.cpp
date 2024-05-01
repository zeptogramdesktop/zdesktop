/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "history/view/history_view_scheduled_section.h"

#include "history/view/controls/history_view_compose_controls.h"
#include "history/view/history_view_empty_list_bubble.h"
#include "history/view/history_view_top_bar_widget.h"
#include "history/view/history_view_schedule_box.h"
#include "history/view/history_view_sticker_toast.h"
#include "history/history.h"
#include "history/history_drag_area.h"
#include "history/history_item_helpers.h" // GetErrorTextForSending.
#include "menu/menu_send.h" // SendMenu::Type.
#include "ui/widgets/scroll_area.h"
#include "ui/widgets/shadow.h"
#include "ui/chat/chat_style.h"
#include "ui/text/text_utilities.h"
#include "api/api_editing.h"
#include "api/api_sending.h"
#include "apiwrap.h"
#include "boxes/delete_messages_box.h"
#include "boxes/send_files_box.h"
#include "boxes/premium_limits_box.h"
#include "window/window_session_controller.h"
#include "window/window_peer_menu.h"
#include "base/call_delayed.h"
#include "base/qt/qt_key_modifiers.h"
#include "core/mime_type.h"
#include "chat_helpers/tabbed_selector.h"
#include "main/main_session.h"
#include "data/components/scheduled_messages.h"
#include "data/data_forum.h"
#include "data/data_forum_topic.h"
#include "data/data_session.h"
#include "data/data_changes.h"
#include "data/data_user.h"
#include "data/data_message_reactions.h"
#include "data/data_peer_values.h"
#include "data/data_premium_limits.h"
#include "storage/storage_media_prepare.h"
#include "storage/storage_account.h"
#include "storage/localimageloader.h"
#include "inline_bots/inline_bot_result.h"
#include "lang/lang_keys.h"
#include "styles/style_chat.h"
#include "styles/style_chat_helpers.h"
#include "styles/style_boxes.h"

#include <QtCore/QMimeData>

namespace HistoryView {

ScheduledMemento::ScheduledMemento(not_null<History*> history)
: _history(history)
, _forumTopic(nullptr) {
	const auto list = _history->session().scheduledMessages().list(_history);
	if (!list.ids.empty()) {
		_list.setScrollTopState({ .item = { .fullId = list.ids.front() } });
	}
}

ScheduledMemento::ScheduledMemento(not_null<Data::ForumTopic*> forumTopic)
: _history(forumTopic->owningHistory())
, _forumTopic(forumTopic) {
	const auto list = _history->session().scheduledMessages().list(
		_forumTopic);
	if (!list.ids.empty()) {
		_list.setScrollTopState({ .item = { .fullId = list.ids.front() } });
	}
}

object_ptr<Window::SectionWidget> ScheduledMemento::createWidget(
		QWidget *parent,
		not_null<Window::SessionController*> controller,
		Window::Column column,
		const QRect &geometry) {
	if (column == Window::Column::Third) {
		return nullptr;
	}
	auto result = object_ptr<ScheduledWidget>(
		parent,
		controller,
		_history,
		_forumTopic);
	result->setInternalState(geometry, this);
	return result;
}

ScheduledWidget::ScheduledWidget(
	QWidget *parent,
	not_null<Window::SessionController*> controller,
	not_null<History*> history,
	const Data::ForumTopic *forumTopic)
: Window::SectionWidget(parent, controller, history->peer)
, _history(history)
, _forumTopic(forumTopic)
, _scroll(
	this,
	controller->chatStyle()->value(lifetime(), st::historyScroll),
	false)
, _topBar(this, controller)
, _topBarShadow(this)
, _composeControls(std::make_unique<ComposeControls>(
	this,
	ComposeControlsDescriptor{
		.show = controller->uiShow(),
		.unavailableEmojiPasted = [=](not_null<DocumentData*> emoji) {
			listShowPremiumToast(emoji);
		},
		.mode = ComposeControls::Mode::Scheduled,
		.sendMenuType = SendMenu::Type::Disabled,
		.regularWindow = controller,
		.stickerOrEmojiChosen = controller->stickerOrEmojiChosen(),
	}))
, _cornerButtons(
		_scroll.data(),
		controller->chatStyle(),
		static_cast<HistoryView::CornerButtonsDelegate*>(this)) {
	controller->chatStyle()->paletteChanged(
	) | rpl::start_with_next([=] {
		_scroll->updateBars();
	}, _scroll->lifetime());

	Window::ChatThemeValueFromPeer(
		controller,
		history->peer
	) | rpl::start_with_next([=](std::shared_ptr<Ui::ChatTheme> &&theme) {
		_theme = std::move(theme);
		controller->setChatStyleTheme(_theme);
	}, lifetime());

	const auto state = Dialogs::EntryState{
		.key = _history,
		.section = Dialogs::EntryState::Section::Scheduled,
	};
	_topBar->setActiveChat(state, nullptr);
	_composeControls->setCurrentDialogsEntryState(state);
	controller->setCurrentDialogsEntryState(state);

	_topBar->move(0, 0);
	_topBar->resizeToWidth(width());
	_topBar->show();

	_topBar->sendNowSelectionRequest(
	) | rpl::start_with_next([=] {
		confirmSendNowSelected();
	}, _topBar->lifetime());
	_topBar->deleteSelectionRequest(
	) | rpl::start_with_next([=] {
		confirmDeleteSelected();
	}, _topBar->lifetime());
	_topBar->clearSelectionRequest(
	) | rpl::start_with_next([=] {
		clearSelected();
	}, _topBar->lifetime());

	_topBarShadow->raise();
	controller->adaptive().value(
	) | rpl::start_with_next([=] {
		updateAdaptiveLayout();
	}, lifetime());

	_inner = _scroll->setOwnedWidget(object_ptr<ListWidget>(
		this,
		controller,
		static_cast<ListDelegate*>(this)));
	_scroll->move(0, _topBar->height());
	_scroll->show();
	_scroll->scrolls(
	) | rpl::start_with_next([=] {
		onScroll();
	}, lifetime());

	_inner->editMessageRequested(
	) | rpl::start_with_next([=](auto fullId) {
		if (const auto item = session().data().message(fullId)) {
			const auto media = item->media();
			if (!media || media->webpage() || media->allowsEditCaption()) {
				_composeControls->editMessage(
					fullId,
					_inner->getSelectedTextRange(item));
			}
		}
	}, _inner->lifetime());

	{
		auto emptyInfo = base::make_unique_q<EmptyListBubbleWidget>(
			_inner,
			controller->chatStyle(),
			st::msgServicePadding);
		const auto emptyText = Ui::Text::Semibold(
			tr::lng_scheduled_messages_empty(tr::now));
		emptyInfo->setText(emptyText);
		_inner->setEmptyInfoWidget(std::move(emptyInfo));
	}
	setupComposeControls();
}

ScheduledWidget::~ScheduledWidget() = default;

void ScheduledWidget::setupComposeControls() {
	auto writeRestriction = _forumTopic
		? [&] {
			auto topicWriteRestrictions = rpl::single(
			) | rpl::then(session().changes().topicUpdates(
				Data::TopicUpdate::Flag::Closed
			) | rpl::filter([=](const Data::TopicUpdate &update) {
				return (update.topic->history() == _history)
					&& (update.topic->rootId() == _forumTopic->rootId());
			}) | rpl::to_empty) | rpl::map([=] {
				return (!_forumTopic
						|| _forumTopic->canToggleClosed()
						|| !_forumTopic->closed())
					? std::optional<QString>()
					: tr::lng_forum_topic_closed(tr::now);
			});
			return rpl::combine(
				session().changes().peerFlagsValue(
					_history->peer,
					Data::PeerUpdate::Flag::Rights),
				Data::CanSendAnythingValue(_history->peer),
				std::move(topicWriteRestrictions)
			) | rpl::map([=](
					auto,
					auto,
					std::optional<QString> topicRestriction) {
				const auto allWithoutPolls = Data::AllSendRestrictions()
					& ~ChatRestriction::SendPolls;
				const auto canSendAnything = Data::CanSendAnyOf(
					_forumTopic,
					allWithoutPolls);
				const auto restriction = Data::RestrictionError(
					_history->peer,
					ChatRestriction::SendOther);
				auto text = !canSendAnything
					? (restriction
						? restriction
						: topicRestriction
						? std::move(topicRestriction)
						: tr::lng_group_not_accessible(tr::now))
					: topicRestriction
					? std::move(topicRestriction)
					: std::optional<QString>();
				return text ? Controls::WriteRestriction{
					.text = std::move(*text),
					.type = Controls::WriteRestrictionType::Rights,
				} : Controls::WriteRestriction();
			}) | rpl::type_erased();
		}()
		: [&] {
			return rpl::combine(
				session().changes().peerFlagsValue(
					_history->peer,
					Data::PeerUpdate::Flag::Rights),
				Data::CanSendAnythingValue(_history->peer)
			) | rpl::map([=] {
				const auto allWithoutPolls = Data::AllSendRestrictions()
					& ~ChatRestriction::SendPolls;
				const auto canSendAnything = Data::CanSendAnyOf(
					_history->peer,
					allWithoutPolls,
					false);
				const auto restriction = Data::RestrictionError(
					_history->peer,
					ChatRestriction::SendOther);
				auto text = !canSendAnything
					? (restriction
						? restriction
						: tr::lng_group_not_accessible(tr::now))
					: std::optional<QString>();
				return text ? Controls::WriteRestriction{
					.text = std::move(*text),
					.type = Controls::WriteRestrictionType::Rights,
				} : Controls::WriteRestriction();
			}) | rpl::type_erased();
		}();
	_composeControls->setHistory({
		.history = _history.get(),
		.writeRestriction = std::move(writeRestriction),
	});

	_composeControls->height(
	) | rpl::start_with_next([=] {
		const auto wasMax = (_scroll->scrollTopMax() == _scroll->scrollTop());
		updateControlsGeometry();
		if (wasMax) {
			listScrollTo(_scroll->scrollTopMax());
		}
	}, lifetime());

	_composeControls->cancelRequests(
	) | rpl::start_with_next([=] {
		listCancelRequest();
	}, lifetime());

	_composeControls->sendRequests(
	) | rpl::start_with_next([=] {
		send();
	}, lifetime());

	_composeControls->sendVoiceRequests(
	) | rpl::start_with_next([=](ComposeControls::VoiceToSend &&data) {
		sendVoice(data.bytes, data.waveform, data.duration);
	}, lifetime());

	_composeControls->sendCommandRequests(
	) | rpl::start_with_next([=](const QString &command) {
		listSendBotCommand(command, FullMsgId());
	}, lifetime());

	const auto saveEditMsgRequestId = lifetime().make_state<mtpRequestId>(0);
	_composeControls->editRequests(
	) | rpl::start_with_next([=](auto data) {
		if (const auto item = session().data().message(data.fullId)) {
			if (item->isScheduled()) {
				edit(item, data.options, saveEditMsgRequestId);
			}
		}
	}, lifetime());

	_composeControls->attachRequests(
	) | rpl::filter([=] {
		return !_choosingAttach;
	}) | rpl::start_with_next([=] {
		_choosingAttach = true;
		base::call_delayed(
			st::historyAttach.ripple.hideDuration,
			this,
			[=] { _choosingAttach = false; chooseAttach(); });
	}, lifetime());

	_composeControls->fileChosen(
	) | rpl::start_with_next([=](ChatHelpers::FileChosen data) {
		controller()->hideLayer(anim::type::normal);
		sendExistingDocument(data.document);
	}, lifetime());

	_composeControls->photoChosen(
	) | rpl::start_with_next([=](ChatHelpers::PhotoChosen chosen) {
		sendExistingPhoto(chosen.photo);
	}, lifetime());

	_composeControls->inlineResultChosen(
	) | rpl::start_with_next([=](ChatHelpers::InlineChosen chosen) {
		sendInlineResult(chosen.result, chosen.bot);
	}, lifetime());

	_composeControls->jumpToItemRequests(
	) | rpl::start_with_next([=](FullReplyTo to) {
		if (const auto item = session().data().message(to.messageId)) {
			if (item->isScheduled() && item->history() == _history) {
				showAtPosition(item->position());
			} else {
				JumpToMessageClickHandler(
					item,
					{},
					to.quote,
					to.quoteOffset
				)->onClick({});
			}
		}
	}, lifetime());

	_composeControls->scrollKeyEvents(
	) | rpl::start_with_next([=](not_null<QKeyEvent*> e) {
		_scroll->keyPressEvent(e);
	}, lifetime());

	_composeControls->editLastMessageRequests(
	) | rpl::start_with_next([=](not_null<QKeyEvent*> e) {
		if (!_inner->lastMessageEditRequestNotify()) {
			_scroll->keyPressEvent(e);
		}
	}, lifetime());

	_composeControls->setMimeDataHook([=](
			not_null<const QMimeData*> data,
			Ui::InputField::MimeAction action) {
		if (action == Ui::InputField::MimeAction::Check) {
			return Core::CanSendFiles(data);
		} else if (action == Ui::InputField::MimeAction::Insert) {
			return confirmSendingFiles(
				data,
				std::nullopt,
				Core::ReadMimeText(data));
		}
		Unexpected("action in MimeData hook.");
	});

	_composeControls->lockShowStarts(
	) | rpl::start_with_next([=] {
		_cornerButtons.updateJumpDownVisibility();
		_cornerButtons.updateUnreadThingsVisibility();
	}, lifetime());

	_composeControls->viewportEvents(
	) | rpl::start_with_next([=](not_null<QEvent*> e) {
		_scroll->viewportEvent(e);
	}, lifetime());
}

void ScheduledWidget::chooseAttach() {
	if (const auto error = Data::AnyFileRestrictionError(_history->peer)) {
		controller()->showToast(*error);
		return;
	}

	const auto filter = FileDialog::AllOrImagesFilter();
	FileDialog::GetOpenPaths(this, tr::lng_choose_files(tr::now), filter, crl::guard(this, [=](
			FileDialog::OpenResult &&result) {
		if (result.paths.isEmpty() && result.remoteContent.isEmpty()) {
			return;
		}

		if (!result.remoteContent.isEmpty()) {
			auto read = Images::Read({
				.content = result.remoteContent,
			});
			if (!read.image.isNull() && !read.animated) {
				confirmSendingFiles(
					std::move(read.image),
					std::move(result.remoteContent));
			} else {
				uploadFile(result.remoteContent, SendMediaType::File);
			}
		} else {
			const auto premium = controller()->session().user()->isPremium();
			auto list = Storage::PrepareMediaList(
				result.paths,
				st::sendMediaPreviewSize,
				premium);
			confirmSendingFiles(std::move(list));
		}
	}), nullptr);
}

bool ScheduledWidget::confirmSendingFiles(
		not_null<const QMimeData*> data,
		std::optional<bool> overrideSendImagesAsPhotos,
		const QString &insertTextOnCancel) {
	const auto hasImage = data->hasImage();
	const auto premium = controller()->session().user()->isPremium();

	if (const auto urls = Core::ReadMimeUrls(data); !urls.empty()) {
		auto list = Storage::PrepareMediaList(
			urls,
			st::sendMediaPreviewSize,
			premium);
		if (list.error != Ui::PreparedList::Error::NonLocalUrl) {
			if (list.error == Ui::PreparedList::Error::None
				|| !hasImage) {
				const auto emptyTextOnCancel = QString();
				list.overrideSendImagesAsPhotos = overrideSendImagesAsPhotos;
				confirmSendingFiles(std::move(list), emptyTextOnCancel);
				return true;
			}
		}
	}

	if (auto read = Core::ReadMimeImage(data)) {
		confirmSendingFiles(
			std::move(read.image),
			std::move(read.content),
			overrideSendImagesAsPhotos,
			insertTextOnCancel);
		return true;
	}
	return false;
}

bool ScheduledWidget::confirmSendingFiles(
		Ui::PreparedList &&list,
		const QString &insertTextOnCancel) {
	if (_composeControls->confirmMediaEdit(list)) {
		return true;
	} else if (showSendingFilesError(list)) {
		return false;
	}

	auto box = Box<SendFilesBox>(
		controller(),
		std::move(list),
		_composeControls->getTextWithAppliedMarkdown(),
		_history->peer,
		(CanScheduleUntilOnline(_history->peer)
			? Api::SendType::ScheduledToUser
			: Api::SendType::Scheduled),
		SendMenu::Type::Disabled);

	box->setConfirmedCallback(crl::guard(this, [=](
			Ui::PreparedList &&list,
			Ui::SendFilesWay way,
			TextWithTags &&caption,
			Api::SendOptions options,
			bool ctrlShiftEnter) {
		sendingFilesConfirmed(
			std::move(list),
			way,
			std::move(caption),
			options,
			ctrlShiftEnter);
	}));
	box->setCancelledCallback(_composeControls->restoreTextCallback(
		insertTextOnCancel));

	//ActivateWindow(controller());
	controller()->show(std::move(box));

	return true;
}

void ScheduledWidget::sendingFilesConfirmed(
		Ui::PreparedList &&list,
		Ui::SendFilesWay way,
		TextWithTags &&caption,
		Api::SendOptions options,
		bool ctrlShiftEnter) {
	Expects(list.filesToProcess.empty());

	if (showSendingFilesError(list, way.sendImagesAsPhotos())) {
		return;
	}
	auto groups = DivideByGroups(std::move(list), way, false);
	const auto type = way.sendImagesAsPhotos()
		? SendMediaType::Photo
		: SendMediaType::File;
	auto action = prepareSendAction(options);
	action.clearDraft = false;
	if ((groups.size() != 1 || !groups.front().sentWithCaption())
		&& !caption.text.isEmpty()) {
		auto message = Api::MessageToSend(action);
		message.textWithTags = base::take(caption);
		session().api().sendMessage(std::move(message));
	}
	for (auto &group : groups) {
		const auto album = (group.type != Ui::AlbumType::None)
			? std::make_shared<SendingAlbum>()
			: nullptr;
		session().api().sendFiles(
			std::move(group.list),
			type,
			base::take(caption),
			album,
			action);
	}
}

bool ScheduledWidget::confirmSendingFiles(
		QImage &&image,
		QByteArray &&content,
		std::optional<bool> overrideSendImagesAsPhotos,
		const QString &insertTextOnCancel) {
	if (image.isNull()) {
		return false;
	}

	auto list = Storage::PrepareMediaFromImage(
		std::move(image),
		std::move(content),
		st::sendMediaPreviewSize);
	list.overrideSendImagesAsPhotos = overrideSendImagesAsPhotos;
	return confirmSendingFiles(std::move(list), insertTextOnCancel);
}

void ScheduledWidget::pushReplyReturn(not_null<HistoryItem*> item) {
	if (_inner->viewByPosition(item->position())) {
		_cornerButtons.pushReplyReturn(item);
	}
}

void ScheduledWidget::checkReplyReturns() {
	const auto currentTop = _scroll->scrollTop();
	while (const auto replyReturn = _cornerButtons.replyReturn()) {
		const auto position = replyReturn->position();
		const auto scrollTop = _inner->scrollTopForPosition(position);
		const auto below = scrollTop
			? (currentTop >= std::min(*scrollTop, _scroll->scrollTopMax()))
			: _inner->isBelowPosition(position);
		if (below) {
			_cornerButtons.calculateNextReplyReturn();
		} else {
			break;
		}
	}
}

void ScheduledWidget::uploadFile(
		const QByteArray &fileContent,
		SendMediaType type) {
	const auto callback = [=](Api::SendOptions options) {
		session().api().sendFile(
			fileContent,
			type,
			prepareSendAction(options));
	};
	controller()->show(PrepareScheduleBox(this, sendMenuType(), callback));
}

bool ScheduledWidget::showSendingFilesError(
		const Ui::PreparedList &list) const {
	return showSendingFilesError(list, std::nullopt);
}

bool ScheduledWidget::showSendingFilesError(
		const Ui::PreparedList &list,
		std::optional<bool> compress) const {
	const auto text = [&] {
		using Error = Ui::PreparedList::Error;
		const auto peer = _history->peer;
		const auto error = Data::FileRestrictionError(peer, list, compress);
		if (error) {
			return *error;
		} else switch (list.error) {
		case Error::None: return QString();
		case Error::EmptyFile:
		case Error::Directory:
		case Error::NonLocalUrl: return tr::lng_send_image_empty(
			tr::now,
			lt_name,
			list.errorData);
		case Error::TooLargeFile: return u"(toolarge)"_q;
		}
		return tr::lng_forward_send_files_cant(tr::now);
	}();
	if (text.isEmpty()) {
		return false;
	} else if (text == u"(toolarge)"_q) {
		const auto fileSize = list.files.back().size;
		controller()->show(
			Box(FileSizeLimitBox, &session(), fileSize, nullptr));
		return true;
	}

	controller()->showToast(text);
	return true;
}

Api::SendAction ScheduledWidget::prepareSendAction(
		Api::SendOptions options) const {
	auto result = Api::SendAction(_history, options);
	result.options.sendAs = _composeControls->sendAsPeer();
	if (_forumTopic) {
		result.replyTo.topicRootId = _forumTopic->topicRootId();
		result.replyTo.messageId = FullMsgId(
			history()->peer->id,
			_forumTopic->topicRootId());
	}
	return result;
}

void ScheduledWidget::send() {
	const auto textWithTags = _composeControls->getTextWithAppliedMarkdown();
	if (textWithTags.text.isEmpty() && !_composeControls->readyToForward()) {
		return;
	}

	const auto error = GetErrorTextForSending(
		_history->peer,
		{
			.topicRootId = _forumTopic
				? _forumTopic->topicRootId()
				: history()->isForum()
				? MsgId(1)
				: MsgId(),
			.forward = nullptr,
			.text = &textWithTags,
			.ignoreSlowmodeCountdown = true,
		});
	if (!error.isEmpty()) {
		controller()->showToast(error);
		return;
	}
	const auto callback = [=](Api::SendOptions options) { send(options); };
	controller()->show(PrepareScheduleBox(this, sendMenuType(), callback));
}

void ScheduledWidget::send(Api::SendOptions options) {
	const auto webPageDraft = _composeControls->webPageDraft();

	auto message = Api::MessageToSend(prepareSendAction(options));
	message.textWithTags = _composeControls->getTextWithAppliedMarkdown();
	message.webPage = webPageDraft;

	session().api().sendMessage(std::move(message));

	_composeControls->cancelForward();
	_composeControls->clear();
	//_saveDraftText = true;
	//_saveDraftStart = crl::now();
	//onDraftSave();

	_composeControls->hidePanelsAnimated();

	//if (_previewData && _previewData->pendingTill) previewCancel();
	_composeControls->focus();
}

void ScheduledWidget::sendVoice(
		QByteArray bytes,
		VoiceWaveform waveform,
		crl::time duration) {
	const auto callback = [=](Api::SendOptions options) {
		sendVoice(bytes, waveform, duration, options);
	};
	controller()->show(PrepareScheduleBox(this, sendMenuType(), callback));
}

void ScheduledWidget::sendVoice(
		QByteArray bytes,
		VoiceWaveform waveform,
		crl::time duration,
		Api::SendOptions options) {
	session().api().sendVoiceMessage(
		bytes,
		waveform,
		duration,
		prepareSendAction(options));
	_composeControls->clearListenState();
}

void ScheduledWidget::edit(
		not_null<HistoryItem*> item,
		Api::SendOptions options,
		mtpRequestId *const saveEditMsgRequestId) {
	if (*saveEditMsgRequestId) {
		return;
	}
	const auto webpage = _composeControls->webPageDraft();
	const auto sending = _composeControls->prepareTextForEditMsg();

	const auto hasMediaWithCaption = item
		&& item->media()
		&& item->media()->allowsEditCaption();
	if (sending.text.isEmpty() && !hasMediaWithCaption) {
		if (item) {
			controller()->show(Box<DeleteMessagesBox>(item, false));
		} else {
			_composeControls->focus();
		}
		return;
	} else {
		const auto maxCaptionSize = !hasMediaWithCaption
			? MaxMessageSize
			: Data::PremiumLimits(&session()).captionLengthCurrent();
		const auto remove = _composeControls->fieldCharacterCount()
			- maxCaptionSize;
		if (remove > 0) {
			controller()->showToast(
				tr::lng_edit_limit_reached(tr::now, lt_count, remove));
			return;
		}
	}

	lifetime().add([=] {
		if (!*saveEditMsgRequestId) {
			return;
		}
		session().api().request(base::take(*saveEditMsgRequestId)).cancel();
	});

	const auto done = [=](mtpRequestId requestId) {
		if (requestId == *saveEditMsgRequestId) {
			*saveEditMsgRequestId = 0;
			_composeControls->cancelEditMessage();
		}
	};

	const auto fail = [=](const QString &error, mtpRequestId requestId) {
		if (requestId == *saveEditMsgRequestId) {
			*saveEditMsgRequestId = 0;
		}

		if (ranges::contains(Api::kDefaultEditMessagesErrors, error)) {
			controller()->showToast(tr::lng_edit_error(tr::now));
		} else if (error == u"MESSAGE_NOT_MODIFIED"_q) {
			_composeControls->cancelEditMessage();
		} else if (error == u"MESSAGE_EMPTY"_q) {
			_composeControls->focus();
		} else {
			controller()->showToast(tr::lng_edit_error(tr::now));
		}
		update();
		return true;
	};

	*saveEditMsgRequestId = Api::EditTextMessage(
		item,
		sending,
		webpage,
		options,
		crl::guard(this, done),
		crl::guard(this, fail));

	_composeControls->hidePanelsAnimated();
	_composeControls->focus();
}

void ScheduledWidget::sendExistingDocument(
		not_null<DocumentData*> document) {
	const auto callback = [=](Api::SendOptions options) {
		sendExistingDocument(document, options);
	};
	controller()->show(PrepareScheduleBox(this, sendMenuType(), callback));
}

bool ScheduledWidget::sendExistingDocument(
		not_null<DocumentData*> document,
		Api::SendOptions options) {
	const auto error = Data::RestrictionError(
		_history->peer,
		ChatRestriction::SendStickers);
	if (error) {
		controller()->showToast(*error);
		return false;
	} else if (ShowSendPremiumError(controller(), document)) {
		return false;
	}

	Api::SendExistingDocument(
		Api::MessageToSend(prepareSendAction(options)),
		document);

	_composeControls->hidePanelsAnimated();
	_composeControls->focus();
	return true;
}

void ScheduledWidget::sendExistingPhoto(not_null<PhotoData*> photo) {
	const auto callback = [=](Api::SendOptions options) {
		sendExistingPhoto(photo, options);
	};
	controller()->show(PrepareScheduleBox(this, sendMenuType(), callback));
}

bool ScheduledWidget::sendExistingPhoto(
		not_null<PhotoData*> photo,
		Api::SendOptions options) {
	const auto error = Data::RestrictionError(
		_history->peer,
		ChatRestriction::SendPhotos);
	if (error) {
		controller()->showToast(*error);
		return false;
	}

	Api::SendExistingPhoto(
		Api::MessageToSend(prepareSendAction(options)),
		photo);

	_composeControls->hidePanelsAnimated();
	_composeControls->focus();
	return true;
}

void ScheduledWidget::sendInlineResult(
		not_null<InlineBots::Result*> result,
		not_null<UserData*> bot) {
	const auto errorText = result->getErrorOnSend(_history);
	if (!errorText.isEmpty()) {
		controller()->showToast(errorText);
		return;
	}
	const auto callback = [=](Api::SendOptions options) {
		sendInlineResult(result, bot, options);
	};
	controller()->show(PrepareScheduleBox(this, sendMenuType(), callback));
}

void ScheduledWidget::sendInlineResult(
		not_null<InlineBots::Result*> result,
		not_null<UserData*> bot,
		Api::SendOptions options) {
	auto action = prepareSendAction(options);
	action.generateLocal = true;
	session().api().sendInlineResult(bot, result, action, std::nullopt);

	_composeControls->clear();
	//_saveDraftText = true;
	//_saveDraftStart = crl::now();
	//onDraftSave();

	auto &bots = cRefRecentInlineBots();
	const auto index = bots.indexOf(bot);
	if (index) {
		if (index > 0) {
			bots.removeAt(index);
		} else if (bots.size() >= RecentInlineBotsLimit) {
			bots.resize(RecentInlineBotsLimit - 1);
		}
		bots.push_front(bot);
		bot->session().local().writeRecentHashtagsAndBots();
	}

	_composeControls->hidePanelsAnimated();
	_composeControls->focus();
}

SendMenu::Type ScheduledWidget::sendMenuType() const {
	return _history->peer->isSelf()
		? SendMenu::Type::Reminder
		: HistoryView::CanScheduleUntilOnline(_history->peer)
		? SendMenu::Type::ScheduledToUser
		: SendMenu::Type::Scheduled;
}

void ScheduledWidget::cornerButtonsShowAtPosition(
		Data::MessagePosition position) {
	showAtPosition(position);
}

Data::Thread *ScheduledWidget::cornerButtonsThread() {
	return _history;
}

FullMsgId ScheduledWidget::cornerButtonsCurrentId() {
	return {};
}

bool ScheduledWidget::cornerButtonsIgnoreVisibility() {
	return animatingShow();
}

std::optional<bool> ScheduledWidget::cornerButtonsDownShown() {
	if (_composeControls->isLockPresent()
		|| _composeControls->isTTLButtonShown()) {
		return false;
	}
	const auto top = _scroll->scrollTop() + st::historyToDownShownAfter;
	if (top < _scroll->scrollTopMax() || _cornerButtons.replyReturn()) {
		return true;
	} else if (_inner->loadedAtBottomKnown()) {
		return !_inner->loadedAtBottom();
	}
	return std::nullopt;
}

bool ScheduledWidget::cornerButtonsUnreadMayBeShown() {
	return _inner->loadedAtBottomKnown()
		&& !_composeControls->isLockPresent()
		&& !_composeControls->isTTLButtonShown();
}

bool ScheduledWidget::cornerButtonsHas(CornerButtonType type) {
	return (type == CornerButtonType::Down);
}

void ScheduledWidget::showAtPosition(
		Data::MessagePosition position,
		FullMsgId originId) {
	_inner->showAtPosition(
		position,
		{},
		_cornerButtons.doneJumpFrom(position.fullId, originId));
}

void ScheduledWidget::updateAdaptiveLayout() {
	_topBarShadow->moveToLeft(
		controller()->adaptive().isOneColumn() ? 0 : st::lineWidth,
		_topBar->height());
}

not_null<History*> ScheduledWidget::history() const {
	return _history;
}

Dialogs::RowDescriptor ScheduledWidget::activeChat() const {
	return {
		_history,
		FullMsgId(_history->peer->id, ShowAtUnreadMsgId)
	};
}

bool ScheduledWidget::preventsClose(Fn<void()> &&continueCallback) const {
	return _composeControls->preventsClose(std::move(continueCallback));
}

QPixmap ScheduledWidget::grabForShowAnimation(const Window::SectionSlideParams &params) {
	_topBar->updateControlsVisibility();
	if (params.withTopBarShadow) _topBarShadow->hide();
	_composeControls->showForGrab();
	auto result = Ui::GrabWidget(this);
	if (params.withTopBarShadow) _topBarShadow->show();
	return result;
}

void ScheduledWidget::checkActivation() {
	_inner->checkActivation();
}

void ScheduledWidget::doSetInnerFocus() {
	_composeControls->focus();
}

bool ScheduledWidget::showInternal(
		not_null<Window::SectionMemento*> memento,
		const Window::SectionShow &params) {
	if (auto logMemento = dynamic_cast<ScheduledMemento*>(memento.get())) {
		if (logMemento->getHistory() == history()) {
			restoreState(logMemento);
			if (params.reapplyLocalDraft) {
				_composeControls->applyDraft(
					ComposeControls::FieldHistoryAction::NewEntry);
			}
			return true;
		}
	}
	return false;
}

void ScheduledWidget::setInternalState(
		const QRect &geometry,
		not_null<ScheduledMemento*> memento) {
	setGeometry(geometry);
	Ui::SendPendingMoveResizeEvents(this);
	restoreState(memento);
}

bool ScheduledWidget::pushTabbedSelectorToThirdSection(
		not_null<Data::Thread*> thread,
		const Window::SectionShow &params) {
	return _composeControls->pushTabbedSelectorToThirdSection(
		thread,
		params);
}

bool ScheduledWidget::returnTabbedSelector() {
	return _composeControls->returnTabbedSelector();
}

std::shared_ptr<Window::SectionMemento> ScheduledWidget::createMemento() {
	if (_forumTopic) {
		if (const auto forum = history()->asForum()) {
			const auto rootId = _forumTopic->topicRootId();
			if (const auto topic = forum->topicFor(rootId)) {
				auto result = std::make_shared<ScheduledMemento>(topic);
				saveState(result.get());
				return result;
			}
		}
	}
	auto result = std::make_shared<ScheduledMemento>(history());
	saveState(result.get());
	return result;
}

void ScheduledWidget::saveState(not_null<ScheduledMemento*> memento) {
	_inner->saveState(memento->list());
}

void ScheduledWidget::restoreState(not_null<ScheduledMemento*> memento) {
	_inner->restoreState(memento->list());
}

void ScheduledWidget::resizeEvent(QResizeEvent *e) {
	if (!width() || !height()) {
		return;
	}
	_composeControls->resizeToWidth(width());
	updateControlsGeometry();
}

void ScheduledWidget::updateControlsGeometry() {
	const auto contentWidth = width();

	const auto newScrollTop = _scroll->isHidden()
		? std::nullopt
		: base::make_optional(_scroll->scrollTop() + topDelta());
	_topBar->resizeToWidth(contentWidth);
	_topBarShadow->resize(contentWidth, st::lineWidth);

	const auto bottom = height();
	const auto controlsHeight = _composeControls->heightCurrent();
	const auto scrollHeight = bottom - _topBar->height() - controlsHeight;
	const auto scrollSize = QSize(contentWidth, scrollHeight);
	if (_scroll->size() != scrollSize) {
		_skipScrollEvent = true;
		_scroll->resize(scrollSize);
		_inner->resizeToWidth(scrollSize.width(), _scroll->height());
		_skipScrollEvent = false;
	}
	if (!_scroll->isHidden()) {
		if (newScrollTop) {
			_scroll->scrollToY(*newScrollTop);
		}
		updateInnerVisibleArea();
	}
	_composeControls->move(0, bottom - controlsHeight);
	_composeControls->setAutocompleteBoundingRect(_scroll->geometry());

	_cornerButtons.updatePositions();
}

void ScheduledWidget::paintEvent(QPaintEvent *e) {
	if (animatingShow()) {
		SectionWidget::paintEvent(e);
		return;
	} else if (controller()->contentOverlapped(this, e)) {
		return;
	}
	//if (hasPendingResizedItems()) {
	//	updateListSize();
	//}

	//auto ms = crl::now();
	//_historyDownShown.step(ms);

	const auto clip = e->rect();
	SectionWidget::PaintBackground(controller(), _theme.get(), this, clip);
}

void ScheduledWidget::onScroll() {
	if (_skipScrollEvent) {
		return;
	}
	updateInnerVisibleArea();
}

void ScheduledWidget::updateInnerVisibleArea() {
	if (!_inner->animatedScrolling()) {
		checkReplyReturns();
	}
	const auto scrollTop = _scroll->scrollTop();
	_inner->setVisibleTopBottom(scrollTop, scrollTop + _scroll->height());
	_cornerButtons.updateJumpDownVisibility();
	_cornerButtons.updateUnreadThingsVisibility();
}

void ScheduledWidget::showAnimatedHook(
		const Window::SectionSlideParams &params) {
	_topBar->setAnimatingMode(true);
	if (params.withTopBarShadow) {
		_topBarShadow->show();
	}
	_composeControls->showStarted();
}

void ScheduledWidget::showFinishedHook() {
	_topBar->setAnimatingMode(false);
	_composeControls->showFinished();
	_inner->showFinished();

	// We should setup the drag area only after
	// the section animation is finished,
	// because after that the method showChildren() is called.
	setupDragArea();
}

bool ScheduledWidget::floatPlayerHandleWheelEvent(QEvent *e) {
	return _scroll->viewportEvent(e);
}

QRect ScheduledWidget::floatPlayerAvailableRect() {
	return mapToGlobal(_scroll->geometry());
}

Context ScheduledWidget::listContext() {
	return _forumTopic ? Context::ScheduledTopic : Context::History;
}

bool ScheduledWidget::listScrollTo(int top, bool syntetic) {
	top = std::clamp(top, 0, _scroll->scrollTopMax());
	if (_scroll->scrollTop() == top) {
		updateInnerVisibleArea();
		return false;
	}
	_scroll->scrollToY(top);
	return true;
}

void ScheduledWidget::listCancelRequest() {
	if (_inner && !_inner->getSelectedItems().empty()) {
		clearSelected();
		return;
	} else if (_composeControls->handleCancelRequest()) {
		return;
	}
	controller()->showBackFromStack();
}

void ScheduledWidget::listDeleteRequest() {
	confirmDeleteSelected();
}

void ScheduledWidget::listTryProcessKeyInput(not_null<QKeyEvent*> e) {
	_composeControls->tryProcessKeyInput(e);
}

rpl::producer<Data::MessagesSlice> ScheduledWidget::listSource(
		Data::MessagePosition aroundId,
		int limitBefore,
		int limitAfter) {
	const auto session = &controller()->session();
	return rpl::single(rpl::empty) | rpl::then(
		session->scheduledMessages().updates(_history)
	) | rpl::map([=] {
		return _forumTopic
			? session->scheduledMessages().list(_forumTopic)
			: session->scheduledMessages().list(_history);
	}) | rpl::after_next([=](const Data::MessagesSlice &slice) {
		highlightSingleNewMessage(slice);
	});
}

void ScheduledWidget::highlightSingleNewMessage(
		const Data::MessagesSlice &slice) {
	const auto guard = gsl::finally([&] { _lastSlice = slice; });
	if (_lastSlice.ids.empty()
		|| (slice.ids.size() != _lastSlice.ids.size() + 1)) {
		return;
	}
	auto firstDifferent = 0;
	while (firstDifferent != _lastSlice.ids.size()) {
		if (slice.ids[firstDifferent] != _lastSlice.ids[firstDifferent]) {
			break;
		}
		++firstDifferent;
	}
	auto lastDifferent = slice.ids.size() - 1;
	while (lastDifferent != firstDifferent) {
		if (slice.ids[lastDifferent] != _lastSlice.ids[lastDifferent - 1]) {
			break;
		}
		--lastDifferent;
	}
	if (firstDifferent != lastDifferent) {
		return;
	}
	const auto newId = slice.ids[firstDifferent];
	if (const auto item = session().data().message(newId)) {
		showAtPosition(item->position());
	}
}

bool ScheduledWidget::listAllowsMultiSelect() {
	return true;
}

bool ScheduledWidget::listIsItemGoodForSelection(
		not_null<HistoryItem*> item) {
	return !item->isSending() && !item->hasFailed();
}

bool ScheduledWidget::listIsLessInOrder(
		not_null<HistoryItem*> first,
		not_null<HistoryItem*> second) {
	return first->position() < second->position();
}

void ScheduledWidget::listSelectionChanged(SelectedItems &&items) {
	HistoryView::TopBarWidget::SelectedState state;
	state.count = items.size();
	for (const auto &item : items) {
		if (item.canDelete) {
			++state.canDeleteCount;
		}
		if (item.canSendNow) {
			++state.canSendNowCount;
		}
	}
	_topBar->showSelected(state);
	if (items.empty()) {
		doSetInnerFocus();
	}
}

void ScheduledWidget::listMarkReadTill(not_null<HistoryItem*> item) {
}

void ScheduledWidget::listMarkContentsRead(
	const base::flat_set<not_null<HistoryItem*>> &items) {
}

MessagesBarData ScheduledWidget::listMessagesBar(
		const std::vector<not_null<Element*>> &elements) {
	return {};
}

void ScheduledWidget::listContentRefreshed() {
}

void ScheduledWidget::listUpdateDateLink(
	ClickHandlerPtr &link,
	not_null<Element*> view) {
}

bool ScheduledWidget::listElementHideReply(not_null<const Element*> view) {
	if (const auto root = view->data()->topicRootId()) {
		return root == view->data()->replyTo().messageId.msg;
	}
	return false;
}

bool ScheduledWidget::listElementShownUnread(not_null<const Element*> view) {
	return true;
}

bool ScheduledWidget::listIsGoodForAroundPosition(
		not_null<const Element*> view) {
	return true;
}

bool ScheduledWidget::showMessage(
		PeerId peerId,
		const Window::SectionShow &params,
		MsgId messageId) {
	if (peerId != _history->peer->id) {
		return false;
	}
	const auto id = FullMsgId(_history->peer->id, messageId);
	const auto message = _history->owner().message(id);
	if (!message || !_inner->viewByPosition(message->position())) {
		return false;
	}

	const auto originItem = [&]() -> HistoryItem* {
		using OriginMessage = Window::SectionShow::OriginMessage;
		if (const auto origin = std::get_if<OriginMessage>(&params.origin)) {
			if (const auto returnTo = session().data().message(origin->id)) {
				if (_inner->viewByPosition(returnTo->position())
					&& _cornerButtons.replyReturn() != returnTo) {
					return returnTo;
				}
			}
		}
		return nullptr;
	}();
	showAtPosition(
		message->position(),
		originItem ? originItem->fullId() : FullMsgId());
	return true;
}

Window::SectionActionResult ScheduledWidget::sendBotCommand(
		Bot::SendCommandRequest request) {
	if (request.peer != _history->peer) {
		return Window::SectionActionResult::Ignore;
	}
	listSendBotCommand(request.command, request.context);
	return Window::SectionActionResult::Handle;
}

void ScheduledWidget::listSendBotCommand(
		const QString &command,
		const FullMsgId &context) {
	const auto callback = [=](Api::SendOptions options) {
		const auto text = Bot::WrapCommandInChat(
			_history->peer,
			command,
			context);
		auto message = Api::MessageToSend(prepareSendAction(options));
		message.textWithTags = { text };
		session().api().sendMessage(std::move(message));
	};
	controller()->show(PrepareScheduleBox(this, sendMenuType(), callback));
}

void ScheduledWidget::listSearch(
		const QString &query,
		const FullMsgId &context) {
	const auto inChat = _history->peer->isUser()
		? Dialogs::Key()
		: Dialogs::Key(_history);
	controller()->searchMessages(query, inChat);
}

void ScheduledWidget::listHandleViaClick(not_null<UserData*> bot) {
	_composeControls->setText({ '@' + bot->username() + ' ' });
}

not_null<Ui::ChatTheme*> ScheduledWidget::listChatTheme() {
	return _theme.get();
}

CopyRestrictionType ScheduledWidget::listCopyRestrictionType(
		HistoryItem *item) {
	return CopyRestrictionType::None;
}

CopyRestrictionType ScheduledWidget::listCopyMediaRestrictionType(
		not_null<HistoryItem*> item) {
	if (const auto media = item->media()) {
		if (const auto invoice = media->invoice()) {
			if (invoice->extendedMedia) {
				return CopyMediaRestrictionTypeFor(_history->peer, item);
			}
		}
	}
	return CopyRestrictionType::None;
}

CopyRestrictionType ScheduledWidget::listSelectRestrictionType() {
	return CopyRestrictionType::None;
}

auto ScheduledWidget::listAllowedReactionsValue()
-> rpl::producer<Data::AllowedReactions> {
	return rpl::single(Data::AllowedReactions());
}

void ScheduledWidget::listShowPremiumToast(
		not_null<DocumentData*> document) {
	if (!_stickerToast) {
		_stickerToast = std::make_unique<HistoryView::StickerToast>(
			controller(),
			this,
			[=] { _stickerToast = nullptr; });
	}
	_stickerToast->showFor(document);
}

void ScheduledWidget::listOpenPhoto(
		not_null<PhotoData*> photo,
		FullMsgId context) {
	controller()->openPhoto(photo, { context });
}

void ScheduledWidget::listOpenDocument(
		not_null<DocumentData*> document,
		FullMsgId context,
		bool showInMediaView) {
	controller()->openDocument(document, showInMediaView, { context });
}

void ScheduledWidget::listPaintEmpty(
	Painter &p,
	const Ui::ChatPaintContext &context) {
}

QString ScheduledWidget::listElementAuthorRank(
		not_null<const Element*> view) {
	return {};
}

History *ScheduledWidget::listTranslateHistory() {
	return nullptr;
}

void ScheduledWidget::listAddTranslatedItems(
	not_null<TranslateTracker*> tracker) {
}

void ScheduledWidget::confirmSendNowSelected() {
	ConfirmSendNowSelectedItems(_inner);
}

void ScheduledWidget::confirmDeleteSelected() {
	ConfirmDeleteSelectedItems(_inner);
}

void ScheduledWidget::clearSelected() {
	_inner->cancelSelection();
}

void ScheduledWidget::setupDragArea() {
	const auto areas = DragArea::SetupDragAreaToContainer(
		this,
		[=](auto d) { return _history && !_composeControls->isRecording(); },
		nullptr,
		[=] { updateControlsGeometry(); });

	const auto droppedCallback = [=](bool overrideSendImagesAsPhotos) {
		return [=](const QMimeData *data) {
			confirmSendingFiles(data, overrideSendImagesAsPhotos);
			Window::ActivateWindow(controller());
		};
	};
	areas.document->setDroppedCallback(droppedCallback(false));
	areas.photo->setDroppedCallback(droppedCallback(true));
}

} // namespace HistoryView
