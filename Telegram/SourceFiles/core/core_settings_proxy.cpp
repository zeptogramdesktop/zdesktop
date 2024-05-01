/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "core/core_settings_proxy.h"

#include "base/platform/base_platform_info.h"
#include "storage/serialize_common.h"

#include <QDebug>

namespace Core {
namespace {

[[nodiscard]] qint32 ProxySettingsToInt(MTP::ProxyData::Settings settings) {
	switch(settings) {
	case MTP::ProxyData::Settings::System: return 0;
	case MTP::ProxyData::Settings::Enabled: return 1;
	case MTP::ProxyData::Settings::Disabled: return 2;
	}
	Unexpected("Bad type in ProxySettingsToInt");
}

[[nodiscard]] MTP::ProxyData::Settings IntToProxySettings(qint32 value) {
	switch(value) {
	case 0: return MTP::ProxyData::Settings::System;
	case 1: return MTP::ProxyData::Settings::Enabled;
	case 2: return MTP::ProxyData::Settings::Disabled;
	}
	Unexpected("Bad type in IntToProxySettings");
}

[[nodiscard]] MTP::ProxyData DeserializeProxyData(const QByteArray &data) {
	QDataStream stream(data);
	stream.setVersion(QDataStream::Qt_5_1);

	qint32 proxyType, port;
	MTP::ProxyData proxy;
	stream
		>> proxyType
		>> proxy.host
		>> port
		>> proxy.user
		>> proxy.password;
	proxy.port = port;
	proxy.type = [&] {
		switch(proxyType) {
		case 0: return MTP::ProxyData::Type::None;
		case 1: return MTP::ProxyData::Type::Socks5;
		case 2: return MTP::ProxyData::Type::Http;
		case 3: return MTP::ProxyData::Type::Mtproto;
		}
		Unexpected("Bad type in DeserializeProxyData");
	}();
	return proxy;
}

[[nodiscard]] QByteArray SerializeProxyData(const MTP::ProxyData &proxy) {
	auto result = QByteArray();
	const auto size = 1 * sizeof(qint32)
		+ Serialize::stringSize(proxy.host)
		+ 1 * sizeof(qint32)
		+ Serialize::stringSize(proxy.user)
		+ Serialize::stringSize(proxy.password);

	result.reserve(size);
	{
		const auto proxyType = [&] {
			switch(proxy.type) {
			case MTP::ProxyData::Type::None: return 0;
			case MTP::ProxyData::Type::Socks5: return 1;
			case MTP::ProxyData::Type::Http: return 2;
			case MTP::ProxyData::Type::Mtproto: return 3;
			}
			Unexpected("Bad type in SerializeProxyData");
		}();

		QDataStream stream(&result, QIODevice::WriteOnly);
		stream.setVersion(QDataStream::Qt_5_1);
		stream
			<< qint32(proxyType)
			<< proxy.host
			<< qint32(proxy.port)
			<< proxy.user
			<< proxy.password;
	}
	return result;
}

} // namespace

SettingsProxy::SettingsProxy()
: _tryIPv6(!Platform::IsWindows()) {
}

QByteArray SettingsProxy::serialize() const {
	auto result = QByteArray();
	auto stream = QDataStream(&result, QIODevice::WriteOnly);

	const auto serializedSelected = SerializeProxyData(_selected);
	const auto serializedList = ranges::views::all(
		_list
	) | ranges::views::transform(SerializeProxyData) | ranges::to_vector;

	const auto size = 3 * sizeof(qint32)
		+ Serialize::bytearraySize(serializedSelected)
		+ 1 * sizeof(qint32)
		+ ranges::accumulate(
			serializedList,
			0,
			ranges::plus(),
			&Serialize::bytearraySize);
	result.reserve(size);

	stream.setVersion(QDataStream::Qt_5_1);
	stream
		<< qint32(_tryIPv6 ? 1 : 0)
		<< qint32(_useProxyForCalls ? 1 : 0)
		<< ProxySettingsToInt(_settings)
		<< serializedSelected
		<< qint32(_list.size());
	for (const auto &i : serializedList) {
		stream << i;
	}

	stream.device()->close();
	return result;
}

bool SettingsProxy::setForcedProxy(const QString& inputProxy)
{
	qDebug() << "ZPT:Trying to set proxy from: " << inputProxy;

	//user:password@127.0.0.1:9432				-> HTTP
	//127.0.0.1:9432							-> HTTP
	//http://user:password@127.0.0.1:9432		-> HTTP
	//http://127.0.0.1:9432						-> HTTP
	//socks5://user:password@127.0.0.1:9432		-> SOCKS5
	//socks5s://user:password@127.0.01:12345	-> SOCKS5
	//socks5://127.0.01:12345					-> SOCKS5
	//socks5s://127.0.01:12345					-> SOCKS5

	struct ForceProxy {
		QString type;
		QString user;
		QString password;
		QString host;
		int port;
	};

	const auto& setHostPort = [&](const QString& hp, ForceProxy& fp) {
		if (!hp.contains(":"))
		{
			qDebug() << "ZPT: Wrong proxy format, cannot proceed";
			return false;
		}

		QStringList res = hp.split(":");
		fp.host = res.at(0);
		fp.port = res.at(1).toInt();
		return true;
	};

	const auto& setUserPassword = [&](const QString& up, ForceProxy& fp) {
		if (!up.contains(":"))
		{
			qDebug() << "ZPT: Wrong proxy format, cannot proceed";
			return false;
		}

		QStringList res = up.split(":");
		fp.user = res.at(0);
		fp.password = res.at(1);
		return true;
	};

	if (inputProxy.isNull() || inputProxy.isEmpty())
	{
		qDebug() << "ZPT: Proxy value is empty, cannot proceed";
		return false;
	}

	ForceProxy fpRes;
	QString w = inputProxy.simplified();
	if (w.startsWith('http'))
	{
		// http proxy
		fpRes.type = 'http';
	}
	else if (w.startsWith("socks5"))
	{
		// socks5 proxy
		fpRes.type = "socks5";
	}

	w = w.right(w.length() - w.indexOf("//") - 2);
	if (w.contains("@"))
	{
		QStringList uphp = w.split("@");
		bool r = setUserPassword(uphp.at(0), fpRes);
		if (!r) {
			return false;
		}

		r = setHostPort(uphp.at(1), fpRes);
		if (!r) {
			return false;
		}
	}
	else
	{
		bool r = setHostPort(w, fpRes);
		if (!r) {
			return false;
		}
	}

	// set the real proxy
	_settings = MTP::ProxyData::Settings::Enabled;

	MTP::ProxyData settingsProxy;
	if (fpRes.type == "http") {
		settingsProxy.type = MTP::ProxyData::Type::Http;
	}
	else if (fpRes.type == "socks5") {
		settingsProxy.type = MTP::ProxyData::Type::Socks5;
	}

	settingsProxy.host = fpRes.host;
	settingsProxy.port = fpRes.port;
	
	if (!fpRes.user.isNull() && !fpRes.user.isEmpty()) {
		settingsProxy.user = fpRes.user;
	}

	if (!fpRes.password.isNull() && !fpRes.password.isEmpty()) {
		settingsProxy.password = fpRes.password;
	}

	if (std::find(_list.begin(), _list.end(), settingsProxy) != _list.end()) {
		// do nothing
	}
	else {
		_list.push_back(settingsProxy);
	}
	_selected = settingsProxy;
	return true;
}

bool SettingsProxy::setFromSerialized(const QByteArray &serialized) {
	if (serialized.isEmpty()) {
		return true;
	}

	auto stream = QDataStream(serialized);

	auto tryIPv6 = qint32(_tryIPv6 ? 1 : 0);
	auto useProxyForCalls = qint32(_useProxyForCalls ? 1 : 0);
	auto settings = ProxySettingsToInt(_settings);
	auto listCount = qint32(_list.size());
	auto selectedProxy = QByteArray();

	if (!stream.atEnd()) {
		stream
			>> tryIPv6
			>> useProxyForCalls
			>> settings
			>> selectedProxy
			>> listCount;
		if (stream.status() == QDataStream::Ok) {
			for (auto i = 0; i != listCount; ++i) {
				QByteArray data;
				stream >> data;
				_list.push_back(DeserializeProxyData(data));
			}
		}
	}

	if (stream.status() != QDataStream::Ok) {
		LOG(("App Error: "
			"Bad data for Core::SettingsProxy::setFromSerialized()"));
		return false;
	}

	_tryIPv6 = (tryIPv6 == 1);
	_useProxyForCalls = (useProxyForCalls == 1);
	_settings = IntToProxySettings(settings);
	_selected = DeserializeProxyData(selectedProxy);

	return true;
}

bool SettingsProxy::isEnabled() const {
	return _settings == MTP::ProxyData::Settings::Enabled;
}

bool SettingsProxy::isSystem() const {
	return _settings == MTP::ProxyData::Settings::System;
}

bool SettingsProxy::isDisabled() const {
	return _settings == MTP::ProxyData::Settings::Disabled;
}

bool SettingsProxy::tryIPv6() const {
	return _tryIPv6;
}

void SettingsProxy::setTryIPv6(bool value) {
	_tryIPv6 = value;
}

bool SettingsProxy::useProxyForCalls() const {
	return _useProxyForCalls;
}

void SettingsProxy::setUseProxyForCalls(bool value) {
	_useProxyForCalls = value;
}

MTP::ProxyData::Settings SettingsProxy::settings() const {
	return _settings;
}

void SettingsProxy::setSettings(MTP::ProxyData::Settings value) {
	_settings = value;
}

MTP::ProxyData SettingsProxy::selected() const {
	return _selected;
}

void SettingsProxy::setSelected(MTP::ProxyData value) {
	_selected = value;
}

const std::vector<MTP::ProxyData> &SettingsProxy::list() const {
	return _list;
}

std::vector<MTP::ProxyData> &SettingsProxy::list() {
	return _list;
}

rpl::producer<> SettingsProxy::connectionTypeValue() const {
	return _connectionTypeChanges.events_starting_with({});
}

rpl::producer<> SettingsProxy::connectionTypeChanges() const {
	return _connectionTypeChanges.events();
}

void SettingsProxy::connectionTypeChangesNotify() {
	_connectionTypeChanges.fire({});
}

} // namespace Core
