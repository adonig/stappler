// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/**
Copyright (c) 2017 Roman Katuntsev <sbkarr@stappler.org>

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

#include "Define.h"
#include "ServerComponent.h"
#include "StorageScheme.h"
#include "RequestHandler.h"
#include "Networking.h"
#include "Tools.h"

NS_SA_EXT_BEGIN(test)

class TestHandler : public ServerComponent {
public:
	TestHandler(Server &serv, const String &name, const data::Value &dict);
	virtual ~TestHandler() { }

	virtual void onChildInit(Server &) override;

protected:
	using Field = storage::Field;
	using Flags = storage::Flags;
	using RemovePolicy = storage::RemovePolicy;
	using Transform = storage::Transform;
	using Scheme = storage::Scheme;
	using MaxFileSize = storage::MaxFileSize;
	using MaxImageSize = storage::MaxImageSize;
	using Thumbnail = storage::Thumbnail;

	Scheme _objects = Scheme("objects");
	Scheme _refs = Scheme("refs");
	Scheme _subobjects = Scheme("subobjects");
};

TestHandler::TestHandler(Server &serv, const String &name, const data::Value &dict)
: ServerComponent(serv, name, dict) {
	exportValues(_objects, _refs, _subobjects);

	_objects.define({
		Field::Text("text", storage::MinLength(3)),
		Field::Extra("data", Vector<Field>{
			Field::Array("strings", Field::Text("")),
		}),
		Field::Set("subobjects", _subobjects),
		Field::Text("alias", storage::Transform::Alias),
		Field::Integer("mtime", storage::Flags::AutoMTime | storage::Flags::Indexed),
		Field::Integer("index", storage::Flags::Indexed),
	});

	_refs.define({
		Field::Text("alias", storage::Transform::Alias),
		Field::Text("text", storage::MinLength(3)),
		Field::Set("features", _objects, RemovePolicy::StrongReference),
		Field::Integer("mtime", storage::Flags::AutoMTime | storage::Flags::Indexed),
		Field::Integer("index", storage::Flags::Indexed),
		Field::File("file", MaxFileSize(100_KiB)),
		Field::Array("array", Field::Text("", storage::MaxLength(10))),

		Field::Image("cover", storage::MaxImageSize(1080, 1080, storage::ImagePolicy::Resize), Vector<storage::Thumbnail>{
			storage::Thumbnail("thumb", 160, 160),
			storage::Thumbnail("cover512", 512, 512),
			storage::Thumbnail("cover256", 256, 256),
			storage::Thumbnail("cover128", 128, 128),
			storage::Thumbnail("cover64", 64, 64),
		}),
	});

	_subobjects.define({
		Field::Text("text", storage::MinLength(3)),
		Field::Object("object", _objects),
		Field::Integer("mtime", storage::Flags::AutoMTime | storage::Flags::Indexed),
		Field::Integer("index", storage::Flags::Indexed),
	});
}

void TestHandler::onChildInit(Server &serv) {
	serv.addResourceHandler("/objects/", _objects);
	serv.addResourceHandler("/refs/", _refs);
}

extern "C" ServerComponent * CreateTestHandler(Server &serv, const String &name, const data::Value &dict) {
	return new TestHandler(serv, name, dict);
}

NS_SA_EXT_END(test)