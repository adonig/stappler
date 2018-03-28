// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/**
Copyright (c) 2016 Roman Katuntsev <sbkarr@stappler.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
**/

#include "SPDefine.h"
#include "SPLocale.h"

NS_SP_EXT_BEGIN(locale)

class LocaleManager {
public:
	static LocaleManager *s_sharedInstance;

	static LocaleManager *getInstance() {
		if (!s_sharedInstance) {
			s_sharedInstance = new LocaleManager();
		}
		return s_sharedInstance;
	}

	LocaleManager() : _defaultTime{
		"today",
		"yesterday",
		"jan", "feb", "mar", "apr", "may", "jun",
		"jul", "aug", "sep", "oct", "nov", "dec"
	} { }

	void define(const String &locale, LocaleInitList &&init) {
		auto it = _strings.find(locale);
		if (it != _strings.end()) {
			for (auto &iit : init) {
				it->second.emplace(iit.first, iit.second);
			}
		} else {
			auto it = _strings.emplace(locale, StringMap()).first;
			for (auto &iit : init) {
				it->second.emplace(iit.first, iit.second);
			}
		}
	}

	void define(const String &locale, const std::array<StringView, toInt(TimeTokens::Max)> &arr) {
		auto it = _timeTokens.find(locale);
		if (it == _timeTokens.end()) {
			it = _timeTokens.emplace(locale, std::array<String, toInt(TimeTokens::Max)>()).first;
		}

		size_t i = 0;
		for (auto &arr_it : arr) {
			it->second[i] = arr_it.str();
			++ i;
		}
	}

	String string(const String &str) {
		auto it = _strings.find(_locale);
		if (it == _strings.end()) {
			it = _strings.find(_default);
		}
		if (it == _strings.end()) {
			it = _strings.begin();
		}

		if (it != _strings.end()) {
			auto sit = it->second.find(str);
			if (sit != it->second.end()) {
				return sit->second;
			}
		}

		return String();
	}

	String numeric(const String &str, int64_t num) {
		auto ruleIt = _numRules.find(_locale);
		if (ruleIt == _numRules.end()) {
			auto fmt = string(str);
			StringView r(fmt);
			return r.readUntil<StringView::Chars<':'>>().str();
		} else {
			uint8_t numEq = ruleIt->second(num);
			auto fmt = string(str);
			StringView r(fmt);
			StringView def = r.readUntil<StringView::Chars<':'>>();
			if (r.empty() || numEq == 0) {
				return def.str();
			}

			while (!r.empty()) {
				if (r.is(':')) {
					++ r;
				}

				StringView res = r.readUntil<StringView::Chars<':'>>();
				if (numEq == 1) {
					return res.str();
				} else {
					-- numEq;
				}
			}

			return def.str();
		}
	}

	void setDefault(const String &def) {
		_default = def;
	}
	const String &getDefault() {
		return _default;
	}

	void setLocale(const String &loc) {
		if (_locale != loc) {
			_locale = loc;
			onLocale(nullptr, loc);
		}
	}
	const String &getLocale() {
		return _locale;
	}

	void setNumRule(const String &locale, const NumRule &rule) {
		_numRules.emplace(locale, rule);
	}

	bool hasLocaleTags(const char16_t *str, size_t len) {
		if (len == 0) {
			return false;
		}

		WideStringView r(str, len);
		if (r.is(u"@Locale:")) { // raw locale string
			return true;
		} else {
			r.skipUntil<WideStringView::Chars<'%'>>();
			if (r.is('%')) {
				++ r;
				r.skipChars<
					WideStringView::CharGroup<CharGroupId::Alphanumeric>,
					WideStringView::Chars<':', '.', '-', '_', '[', ']', '+'>>();
				if (r.is('%')) {
					return true;
				}
			}
		}

		return false;
	}

	WideString resolveLocaleTags(const char16_t *str, size_t len) {
		WideStringView r(str, len);
		if (r.is(u"@Locale:")) { // raw locale string
			auto name = string::toUtf8(std::u16string(str + "@Locale:"_len, len - "@Locale:"_len));
			return string::toUtf16(string(name));
		} else {
			std::u16string ret; ret.reserve(len);
			while (!r.empty()) {
				auto tmp = r.readUntil<WideStringView::Chars<'%'>>();
				ret.append(tmp.data(), tmp.size());
				if (r.is('%')) {
					++ r;
					auto token = r.readChars<
						WideStringView::CharGroup<CharGroupId::Alphanumeric>,
						WideStringView::Chars<':', '.', '-', '_', '[', ']', '+'>>();
					if (r.is('%')) {
						++ r;

						std::u16string replacement;
						if (token.is(u"Num:")) {
							WideStringView splitMaster(token);
							WideStringView num;
							while (!splitMaster.empty()) {
								num = splitMaster.readUntil<WideStringView::Chars<':'>>();
								if (splitMaster.is(':')) {
									++ splitMaster;
								}
							}

							if (!num.empty()) {
								WideStringView validate(num);
								if (validate.is('-')) {
									++ validate;
								}
								validate.skipChars<WideStringView::CharGroup<CharGroupId::Numbers>>();
								if (validate.empty()) {
									WideStringView vtoken(token.data(), token.size() - num.size() - 1);
									replacement = string::toUtf16(numeric(string::toUtf8(vtoken.str()), num.readInteger()));
								}
							}
						}

						if (replacement.empty()) {
							replacement = string::toUtf16(string(string::toUtf8(token.str())));
						}

						if (replacement.empty()) {
							ret.push_back(u'%');
							ret.append(token.data(), token.size());
							ret.push_back(u'%');
						} else {
							ret.append(replacement);
						}
					} else {
						ret.push_back(u'%');
						ret.append(token.data(), token.size());
					}
				}
			}
			return ret;
		}
		return std::u16string();
	}

	String language(const String &locale) {
		if (locale == "ru-ru") {
			return "Русский";
		} else if (locale.compare(0, 3, "en-") == 0) {
			return "English";
		}
		return String();
	}

	String common(const String &locale) {
		String ret = string::tolower(locale);
		if (ret.size() == 2) {
			if (ret == "en") {
				ret.reserve(5);
				ret.append("-gb");
			} else {
				ret.reserve(5);
				ret.append("-").append(string::tolower(locale));
			}
		}

		return ret;
	}

	String timeToken(TimeTokens tok) {
		auto it = _timeTokens.find(_locale);
		if (it == _timeTokens.end()) {
			it = _timeTokens.find(_default);
		}

		auto &table = it == _timeTokens.end()?_defaultTime:it->second;
		return table[toInt(tok)];
	}

	const std::array<String, toInt(TimeTokens::Max)> &timeTokenTable() {
		auto it = _timeTokens.find(_locale);
		if (it == _timeTokens.end()) {
			it = _timeTokens.find(_default);
		}

		return it == _timeTokens.end()?_defaultTime:it->second;
	}

protected:
	String _default;
	String _locale;

	LocaleMap _strings;
	std::unordered_map<String, NumRule> _numRules;

	Map<String, std::array<String, toInt(TimeTokens::Max)>> _timeTokens;
	std::array<String, toInt(TimeTokens::Max)> _defaultTime;
};

LocaleManager *LocaleManager::s_sharedInstance = nullptr;

EventHeader onLocale("Locale", "onLocale");

Initializer::Initializer(const String &locale, LocaleInitList && list) {
	LocaleManager::getInstance()->define(locale, std::move(list));
}

void define(const String &locale, LocaleInitList &&init) {
	LocaleManager::getInstance()->define(locale, std::move(init));
}

void define(const String &locale, const std::array<StringView, toInt(TimeTokens::Max)> &arr) {
	LocaleManager::getInstance()->define(locale, arr);
}

String string(const String &str) {
	return LocaleManager::getInstance()->string(str);
}

String numeric(const String &str, int64_t num) {
	return LocaleManager::getInstance()->numeric(str, num);
}

void setDefault(const String &def) {
	LocaleManager::getInstance()->setDefault(def);
}
const String &getDefault() {
	return LocaleManager::getInstance()->getDefault();
}

void setLocale(const String &loc) {
	LocaleManager::getInstance()->setLocale(loc);
}
const String &getLocale() {
	return LocaleManager::getInstance()->getLocale();
}

void setNumRule(const String &locale, const NumRule &rule) {
	LocaleManager::getInstance()->setNumRule(locale, rule);
}

bool hasLocaleTags(const char16_t *str, size_t len) {
	return LocaleManager::getInstance()->hasLocaleTags(str, len);
}

WideString resolveLocaleTags(const char16_t *str, size_t len) {
	return LocaleManager::getInstance()->resolveLocaleTags(str, len);
}

String language(const String &locale) {
	return LocaleManager::getInstance()->language(locale);
}

String common(const String &locale) {
	return LocaleManager::getInstance()->common(locale);
}

String timeToken(TimeTokens tok) {
	return LocaleManager::getInstance()->timeToken(tok);
}

static bool isToday(struct tm &tm, struct tm &now) {
	return tm.tm_year == now.tm_year && tm.tm_yday == now.tm_yday;
}

static bool isYesterday(struct tm &tm, struct tm &now) {
	if (now.tm_yday > 0) {
		return tm.tm_year == now.tm_year && tm.tm_yday == now.tm_yday - 1;
	} else if (now.tm_year > 0) {
		if ((tm.tm_year & 3) || (((tm.tm_year % 100) == 0) && (((tm.tm_year % 400) != 100)))) {
			return tm.tm_year == now.tm_year - 1 && tm.tm_yday == 354;
		} else {
			return tm.tm_year == now.tm_year - 1 && tm.tm_yday == 355;
		}
	}
	return false;
}

template <typename T>
static String localDate_impl(const std::array<T, toInt(TimeTokens::Max)> &table, Time t) {
	auto sec_now = time_t(Time::now().toSeconds());
	struct tm tm_now;
	localtime_r(&sec_now, &tm_now);

	auto sec_time = time_t(t.toSeconds());
	struct tm tm_time;
	localtime_r(&sec_time, &tm_time);

	if (isToday(tm_time, tm_now)) {
		return String(table[toInt(TimeTokens::Today)].data(), table[toInt(TimeTokens::Today)].size());
	} else if (isYesterday(tm_time, tm_now)) {
		return String(table[toInt(TimeTokens::Yesterday)].data(), table[toInt(TimeTokens::Yesterday)].size());
	}
	if (tm_time.tm_year == tm_now.tm_year) {
		return toString(tm_time.tm_mday, " ", table[tm_time.tm_mon + 2]);
	} else {
		return toString(tm_time.tm_mday, " ", table[tm_time.tm_mon + 2], " ", 1900 + tm_time.tm_year);
	}
}

String localDate(Time t) {
	return localDate_impl(LocaleManager::getInstance()->timeTokenTable(), t);
}

String localDate(const std::array<StringView, toInt(TimeTokens::Max)> &table, Time t) {
	return localDate_impl(table, t);
}

NS_SP_EXT_END(locale)
