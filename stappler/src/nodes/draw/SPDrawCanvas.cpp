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

#include "SPDrawCanvas.h"
#include "SPDefine.h"
#include "SPThreadManager.h"
#include "SPTextureCache.h"
#include "renderer/CCTexture2D.h"
#include "renderer/ccGLStateCache.h"
#include "base/CCConfiguration.h"

TESS_OPTIMIZE

NS_SP_EXT_BEGIN(draw)

Canvas::~Canvas() {
	if (_vbo[0] || _vbo[1]) {
		glDeleteBuffers(2, _vbo);
	}

	if (_rbo) {
		glDeleteRenderbuffers(1, &_rbo);
		_rbo = 0;
	}

	if (_fbo) {
		glDeleteFramebuffers(1, &_fbo);
		_fbo = 0;
	}
}

bool Canvas::init() {
	if (!layout::Canvas::init()) {
		return false;
	}

	glGenFramebuffers(1, &_fbo);
	glGenRenderbuffers(1, &_rbo);
	glGenBuffers(2, _vbo);
	if (_fbo != 0 && _vbo[0] != 0 && _vbo[1] != 0) {
		return true;
	}

	return false;
}

void Canvas::begin(cocos2d::Texture2D *tex, const Color4B &color) {
	auto w = tex->getPixelsWide();
	auto h = tex->getPixelsHigh();
	_internalFormat = tex->getPixelFormat();
	_referenceFormat = tex->getReferenceFormat();
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_oldFbo);

	if (ThreadManager::getInstance()->isMainThread()) {
		cocos2d::GL::useProgram(0);
		cocos2d::GL::enableVertexAttribs(0);
		cocos2d::GL::bindTexture2D(0);
		cocos2d::GL::bindVAO(0);
	}

	if (uint32_t(w) != _width || uint32_t(h) != _height) {
		GLint oldRbo = 0;
		glGetIntegerv(GL_RENDERBUFFER_BINDING, &oldRbo);
		_width = w;
		_height = h;
		glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, _width, _height);
		glBindRenderbuffer(GL_RENDERBUFFER, oldRbo);
	    CHECK_GL_ERROR_DEBUG();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->getName(), 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);
    CHECK_GL_ERROR_DEBUG();

	auto check = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (check) {
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		log::format("Framebuffer", "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		log::format("Framebuffer", "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		log::format("Framebuffer", "GL_FRAMEBUFFER_UNSUPPORTED");
		break;
#ifndef GL_ES_VERSION_2_0
	case GL_FRAMEBUFFER_UNDEFINED:
		log::format("Framebuffer", "GL_FRAMEBUFFER_UNDEFINED");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		log::format("Framebuffer", "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
		break;
#endif
	case GL_FRAMEBUFFER_COMPLETE:
		_valid = true;
		//log::format("Framebuffer", "GL_FRAMEBUFFER_COMPLETE");
		break;
	case 0:
		log::format("Framebuffer", "Success");
		break;
	default:
		log::format("Framebuffer", "Undefined %d", check);
		break;
	}

	if (_valid) {
		glViewport(0, 0, (GLsizei)_width, (GLsizei)_height);
		glClearColor(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		int32_t gcd = sp_gcd(_width, _height);
		int32_t dw = (int32_t)_width / gcd;
		int32_t dh = (int32_t)_height / gcd;
		int32_t dwh = gcd * dw * dh;

		float mod = 1.0f;
		while (dwh * mod > 8_KiB) {
			mod /= 2.0f;
		}

		_viewTransform = Mat4::IDENTITY;
		_viewTransform.scale(dh * mod, dw * mod, -1.0);
		_viewTransform.m[12] = -dwh * mod / 2.0f;
		_viewTransform.m[13] = -dwh * mod / 2.0f;
		_viewTransform.m[14] = dwh * mod / 2.0f - 1;
		_viewTransform.m[15] = dwh * mod / 2.0f + 1;
		_viewTransform.m[11] = -1.0f;

		_transform = Mat4::IDENTITY;

		_line.reserve(256);

		_drawProgram = TextureCache::getInstance()->getRawPrograms()->getProgram(GLProgramSet::RawRects);
		_aaProgram = TextureCache::getInstance()->getRawPrograms()->getProgram(
				_internalFormat == cocos2d::Texture2D::PixelFormat::R8?GLProgramSet::RawAAMaskR:GLProgramSet::RawAAMaskRGBA);

		_vertexBufferSize = 0;
		_indexBufferSize = 0;
		_uniformColorDraw = Color4B(0, 0, 0, 0);
		_uniformColorAA = Color4B(0, 0, 0, 0);

		_uniformTransformDraw = Mat4::ZERO;
		_uniformTransformAA = Mat4::ZERO;

		_subAccum.clear();
		_tessAccum.clear();
		_glAccum.clear();
		_contourVertex = 0;
		_fillVertex = 0;

	    CHECK_GL_ERROR_DEBUG();

	} else {
		glBindFramebuffer(GL_FRAMEBUFFER, _oldFbo);
	}
}

void Canvas::end() {
	if (_valid) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, _oldFbo);
	    CHECK_GL_ERROR_DEBUG();
	}

	_valid = false;

	//log::format("CanvasTiming", "sub: %llu, tess: %llu, gl: %llu, Vertex: %u %u", _subAccum.toMicroseconds(), _tessAccum.toMicroseconds(),
	//		_glAccum.toMicroseconds(), _fillVertex, _contourVertex);

	cleanup();
}

void Canvas::flush() {
	if (!_valid) {
		return;
	}

	Time t = Time::now();

	if (!_tess.empty()) {
		if (TESSResult * res = tessVecResultTriangles(_tess.data(), int(_tess.size()))) {
			auto verts = res->contour.vertexBuffer;
			auto nverts = res->contour.vertexCount;

			auto elts = res->contour.elementsBuffer;
			auto nelts = res->contour.elementCount;

			bool aa = (nelts > 0);

			_tessAccum += (Time::now() - t);
			t = Time::now();

			blendFunc(cocos2d::BlendFunc::ALPHA_NON_PREMULTIPLIED);
			bindTexture(0);
			useProgram(_aaProgram->getProgram());
			setUniformTransform(_aaProgram, _transform);


			glBindBuffer(GL_ARRAY_BUFFER, _vbo[0]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo[1]);

			size_t vertexBufferOffset = aa ? ((nverts * sizeof(TESSPoint) + 0xF) & ~0xF) : 0;
			size_t vertexBufferSize = aa ? (vertexBufferOffset + ((res->triangles.vertexCount * sizeof(TESSPoint) + 0xF) & ~0xF)) : (res->triangles.vertexCount * sizeof(TESSPoint));
			size_t indexBufferSize = sizeof(TESSshort) * res->triangles.elementCount * 3;

			glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, nullptr, GL_STREAM_DRAW);
			if (aa) {
				glBufferSubData(GL_ARRAY_BUFFER, 0,  nverts * sizeof(TESSPoint), verts);
				glBufferSubData(GL_ARRAY_BUFFER, vertexBufferOffset, res->triangles.vertexCount * sizeof(TESSPoint), res->triangles.vertexBuffer);
			} else {
				glBufferSubData(GL_ARRAY_BUFFER, vertexBufferOffset, vertexBufferSize, res->triangles.vertexBuffer);
			}
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, res->triangles.elementsBuffer, GL_STREAM_DRAW);

			enableVertexAttribs(cocos2d::GL::VERTEX_ATTRIB_FLAG_POSITION | cocos2d::GL::VERTEX_ATTRIB_FLAG_COLOR);

			if (aa) {
				glVertexAttribPointer(cocos2d::GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(TESSPoint), 0);
				glVertexAttribPointer(cocos2d::GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE,
						sizeof(TESSPoint), (GLvoid*) offsetof(TESSPoint, c));

				glEnable(GL_STENCIL_TEST);
				glStencilMask(1);
				glClear(GL_STENCIL_BUFFER_BIT);
				glStencilFunc(GL_ALWAYS, 1, 1);
				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

				for (int i = 0; i < nelts; ++ i) {
					auto pts = elts[2 * i];
					auto npts = elts[2 * i + 1];

					glDrawArrays(GL_TRIANGLE_STRIP, pts, npts);
					_contourVertex += npts;
				}

				glStencilFunc(GL_NOTEQUAL, 1, 1);
				glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			}

			elts = res->triangles.elementsBuffer;
			nelts = res->triangles.elementCount;

			_fillVertex += nelts;

			glVertexAttribPointer(cocos2d::GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(TESSPoint), (GLvoid*)vertexBufferOffset);
			glVertexAttribPointer(cocos2d::GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE,
					sizeof(TESSPoint), (GLvoid*) (vertexBufferOffset + offsetof(TESSPoint, c)));

			glDrawElements(GL_TRIANGLES, (GLsizei) nelts * 3, GL_UNSIGNED_SHORT, (GLvoid *) (0));

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			if (aa) {
				glDisable(GL_STENCIL_TEST);
			}

			_glAccum += (Time::now() - t);
		} else {
			log::text("Canvas", "fail to tesselate contour");
		}
	}

	if (!_stroke.empty()) {
		Time t = Time::now();
		blendFunc(cocos2d::BlendFunc::ALPHA_NON_PREMULTIPLIED);
		bindTexture(0);
		useProgram(_aaProgram->getProgram());
		setUniformTransform(_aaProgram, _transform);

		size_t size = 0;
		size_t offset = 0;
		for (auto & it : _stroke) {
			size += it.outline.size();
			if (it.antialiased) {
				size += it.inner.size();
				size += it.outer.size();
			}
		}

		auto vertexBufferSize = (size * sizeof(TESSPoint));

		glBindBuffer(GL_ARRAY_BUFFER, _vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, nullptr, GL_STATIC_DRAW);

		for (auto & it : _stroke) {
			auto s = it.outline.size() * sizeof(TESSPoint);
			glBufferSubData(GL_ARRAY_BUFFER, offset, s, it.outline.data());
			offset += s;
			if (it.antialiased) {
				s = it.inner.size() * sizeof(TESSPoint);
				glBufferSubData(GL_ARRAY_BUFFER, offset, s, it.inner.data());
				offset += s;

				s = it.outer.size() * sizeof(TESSPoint);
				glBufferSubData(GL_ARRAY_BUFFER, offset, s, it.outer.data());
				offset += s;
			}
		}

		enableVertexAttribs(cocos2d::GL::VERTEX_ATTRIB_FLAG_POSITION | cocos2d::GL::VERTEX_ATTRIB_FLAG_COLOR);
		glVertexAttribPointer(cocos2d::GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(TESSPoint), 0);
		glVertexAttribPointer(cocos2d::GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(TESSPoint), (GLvoid*) offsetof(TESSPoint, c));

		offset = 0;
		for (auto & it : _stroke) {
			if (!it.antialiased) {
				auto s = it.outline.size();
				glDrawArrays(GL_TRIANGLE_STRIP, GLint(offset), GLint(s));
				offset += s;
			} else {
				glEnable(GL_STENCIL_TEST);
				glStencilMask(1);
				glClear(GL_STENCIL_BUFFER_BIT);
				glStencilFunc(GL_ALWAYS, 1, 1);
				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

				auto s = it.outline.size();
				glDrawArrays(GL_TRIANGLE_STRIP, GLint(offset), GLint(s));
				offset += s;

				glStencilFunc(GL_NOTEQUAL, 1, 1);
				glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

				s = it.inner.size();
				glDrawArrays(GL_TRIANGLE_STRIP, GLint(offset), GLint(s));
				offset += s;

				s = it.outer.size();
				glDrawArrays(GL_TRIANGLE_STRIP, GLint(offset), GLint(s));
				offset += s;

				glDisable(GL_STENCIL_TEST);
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		_glAccum += (Time::now() - t);
	}
}

static void updateUniformColor(cocos2d::GLProgram *p, cocos2d::Texture2D::PixelFormat internal,
		cocos2d::Texture2D::PixelFormat reference, const Color4B &color) {

	switch (internal) {
	case cocos2d::Texture2D::PixelFormat::R8:
		switch (reference) {
		case cocos2d::Texture2D::PixelFormat::I8:
			p->setUniformLocationWith4f(p->getUniformLocationForName(cocos2d::GLProgram::UNIFORM_AMBIENT_COLOR),
					((color.r * 299 + color.g * 587 + color.b * 114 + 500) / 1000) / 255.0f, 0.0f, 0.0f, 1.0f);
			break;
		case cocos2d::Texture2D::PixelFormat::A8:
			p->setUniformLocationWith4f(p->getUniformLocationForName(cocos2d::GLProgram::UNIFORM_AMBIENT_COLOR),
					color.a / 255.0f, color.a / 255.0f, color.a / 255.0f, 1.0f);
			break;
		default:
			p->setUniformLocationWith4f(p->getUniformLocationForName(cocos2d::GLProgram::UNIFORM_AMBIENT_COLOR),
					color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
			break;
		}
		break;
	case cocos2d::Texture2D::PixelFormat::RG88:
		if (reference == cocos2d::Texture2D::PixelFormat::AI88) {
			p->setUniformLocationWith4f(p->getUniformLocationForName(cocos2d::GLProgram::UNIFORM_AMBIENT_COLOR),
					((color.r * 299 + color.g * 587 + color.b * 114 + 500) / 1000) / 255.0f, color.a / 255.0f, 0.0f, 1.0f);
			break;
		} else {
			p->setUniformLocationWith4f(p->getUniformLocationForName(cocos2d::GLProgram::UNIFORM_AMBIENT_COLOR),
					color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
		}
		break;
	default:
		p->setUniformLocationWith4f(p->getUniformLocationForName(cocos2d::GLProgram::UNIFORM_AMBIENT_COLOR),
				color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
	}
}

void Canvas::setUniformColor(cocos2d::GLProgram *p, const Color4B &color) {
	if (p == _drawProgram) {
		if (_uniformColorDraw != color) {
			_uniformColorDraw = color;
			updateUniformColor(p, _internalFormat, _referenceFormat, color);
		}
	} else if (p == _aaProgram) {
		if (_uniformColorAA != color) {
			_uniformColorAA = color;
			updateUniformColor(p, _internalFormat, _referenceFormat, color);
		}
	} else {
		updateUniformColor(p, _internalFormat, _referenceFormat, color);
	}
}

void Canvas::setUniformTransform(cocos2d::GLProgram *p, const Mat4 &t) {
	if (p == _drawProgram) {
		if (_uniformTransformDraw != t) {
			_uniformTransformDraw = t;
			Mat4 mv = _viewTransform * t;
			p->setUniformLocationWithMatrix4fv(p->getUniformLocationForName(cocos2d::GLProgram::UNIFORM_MVP_MATRIX), mv.m, 1);
		}
	} else if (p == _aaProgram) {
		if (_uniformTransformAA != t) {
			_uniformTransformAA = t;
			Mat4 mv = _viewTransform * t;
			p->setUniformLocationWithMatrix4fv(p->getUniformLocationForName(cocos2d::GLProgram::UNIFORM_MVP_MATRIX), mv.m, 1);
		}
	} else {
		Mat4 mv = _viewTransform * t;
		p->setUniformLocationWithMatrix4fv(p->getUniformLocationForName(cocos2d::GLProgram::UNIFORM_MVP_MATRIX), mv.m, 1);
	}
}

NS_SP_EXT_END(draw)