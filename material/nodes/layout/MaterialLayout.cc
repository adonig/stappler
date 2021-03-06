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

#include "Material.h"
#include "MaterialLayout.h"
#include "MaterialScene.h"

NS_MD_BEGIN

bool Layout::onBackButton() {
	if (_backButtonCallback) {
		return _backButtonCallback();
	} else {
		auto scene = Scene::getRunningScene();
		auto content = scene->getContentLayer();
		if (content->getNodesCount() >= 2 && content->getRunningNode() == this) {
			content->popLastNode();
			return true;
		}
	}
	return false;
}

void Layout::setBackButtonCallback(const BackButtonCallback &cb) {
	_backButtonCallback = cb;
}
const Layout::BackButtonCallback &Layout::getBackButtonCallback() const {
	return _backButtonCallback;
}

void Layout::onPush(ContentLayer *l, bool replace) {
	_inTransition = true;
}
void Layout::onPushTransitionEnded(ContentLayer *l, bool replace) {
	_inTransition = false;
	_contentSizeDirty = true;
}

void Layout::onPopTransitionBegan(ContentLayer *l, bool replace) {
	_inTransition = true;
}
void Layout::onPop(ContentLayer *l, bool replace) {
	_inTransition = false;
	_contentSizeDirty = true;
}

void Layout::onBackground(ContentLayer *l, Layout *overlay) {
	_inTransition = true;
}
void Layout::onBackgroundTransitionEnded(ContentLayer *l, Layout *overlay) {
	_inTransition = false;
	_contentSizeDirty = true;
}

void Layout::onForegroundTransitionBegan(ContentLayer *l, Layout *overlay) {
	_inTransition = true;
}
void Layout::onForeground(ContentLayer *l, Layout *overlay) {
	_inTransition = false;
	_contentSizeDirty = true;
}

Rc<Layout::Transition> Layout::getDefaultEnterTransition() const {
	return nullptr;
}
Rc<Layout::Transition> Layout::getDefaultExitTransition() const {
	return nullptr;
}

NS_MD_END
