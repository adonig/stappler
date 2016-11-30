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
#include "SPDynamicLabel.h"
#include "SPString.h"
#include "SPLocale.h"
#include "SPFontFormatter.h"
#include "SPGLProgramSet.h"
#include "SPLayer.h"
#include "SPEventListener.h"
#include "SPTextureCache.h"

#include "renderer/CCGLProgram.h"
#include "renderer/CCGLProgramState.h"
#include "renderer/CCGLProgramCache.h"

NS_SP_BEGIN

DynamicLabel::~DynamicLabel() { }

bool DynamicLabel::init(Source *source, const DescriptionStyle &style, const String &str, float width, Alignment alignment, float density) {
    if (!LayeredBatchNode::init(density)) {
    	return false;
    }

    _source = source;
    _style = style;

    auto el = Rc<EventListener>::create();
    el->onEventWithObject(Source::onTextureUpdated, source, std::bind(&DynamicLabel::onTextureUpdated, this));
    addComponent(el);

    _listener = el;

    setColor(_style.text.color);
    setOpacity(_style.text.opacity);

    setNormalized(true);

    setString(str);
    setWidth(width);
    setAlignment(alignment);

    updateLabel();

    return true;
}

void DynamicLabel::setStyle(const DescriptionStyle &style) {
	_style = style;

    setColor(_style.text.color);
    setOpacity(_style.text.opacity);

	_labelDirty = true;
}

const DynamicLabel::DescriptionStyle &DynamicLabel::getStyle() const {
	return _style;
}

void DynamicLabel::setSource(Source *source) {
	if (source != _source) {
		_source = source;
		_textures.clear();
		_colorMap.clear();
		_format = nullptr;
		_labelDirty = true;
	}
}

DynamicLabel::Source *DynamicLabel::getSource() const {
	return _source;
}

void DynamicLabel::updateLabel() {
	if (!_source) {
		return;
	}

	if (_string16.empty()) {
		_format = nullptr;
		_formatDirty = true;
		return;
	}

	_compiledStyles = compileStyle();
	_format = Rc<FormatSpec>::create(_string16.size(), _compiledStyles.size() + 1);

	_style.text.color = _displayedColor;
	_style.text.opacity = _displayedOpacity;
	_style.text.whiteSpace = rich_text::style::WhiteSpace::PreWrap;

	font::Formatter formatter(_source, _format, _density);
	formatter.setWidth((uint16_t)roundf(_width * _density));
	formatter.setTextAlignment(_alignment);
	formatter.setMaxWidth((uint16_t)roundf(_maxWidth * _density));
	formatter.setMaxLines(_maxLines);
	formatter.setOpticalAlignment(_opticalAlignment);
	formatter.setFillerChar(_fillerChar);

	if (_lineHeight != 0.0f) {
		if (_isLineHeightAbsolute) {
			formatter.setLineHeightAbsolute((uint16_t)(_lineHeight * _density));
		} else {
			formatter.setLineHeightRelative(_lineHeight);
		}
	}

	formatter.begin((uint16_t)roundf(_textIndent * _density));

	size_t drawedChars = 0;
	for (auto &it : _compiledStyles) {
		DescriptionStyle params = _style.merge(_source, it.style);

		auto start = _string16.c_str() + it.start;
		auto len = it.length;

		if (_localeEnabled && hasLocaleTags(start, len)) {
			WideString str(resolveLocaleTags(start, len));

			start = str.c_str();
			len = str.length();

			if (_maxChars > 0 && drawedChars + len > _maxChars) {
				len = _maxChars - drawedChars;
			}
			if (!formatter.read(params.font, params.text, start, len)) {
				drawedChars += len;
				break;
			}
		} else {
			if (_maxChars > 0 && drawedChars + len > _maxChars) {
				len = _maxChars - drawedChars;
			}
			if (!formatter.read(params.font, params.text, start, len)) {
				drawedChars += len;
				break;
			}
		}

		_format->ranges.back().colorDirty = params.colorDirty;
		_format->ranges.back().opacityDirty = params.opacityDirty;
	}
	formatter.finalize();

	setContentSize(cocos2d::Size(_format->width / _density, _format->height / _density));

	_labelDirty = false;
	_colorDirty = false;
	_formatDirty = true;
}

void DynamicLabel::visit(cocos2d::Renderer *r, const Mat4& t, uint32_t f, ZPath &zPath) {
	if (_labelDirty) {
		updateLabel();
	}

	if (_formatDirty) {
		updateQuads(f);
	}
	if (_colorDirty) {
		updateColorQuads();
	}
	LayeredBatchNode::visit(r, t, f, zPath);
}

void DynamicLabel::updateColor() {
	if (_format) {
		for (auto &it : _format->ranges) {
			if (!it.colorDirty) {
				it.color.r = _displayedColor.r;
				it.color.g = _displayedColor.g;
				it.color.b = _displayedColor.b;
			}
			if (!it.opacityDirty) {
				it.color.a = _displayedOpacity;
			}
		}
	}
	_colorDirty = true;
}

void DynamicLabel::updateColorQuads() {
	if (!_textures.empty() && !_colorMap.empty()) {
		for (size_t i = 0; i < _textures.size(); ++ i) {
			DynamicQuadArray * quads = _textures[i].quads;
			auto &cMap = _colorMap[i];
			if (quads->size() * 2 == cMap.size()) {
				for (size_t j = 0; j < quads->size(); ++j) {
					if (!cMap[j * 2]) { quads->setColor3B(j, _displayedColor); }
					if (!cMap[j * 2 + 1]) { quads->setOpacity(j, _displayedOpacity); }
				}
			}
		}
	}
	_colorDirty = false;
}

void DynamicLabel::setDensity(float density) {
	if (density != _density) {
		LayeredBatchNode::setDensity(density);
		_labelDirty = true;
	}
}

size_t DynamicLabel::getLinesCount() const {
	return _format?_format->lines.size():0;
}
DynamicLabel::LineSpec DynamicLabel::getLine(uint32_t num) const {
	if (_format) {
		if (num < _format->lines.size()) {
			return _format->lines[num];
		}
	}
	return LineSpec();
}

uint16_t DynamicLabel::getFontHeight() const {
	return const_cast<Source *>(_source.get())->getLayout(_style.font)->getData()->metrics.height;
}

cocos2d::GLProgramState *DynamicLabel::getProgramStateA8() const {
	return cocos2d::GLProgramState::getOrCreateWithGLProgram(TextureCache::getInstance()->getBatchPrograms()->getProgram(GLProgramSet::DynamicBatchA8Highp));
}

void DynamicLabel::updateQuads(uint32_t f) {
	if (!_source) {
		return;
	}

	if (!_format || _format->chars.size() == 0) {
		_textures.clear();
		return;
	}

	for (auto &it : _format->ranges) {
		_source->addTextureChars(it.layout->getName(), _format->chars, it.start, it.count);
	}

	if (!_source->isDirty() && !_source->getTextures().empty()) {
		if (f & FLAGS_FORCE_RENDERING) {
			updateQuadsForeground(_source, _format);
		} else {
			_quadRequestTime = Time::now();
			updateQuadsBackground(_source, _format);
		}
	}

	_formatDirty = false;
}

void DynamicLabel::onTextureUpdated() {
	_formatDirty = true;
}

void DynamicLabel::onLayoutUpdated() {
	_labelDirty = false;
}

void DynamicLabel::onQuads(const Time &t, const Vector<Rc<cocos2d::Texture2D>> &newTex,
		Vector<Rc<DynamicQuadArray>> &&newQuads, Vector<Vector<bool>> &&cMap) {

	//log::format("Label", "onQuads %p %lu %s", this, _updateCount, _string8.c_str());

	++ _updateCount;

	if (t < _quadRequestTime) {
		return;
	}

	if (newQuads.size() != newTex.size()) {
		return;
	}

	bool replaceTextures = false;
	if (newTex.size() != _textures.size()) {
		replaceTextures = true;
	} else {
		for (size_t i = 0; i < newTex.size(); ++ i) {
			if (newTex[i]->getName() != _textures[i].texture->getName()) {
				replaceTextures = true;
			}
		}
	}

	if (replaceTextures) {
		setTextures(newTex, std::move(newQuads));
	} else {
		for (size_t i = 0; i < newQuads.size(); ++i) {
			_textures[i].quads = std::move(newQuads[i]);
			auto a = _textures[i].atlas;
			if (a) {
				a->clear();
				a->addQuadArray(_textures[i].quads);
			}
		}
	}

	_colorMap = std::move(cMap);

	updateColorQuads();
}

NS_SP_END
