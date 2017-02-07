// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/**
Copyright (c) 2016-2017 Roman Katuntsev <sbkarr@stappler.org>

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

#include "SPLayout.h"
#include "SLBuilder.h"
#include "SLNode.h"
#include "SLResult.h"

#include "SPString.h"

//#define SP_RTBUILDER_LOG(...) log::format("RTBuilder", __VA_ARGS__)
#define SP_RTBUILDER_LOG(...)

NS_LAYOUT_BEGIN

void Builder::resolveMediaQueries(const Vector<style::MediaQuery> &vec) {
	_mediaQueries = _media.resolveMediaQueries(vec);
}

bool Builder::resolveMediaQuery(MediaQueryId queryId) const {
	if (queryId < _mediaQueries.size()) {
		return _mediaQueries[queryId];
	}
	return false;
}

String Builder::getCssString(CssStringId id) const {
	if (!_document) {
		return "";
	}

	auto &map = _document->getCssStrings();
	auto it = map.find(id);
	if (it != map.end()) {
		return it->second;
	}
	return "";
}

Builder::Builder(Document *doc, const MediaParameters &media, FontSource *cfg, const Vector<String> &spine) {
	_document = doc;
	_media = media;
	_fontSet = cfg;

	if ((_media.flags & RenderFlag::PaginatedLayout)) {
		Margin extra(0);
		bool isSplitted = _media.flags & RenderFlag::SplitPages;
		if (isSplitted) {
			extra.left = 7 * _media.fontScale;
			extra.right = 7 * _media.fontScale;
		} else {
			extra.left = 3 * _media.fontScale;
			extra.right = 3 * _media.fontScale;
		}

		_media.pageMargin.left += extra.left;
		_media.pageMargin.right += extra.right;

		_media.surfaceSize.width -= (extra.left + extra.right);
	}

	_result = Rc<Result>::create(_media, _fontSet, _document);
	_spine = spine;

	if ((_media.flags & RenderFlag::PaginatedLayout) && _media.mediaType == style::MediaType::Screen) {
		_media.mediaType = style::MediaType::Print;
	}
}

Builder::~Builder() { }

void Builder::setMargin(const Margin &m) {
	_margin = m;
}

void Builder::setHyphens(HyphenMap *map) {
	_hyphens = map;
}

Result *Builder::getResult() const {
	return _result;
}

const MediaParameters &Builder::getMedia() const {
	return _media;
}

Document *Builder::getDocument() const {
	return _document;
}
FontSource *Builder::getFontSet() const {
	return _fontSet;
}

void Builder::render() {
	resolveMediaQueries(_document->getMediaQueries());
	buildBlockModel();
}

void Builder::buildBlockModel() {
	auto &root = _document->getRoot();
	auto &documentStyle = root.getStyle();

	Layout l;
	BlockStyle rootBlockModel;
	if (!_spine.empty() && (_media.flags & RenderFlag::RenderById)) {
		l.style = documentStyle.compileBlockModel(this);
	} else {
		l.style.width = style::Metric{1.0f, style::Metric::Units::Percent};
		l.style.marginLeft = style::Metric{_margin.left, style::Metric::Units::Px};
		l.style.marginTop = style::Metric{_margin.top, style::Metric::Units::Px};
		l.style.marginBottom = style::Metric{_margin.bottom, style::Metric::Units::Px};
		l.style.marginRight = style::Metric{_margin.right, style::Metric::Units::Px};
	}
	BackgroundStyle rootBackground = documentStyle.compileBackground(this);

	bool pageBreak = (_media.flags & RenderFlag::PaginatedLayout);
	FloatContext f{ &l, pageBreak?_media.surfaceSize.height:nan() };
	_floatStack.push_back(&f);
	if (initLayout(l, Vec2::ZERO, Size(_media.surfaceSize.width, nan()), 0.0f)) {
		Vector<const Node *> nodes;
		if (_spine.empty()) {
			auto &content = _document->getContent();
			for (auto &it : content) {
				if (it.linear) {
					nodes.push_back(&it.root);
				}
			}
		} else {
			if (_media.flags & RenderFlag::RenderById) {
				for (auto &it : _spine) {
					if (auto node = _document->getNodeById(it)) {
						nodes.push_back(node);
					}
				}
				if (pageBreak) {
					pageBreak = false;
				}
			} else {
				for (auto &it : _spine) {
					if (auto page = _document->getContentPage(it)) {
						nodes.push_back(&page->root);
					}
				}
			}
		}
		processChilds(l, nodes, pageBreak);
		finalizeLayout(l, Vec2::ZERO, 0.0f);
	}
	_floatStack.pop_back();

	_result->setContentSize(l.size);

	if (rootBackground.backgroundColor.a != 0) {
		_result->setBackgroundColor(rootBackground.backgroundColor);
	} else {
		_result->setBackgroundColor(Color4B(0xfa, 0xfa, 0xfa, 255));
	}

	if (!l.layouts.empty()) {
		addLayoutObjects(l);
	}
	_result->finalize();

	// log::format("ReaderTime", "%lu", _readerAccum.toMicroseconds());
}

void Builder::addLayoutObjects(Layout &l) {
	if (l.node) {
		if (!l.node->getHtmlId().empty()) {
			_result->pushIndex(l.node->getHtmlId(), l.position);
		}
	}

	if (!l.preObjects.empty()) {
		for (auto &it : l.preObjects) {
			it.bbox.origin += l.position;
			//log("object: %f %f %f %f", it.bbox.origin.x, it.bbox.origin.y, it.bbox.size.width, it.bbox.size.height);
			_result->pushObject(std::move(it));
		}
	}

	if (!l.layouts.empty()) {
		for (auto &it : l.layouts) {
			addLayoutObjects(it);
		}
	}

	if (!l.postObjects.empty()) {
		for (auto &it : l.postObjects) {
			it.bbox.origin += l.position;
			//log("object: %f %f %f %f", it.bbox.origin.x, it.bbox.origin.y, it.bbox.size.width, it.bbox.size.height);
			_result->pushObject(std::move(it));
		}
	}
}

TimeInterval Builder::getReaderTime() const {
	return _readerAccum;
}

void Builder::processChilds(Layout &l, const Node &node) {
	auto &nodes = node.getNodes();
	Vector<const Node *> ptrVec; ptrVec.reserve(nodes.size());
	for (auto &it : nodes) {
		ptrVec.push_back(&it);
	}
	processChilds(l, ptrVec, false);
}

void Builder::processChilds(Layout &l, const Vector<const Node *> &nodes, bool pageBreak) {
	Vec2 pos = l.position;
	float collapsableMarginTop = l.collapsableMarginTop;
	float height = 0;
	_layoutStack.push_back(&l);

	l.layoutContext = (l.context && !l.context->finalized)?style::Display::Inline:style::Display::RunIn;
	for (auto it = nodes.begin(); it != nodes.end(); ++ it) {
		l.layoutContext = getLayoutContext(nodes, it, l.layoutContext);
		auto newNode = *it;
		if (pageBreak) {
			if (!l.layouts.empty()) {
				doPageBreak(&l.layouts.back(), pos);
			} else {
				doPageBreak(nullptr, pos);
			}
			collapsableMarginTop = 0;
		}
		if (!processChildNode(l, *newNode, pos, height, collapsableMarginTop)) {
			break;
		}
	}

	finalizeChilds(l, height);
	_layoutStack.pop_back();
}

bool Builder::processChildNode(Layout &l, const Node &newNode, Vec2 &pos, float &height, float &collapsableMarginTop) {
	SP_RTBUILDER_LOG("block style: %s", newNode.getStyle().css(this).c_str());

	// correction of display style property
	// actual value may be different for default/run-in
	// <img> tag is block by default, but inline-block inside <p> or in inline context
	auto blockModel = newNode.getStyle().compileBlockModel(this);
	if (blockModel.display == style::Display::Default || blockModel.display == style::Display::RunIn) {
		if (l.layoutContext == style::Display::Block) {
			blockModel.display = style::Display::Block;
		} else if (l.layoutContext == style::Display::Inline) {
			if (newNode.getHtmlName() == "img") {
				blockModel.display = style::Display::InlineBlock;
			} else {
				blockModel.display = style::Display::Inline;
			}
		}
	}

	if (blockModel.display == style::Display::InlineBlock) {
		if (l.layoutContext == style::Display::Block) {
			blockModel.display = style::Display::Block;
		}
	}

	if (newNode.getHtmlName() == "img" && (_media.flags & RenderFlag::NoImages)) {
		blockModel.display = style::Display::None;
	}

	if (blockModel.display == style::Display::None) {
		return true;
	} else if (blockModel.floating != style::Float::None) {
		return processFloatNode(l, newNode, std::move(blockModel), pos);
	} else if (blockModel.display == style::Display::Inline) {
		return processInlineNode(l, newNode, std::move(blockModel), pos);
	} else if (blockModel.display == style::Display::InlineBlock) {
		return processInlineBlockNode(l, newNode, std::move(blockModel), pos);
	} else {
		return processBlockNode(l, newNode, std::move(blockModel), pos, height, collapsableMarginTop);
	}
	return true;
}

NS_LAYOUT_END