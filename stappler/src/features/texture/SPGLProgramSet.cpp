/*
 * SPGLProgramSet.cpp
 *
 *  Created on: 22 мая 2015 г.
 *      Author: sbkarr
 */

#include "SPDefine.h"
#include "SPGLProgramSet.h"
#include "SPDevice.h"

#include "renderer/CCGLProgram.h"
#include "renderer/CCGLProgramCache.h"

#define STRINGIFY(A)  #A

NS_SP_BEGIN

namespace shaders {

auto frag_TextureColorA8 = R"Shader(
#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

void main() {
	gl_FragColor = vec4( v_fragmentColor.rgb,
		v_fragmentColor.a * texture2D(CC_Texture0, v_texCoord).a
	);
}
)Shader";

auto frag_TextureColorA8Highp = R"Shader(
#ifdef GL_ES
precision highp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

void main() {
	gl_FragColor = vec4( v_fragmentColor.rgb,
		v_fragmentColor.a * texture2D(CC_Texture0, v_texCoord).a
	);
}
)Shader";

auto frag_TextureColorI8 = R"Shader(
#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

void main() {
    gl_FragColor = vec4( v_fragmentColor.rgb,
        v_fragmentColor.a * (1.0 - texture2D(CC_Texture0, v_texCoord).r)
    );
}
)Shader";

auto frag_TextureColor = R"Shader(
#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

void main() {
	gl_FragColor = texture2D(CC_Texture0, v_texCoord) * v_fragmentColor;
}
)Shader";

auto frag_Texture = R"Shader(
#ifdef GL_ES
precision mediump float;
#endif

varying vec2 v_texCoord;
void main() {
    gl_FragColor = texture2D(CC_Texture0, v_texCoord);
}
)Shader";

auto vert_TextureColorProjection = R"Shader(
attribute vec4 a_position;
attribute vec2 a_texCoord;
attribute vec4 a_color;

#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

void main() {
	gl_Position = CC_PMatrix * a_position;
	v_fragmentColor = a_color;
	v_texCoord = a_texCoord;
}
)Shader";

auto vert_TextureColorViewProjection = R"Shader(
attribute vec4 a_position;
attribute vec2 a_texCoord;
attribute vec4 a_color;

#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

void main() {
	gl_Position = CC_MVPMatrix * a_position;
	v_fragmentColor = a_color;
	v_texCoord = a_texCoord;
}
)Shader";

auto vert_TextureColorViewProjectionHighp = R"Shader(
attribute vec4 a_position;
attribute vec2 a_texCoord;
attribute vec4 a_color;

#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying highp vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

void main() {
    gl_Position = CC_MVPMatrix * a_position;
    v_fragmentColor = a_color;
    v_texCoord = a_texCoord;
}
)Shader";

auto vert_Texture = R"Shader(
attribute vec4 a_position;
attribute vec2 a_texCoord;

#ifdef GL_ES
varying mediump vec2 v_texCoord;
#else
varying vec2 v_texCoord;
#endif

void main() {
    gl_Position = CC_MVPMatrix * a_position;
    v_texCoord = a_texCoord;
}
)Shader";

auto vert_TextureColor = R"Shader(
attribute vec4 a_position;
attribute vec2 a_texCoord;
attribute vec4 a_color;

#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

void main() {
    gl_Position = a_position;
    v_fragmentColor = a_color;
    v_texCoord = a_texCoord;
}
)Shader";

auto vert_TextureColorHighp = R"Shader(
attribute vec4 a_position;
attribute vec2 a_texCoord;
attribute vec4 a_color;

#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying highp vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

void main() {
    gl_Position = a_position;
    v_fragmentColor = a_color;
    v_texCoord = a_texCoord;
}
)Shader";


auto frag_RectColor = R"Shader(
#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_size;
uniform vec2 u_position;
uniform vec4 CC_AmbientColor;

void main() {
	vec2 position = abs((gl_FragCoord.xy - u_position) - (u_size)) - u_size;
	gl_FragColor = vec4(CC_AmbientColor.xyz, CC_AmbientColor.a * clamp(-max(position.x, position.y), 0.0, 1.0));
}
)Shader";

auto vert_RectColor = R"Shader(
attribute vec4 a_position;
void main() {
    gl_Position = CC_MVPMatrix * a_position;
}
)Shader";

auto frag_RectBorderColor = R"Shader(
#ifdef GL_ES
precision highp float;
#endif

uniform vec2 u_size;
uniform vec2 u_position;
uniform float u_border;
uniform vec4 CC_AmbientColor;

void main() {
	vec2 position = abs((gl_FragCoord.xy - u_position) - (u_size)) - u_size;
	gl_FragColor = vec4(CC_AmbientColor.xyz, CC_AmbientColor.a * clamp(u_border + 0.5 - abs(max(position.x, position.y)), 0.0, 1.0));
}
)Shader";

auto frag_RectsColor = R"Shader(
#ifdef GL_ES
precision mediump float;
#endif

uniform vec4 CC_AmbientColor;
void main() {
	gl_FragColor = CC_AmbientColor;
}
)Shader";


auto DrawNodeA8_frag = frag_TextureColorA8;
auto DrawNodeA8_vert = vert_TextureColorProjection;

auto DynamicBatchI8_frag = frag_TextureColorI8;
auto DynamicBatchI8_vert = vert_TextureColorViewProjection;

auto DynamicBatchAI88_frag = frag_TextureColor;
auto DynamicBatchAI88_vert = vert_TextureColorViewProjection;

auto DynamicBatchA8Highp_frag = frag_TextureColorA8Highp;
auto DynamicBatchA8Highp_vert = vert_TextureColorViewProjectionHighp;

auto RawTexture_frag = frag_Texture;
auto RawTexture_vert = vert_Texture;

auto RawTextureColor_frag = frag_TextureColor;
auto RawTextureColor_vert = vert_TextureColor;

auto RawTextureColorA8_frag = frag_TextureColorA8;
auto RawTextureColorA8_vert = vert_TextureColor;

auto RawTextureColorA8Highp_frag = frag_TextureColorA8Highp;
auto RawTextureColorA8Highp_vert = vert_TextureColorHighp;

auto RawRect_frag = frag_RectColor;
auto RawRect_vert = vert_RectColor;

auto RawRectBorder_frag = frag_RectBorderColor;
auto RawRectBorder_vert = vert_RectColor;

auto RawRects_frag = frag_RectsColor;
auto RawRects_vert = vert_RectColor;

}

static void GLProgramSet_loadProgram(cocos2d::GLProgram *p, const char *vert, const char *frag) {
	p->initWithByteArrays(vert, frag);
	p->link();
	p->updateUniforms();
}

static void GLProgramSet_reloadPrograms(const Map<GLProgramSet::Program, Rc<cocos2d::GLProgram>> &programs, bool reset) {
	for (auto &it : programs) {
		auto prog = it.second;
		if (reset) {
			prog->reset();
		}
		switch(it.first) {
		case GLProgramSet::DrawNodeA8:
			GLProgramSet_loadProgram(it.second, shaders::DrawNodeA8_vert, shaders::DrawNodeA8_frag); break;
		case GLProgramSet::DynamicBatchI8:
			GLProgramSet_loadProgram(it.second, shaders::DynamicBatchI8_vert, shaders::DynamicBatchI8_frag); break;
		case GLProgramSet::DynamicBatchAI88:
			GLProgramSet_loadProgram(it.second, shaders::DynamicBatchAI88_vert, shaders::DynamicBatchAI88_frag); break;
		case GLProgramSet::DynamicBatchA8Highp:
			GLProgramSet_loadProgram(it.second, shaders::DynamicBatchA8Highp_vert, shaders::DynamicBatchA8Highp_frag); break;
		case GLProgramSet::RawTexture:
			GLProgramSet_loadProgram(it.second, shaders::RawTexture_vert, shaders::RawTexture_frag); break;
		case GLProgramSet::RawTextureColor:
			GLProgramSet_loadProgram(it.second, shaders::RawTextureColor_vert, shaders::RawTextureColor_frag); break;
		case GLProgramSet::RawTextureColorA8:
			GLProgramSet_loadProgram(it.second, shaders::RawTextureColorA8_vert, shaders::RawTextureColorA8_frag); break;
		case GLProgramSet::RawTextureColorA8Highp:
			GLProgramSet_loadProgram(it.second, shaders::RawTextureColorA8Highp_vert, shaders::RawTextureColorA8Highp_frag); break;
		case GLProgramSet::RawRect:
			GLProgramSet_loadProgram(it.second, shaders::RawRect_vert, shaders::RawRect_frag); break;
		case GLProgramSet::RawRectBorder:
			GLProgramSet_loadProgram(it.second, shaders::RawRectBorder_vert, shaders::RawRectBorder_frag); break;
		case GLProgramSet::RawRects:
			GLProgramSet_loadProgram(it.second, shaders::RawRects_vert, shaders::RawRects_frag); break;
		default: break;
		}
	}
}

bool GLProgramSet::init() {
	_programs.emplace(DrawNodeA8, Rc<cocos2d::GLProgram>::alloc());
	_programs.emplace(DynamicBatchI8, Rc<cocos2d::GLProgram>::alloc());
	_programs.emplace(DynamicBatchAI88, Rc<cocos2d::GLProgram>::alloc());
	_programs.emplace(DynamicBatchA8Highp, Rc<cocos2d::GLProgram>::alloc());
	_programs.emplace(RawTexture, Rc<cocos2d::GLProgram>::alloc());
	_programs.emplace(RawTextureColor, Rc<cocos2d::GLProgram>::alloc());
	_programs.emplace(RawTextureColorA8, Rc<cocos2d::GLProgram>::alloc());
	_programs.emplace(RawTextureColorA8Highp, Rc<cocos2d::GLProgram>::alloc());
	_programs.emplace(RawRect, Rc<cocos2d::GLProgram>::alloc());
	_programs.emplace(RawRectBorder, Rc<cocos2d::GLProgram>::alloc());
	_programs.emplace(RawRects, Rc<cocos2d::GLProgram>::alloc());

	GLProgramSet_reloadPrograms(_programs, false);

	return true;
}

bool GLProgramSet::init(uint32_t mask) {
	if (mask & DrawNodeA8) { _programs.emplace(DrawNodeA8, Rc<cocos2d::GLProgram>::alloc()); }
	if (mask & DynamicBatchI8) { _programs.emplace(DynamicBatchI8, Rc<cocos2d::GLProgram>::alloc()); }
	if (mask & DynamicBatchAI88) { _programs.emplace(DynamicBatchAI88, Rc<cocos2d::GLProgram>::alloc()); }
	if (mask & DynamicBatchA8Highp) { _programs.emplace(DynamicBatchA8Highp, Rc<cocos2d::GLProgram>::alloc()); }
	if (mask & RawTexture) { _programs.emplace(RawTexture, Rc<cocos2d::GLProgram>::alloc()); }
	if (mask & RawTextureColor) { _programs.emplace(RawTextureColor, Rc<cocos2d::GLProgram>::alloc()); }
	if (mask & RawTextureColorA8) { _programs.emplace(RawTextureColorA8, Rc<cocos2d::GLProgram>::alloc()); }
	if (mask & RawTextureColorA8Highp) { _programs.emplace(RawTextureColorA8Highp, Rc<cocos2d::GLProgram>::alloc()); }
	if (mask & RawRect) { _programs.emplace(RawRect, Rc<cocos2d::GLProgram>::alloc()); }
	if (mask & RawRectBorder) { _programs.emplace(RawRectBorder, Rc<cocos2d::GLProgram>::alloc()); }
	if (mask & RawRects) { _programs.emplace(RawRects, Rc<cocos2d::GLProgram>::alloc()); }

	GLProgramSet_reloadPrograms(_programs, false);

	return true;
}

cocos2d::GLProgram *GLProgramSet::getProgram(Program p) {
	auto it =_programs.find(p);
	if (it != _programs.end()) {
		return it->second;
	}
	return nullptr;
}

GLProgramSet::GLProgramSet() { }

void GLProgramSet::reloadPrograms() {
	GLProgramSet_reloadPrograms(_programs, true);
}

NS_SP_END