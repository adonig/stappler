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
#include "platform/CCDevice.h"

#include "SPPlatform.h"
#include "SPScreenOrientation.h"

#include "math/CCGeometry.h"

#if (CYGWIN)

#ifndef SP_RESTRICT
NS_SP_PLATFORM_BEGIN

namespace desktop {
	void setScreenSize(const cocos2d::Size &size);
	cocos2d::Size getScreenSize();
	bool isTablet();
	std::string getPackageName();
	float getDensity();
	std::string getUserLanguage();
}

NS_SP_PLATFORM_END
#endif

NS_SP_PLATFORM_BEGIN

namespace device {
	bool _isTablet() {
#ifndef SP_RESTRICT
		return desktop::isTablet();
#else
		return true;
#endif
	}
	std::string _userAgent() {
		return "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:41.0) Gecko/20100101 Firefox/41.0";
	}
	std::string _deviceIdentifier() {
		return "Linux Desktop";
	}
	std::string _bundleName() {
#ifndef SP_RESTRICT
		return desktop::getPackageName();
#else
		return "org.stappler.stappler";
#endif
	}
	std::string _userLanguage() {
#ifndef SP_RESTRICT
		return desktop::getUserLanguage();
#else
		return "en-us";
#endif
	}
	std::string _applicationName() {
		return "Linux App";
	}
	std::string _applicationVersion() {
		return "2.0";
	}
	cocos2d::Size _screenSize() {
#ifndef SP_RESTRICT
		auto size =  desktop::getScreenSize();
		if (size.width <= size.height) {
			return size;
		} else {
			return cocos2d::Size(size.height, size.width);
		}
#else
		return cocos2d::Size(1024, 768);
#endif
	}
	cocos2d::Size _viewportSize(const cocos2d::Size &screenSize, bool isTablet) {
#ifndef SP_RESTRICT
		auto size =  desktop::getScreenSize();
		if (size.width <= size.height) {
			return size;
		} else {
			return cocos2d::Size(size.height, size.width);
		}
#else
		return cocos2d::Size(1024, 768);
#endif
	}
	float _designScale(const cocos2d::Size &screenSize, bool isTablet) {
		return 1.0f;
	}
	const ScreenOrientation &_currentDeviceOrientation() {
#ifndef SP_RESTRICT
		auto size = desktop::getScreenSize();
		if (size.width > size.height) {
			return ScreenOrientation::LandscapeLeft;
		} else {
			return ScreenOrientation::PortraitTop;
		}
#else
		return ScreenOrientation::LandscapeLeft;
#endif
	}
	std::pair<uint64_t, uint64_t> _diskSpace() {
		uint64_t totalSpace = 0;
		uint64_t totalFreeSpace = 0;

		return std::make_pair(totalSpace, totalFreeSpace);
	}
	int _dpi() {
#ifdef SP_RESTRICT
		return 92;
#else
		return 92 * desktop::getDensity();
#endif
	}

	void _onDirectorStarted() { }
	float _density() {
#ifdef SP_RESTRICT
		return 1.0f;
#else
		return desktop::getDensity();
#endif
	}
}

NS_SP_PLATFORM_END

#endif
