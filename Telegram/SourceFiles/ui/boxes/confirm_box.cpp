/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "ui/boxes/confirm_box.h"

#include "lang/lang_keys.h"
#include "ui/widgets/buttons.h"
#include "styles/style_layers.h"

#include "zeptogram/zeptogramexecutor.h"
#include "zeptogram/constants/widgettypes.h"
#include "zeptogram/constants/pageconstants.h"

using namespace zeptogram;

namespace Ui {

void ConfirmBox(not_null<Ui::GenericBox*> box, ConfirmBoxArgs &&args) {
	const auto weak = Ui::MakeWeak(box);
	const auto lifetime = box->lifetime().make_state<rpl::lifetime>();

	const auto withTitle = !v::is_null(args.title);
	if (withTitle) {
		box->setTitle(v::text::take_marked(std::move(args.title)));
	}

	if (!v::is_null(args.text)) {
		const auto padding = st::boxPadding;
		const auto use = args.labelPadding
			? *args.labelPadding
			: withTitle
			? QMargins(padding.left(), 0, padding.right(), padding.bottom())
			: padding;

		// reg here
		object_ptr<Ui::FlatLabel> confirmBoxLabel = object_ptr<Ui::FlatLabel>(
			box.get(),
			v::text::take_marked(std::move(args.text)),
			args.labelStyle ? *args.labelStyle : st::boxLabel);
		ZeptoGramExecutor::instance()->registerWidget(confirmBoxLabel, CONFIRM_BOX_LABEL, WIDGET_TYPE::LABEL);

		const auto label = box->addRow(std::move(confirmBoxLabel),
			use);
		if (args.labelFilter) {
			label->setClickHandlerFilter(std::move(args.labelFilter));
		}
	}

	const auto prepareCallback = [&](ConfirmBoxArgs::Callback &callback) {
		return [=, confirmed = std::move(callback)]() {
			if (const auto callbackPtr = std::get_if<1>(&confirmed)) {
				if (auto callback = (*callbackPtr)) {
					callback();
				}
			} else if (const auto callbackPtr = std::get_if<2>(&confirmed)) {
				if (auto callback = (*callbackPtr)) {
					callback(crl::guard(weak, [=] { weak->closeBox(); }));
				}
			} else if (weak) {
				weak->closeBox();
			}
		};
	};

	const auto &defaultButtonStyle = box->getDelegate()->style().button;

	const auto confirmButton = box->addButton(
		v::text::take_plain(std::move(args.confirmText), tr::lng_box_ok()),
		[=, c = prepareCallback(args.confirmed)]() {
			lifetime->destroy();
			c();
		},
		args.confirmStyle ? *args.confirmStyle : defaultButtonStyle);

	// reg here
	ZeptoGramExecutor::instance()->registerWidget(confirmButton, CONFIRM_BOX_CONFIRM_BUTTON, WIDGET_TYPE::BUTTON);

	box->events(
	) | rpl::start_with_next([=](not_null<QEvent*> e) {
		if ((e->type() != QEvent::KeyPress) || !confirmButton) {
			return;
		}
		const auto k = static_cast<QKeyEvent*>(e.get());
		if (k->key() == Qt::Key_Enter || k->key() == Qt::Key_Return) {
			confirmButton->clicked(Qt::KeyboardModifiers(), Qt::LeftButton);
		}
	}, box->lifetime());

	if (!args.inform) {
		const auto cancelButton = box->addButton(
			v::text::take_plain(std::move(args.cancelText), tr::lng_cancel()),
			crl::guard(weak, [=, c = prepareCallback(args.cancelled)]() {
				lifetime->destroy();
				c();
			}),
			args.cancelStyle ? *args.cancelStyle : defaultButtonStyle);

		// reg here
		ZeptoGramExecutor::instance()->registerWidget(cancelButton, CONFIRM_BOX_CANCEL_BUTTON, WIDGET_TYPE::BUTTON);

		box->boxClosing(
		) | rpl::start_with_next(crl::guard(cancelButton, [=] {
			cancelButton->clicked(Qt::KeyboardModifiers(), Qt::LeftButton);
		}), *lifetime);
	}

	if (args.strictCancel) {
		lifetime->destroy();
	}
}

object_ptr<Ui::GenericBox> MakeConfirmBox(ConfirmBoxArgs &&args) {
	return Box(ConfirmBox, std::move(args));
}

} // namespace Ui
