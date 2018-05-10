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
#include "SPPlatform.h"
#include "SPThread.h"

#if (LINUX)

NS_SP_PLATFORM_BEGIN

namespace interaction {
	bool _dialogOpened = false;
	void _goToUrl(const StringView &url, bool external) {
		stappler::log::format("Intercation", "GoTo url: %s", url.data());
	}
	void _makePhoneCall(const StringView &str) {
		stappler::log::format("Intercation", "phone: %s", str.data());
	}
	void _mailTo(const StringView &address) {
		stappler::log::format("Intercation", "MailTo phone: %s", address.data());
	}
	void _backKey() { }
	void _notification(const StringView &title, const StringView &text) {

	}
	void _rateApplication() {
		stappler::log::text("Intercation", "Rate app");
	}

	void _openFileDialog(const String &path, const Function<void(const String &)> &func) {
		if (func) {
			func("");
		}
	}
}

NS_SP_PLATFORM_END

#endif
