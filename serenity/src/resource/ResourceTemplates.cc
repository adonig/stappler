/**
Copyright (c) 2017-2018 Roman Katuntsev <sbkarr@stappler.org>

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
#include "ResourceTemplates.h"
#include "StorageAdapter.h"
#include "StorageScheme.h"

NS_SA_BEGIN

ResourceObject::ResourceObject(Adapter *a, QueryList &&q)
: Resource(ResourceType::Object, a, move(q)) { }

bool ResourceObject::prepareUpdate() {
	_perms = isSchemeAllowed(getScheme(), Action::Update);
	return _perms != Permission::Restrict;
}

bool ResourceObject::prepareCreate() {
	_status = HTTP_NOT_IMPLEMENTED;
	return false;
}

bool ResourceObject::prepareAppend() {
	_status = HTTP_NOT_IMPLEMENTED;
	return false;
}

bool ResourceObject::removeObject() {
	auto objs = getDatabaseId(_queries);
	if (objs.empty()) {
		return false;
	}

	_perms = isSchemeAllowed(getScheme(), Action::Remove);
	if (_perms == Permission::Restrict) {
		return false;
	} else if (_perms == Permission::Full) {
		bool ret = (objs.size() == 1);
		for (auto &it : objs) {
			_adapter->performInTransaction([&] {
				if (ret) {
					ret = getScheme().remove(_adapter, it);
				} else {
					getScheme().remove(_adapter, it);
				}
				return true;
			});
		}
		return (objs.size() == 1)?ret:true;
	} else {
		for (auto &it : objs) {
			_adapter->performInTransaction([&] {
				// remove with select for update
				auto obj = getScheme().get(_adapter, it, true);
				if (obj && isObjectAllowed(getScheme(), Action::Remove, obj)) {
					getScheme().remove(_adapter, it);
					return true;
				}
				return false;
			});
		}
		return true;
	}
	return false;
}

data::Value ResourceObject::performUpdate(const Vector<int64_t> &objs, data::Value &data, apr::array<InputFile> &files) {
	data::Value ret;
	encodeFiles(data, files);

	if (_transform) {
		auto it = _transform->find(getScheme().getName());
		if (it != _transform->end()) {
			it->second.input.transform(data);
		}
	}

	if (_perms == Permission::Full) {
		for (auto &it : objs) {
			_adapter->performInTransaction([&] {
				ret.addValue(getScheme().update(_adapter, it, data));
				return true;
			});
		}
	} else if (_perms == Permission::Partial) {
		for (auto &it : objs) {
			_adapter->performInTransaction([&] {
				auto obj = getScheme().get(_adapter, it, true);
				data::Value patch = data;
				if (obj && isObjectAllowed(getScheme(), Action::Update, obj, patch)) {
					ret.addValue(getScheme().update(_adapter, it, patch));
					return true;
				}
				return false;
			});
		}
	}

	return processResultList(_queries, ret);
}

data::Value ResourceObject::updateObject(data::Value &data, apr::array<InputFile> &files) {
	data::Value ret;
	if (files.empty() && (!data.isDictionary() || data.empty())) {
		return data::Value();
	}

	// single-object or mass update
	auto objs = getDatabaseId(_queries);
	if (objs.empty()) {
		return data::Value();
	}

	return performUpdate(objs, data, files);
}

data::Value ResourceObject::getResultObject() {
	auto ret = getDatabaseObject();
	if (!ret.isArray()) {
		return data::Value();
	}

	return processResultList(_queries, ret);
}

data::Value ResourceObject::processResultList(const QueryList &s, data::Value &ret) {
	auto perms = isSchemeAllowed(*s.getScheme(), Action::Read);
	if (perms != Permission::Restrict && ret.isArray()) {
		auto &arr = ret.asArray();
		auto it = arr.begin();
		while (it != arr.end()) {
			if (it->isInteger()) {
				if (auto val = getScheme().get(_adapter, it->getInteger())) {
					*it = move(val);
				}
			}

			if (!processResultObject(perms, s, *it)) {
				it = arr.erase(it);
			} else {
				it ++;
			}
		}
		return std::move(ret);
	}
	return data::Value();
}

bool ResourceObject::processResultObject(Permission p, const QueryList &s, data::Value &obj) {
	if (obj.isDictionary() && (p == Permission::Full || isObjectAllowed(*s.getScheme(), Action::Read, obj))) {
		resolveResult(s, obj);
		return true;
	}
	return false;
}

data::Value ResourceObject::getDatabaseObject() {
	return _adapter->performQueryList(_queries);
}

Vector<int64_t> ResourceObject::getDatabaseId(const QueryList &q, size_t count) {
	const Vector<QueryList::Item> &items = q.getItems();
	count = min(items.size(), count);

	return _adapter->performQueryListForIds(q, count);
}

ResourceReslist::ResourceReslist(Adapter *a, QueryList &&q)
: ResourceObject(a, move(q)) {
	_type = ResourceType::ResourceList;
}

bool ResourceReslist::prepareCreate() {
	_perms = isSchemeAllowed(getScheme(), Action::Create);
	return _perms != Permission::Restrict;
}
data::Value ResourceReslist::performCreateObject(data::Value &data, apr::array<InputFile> &files, const data::Value &extra) {
	// single object
	if (data.isDictionary() || data.empty()) {
		if (extra.isDictionary()) {
			for (auto & it : extra.asDict()) {
				data.setValue(it.second, it.first);
			}
		}

		encodeFiles(data, files);

		if (_transform) {
			auto it = _transform->find(getScheme().getName());
			if (it != _transform->end()) {
				it->second.input.transform(data);
			}
		}

		data::Value ret;
		if (_perms == Permission::Full) {
			ret = getScheme().create(_adapter, data);
		} else if (_perms == Permission::Partial) {
			data::Value val;
			if (isObjectAllowed(getScheme(), Action::Create, val, data)) {
				ret = getScheme().create(_adapter, data);
			}
		}
		auto perms = isSchemeAllowed(getScheme(), Action::Read);
		if (processResultObject(perms, _queries, ret)) {
			return ret;
		}
	} else if (data.isArray()) {
		if (_transform) {
			auto it = _transform->find(getScheme().getName());
			if (it != _transform->end()) {
				for (auto &obj : data.asArray()) {
					if (obj.isDictionary()) {
						if (extra.isDictionary()) {
							for (auto & it : extra.asDict()) {
								data.setValue(it.second, it.first);
							}
						}
						it->second.input.transform(obj);
					}
				}
			}
		}

		data::Value ret;
		if (_perms == Permission::Full) {
			for (auto &obj : data.asArray()) {
				data::Value n(getScheme().create(_adapter, obj));
				if (n) {
					ret.addValue(std::move(n));
				}
			}
		} else if (_perms == Permission::Partial) {
			for (auto &obj : data.asArray()) {
				data::Value val;
				if (isObjectAllowed(getScheme(), Action::Create, val, obj)) {
					data::Value n(getScheme().create(_adapter, obj));
					if (n) {
						ret.addValue(std::move(n));
					}
				}
			}
		}
		return processResultList(_queries, ret);
	}

	return data::Value();
}

data::Value ResourceReslist::createObject(data::Value &data, apr::array<InputFile> &file) {
	return performCreateObject(data, file, data::Value());
}

ResourceSet::ResourceSet(Adapter *a, QueryList &&q)
: ResourceReslist(a, move(q)) {
	_type = ResourceType::Set;
}

bool ResourceSet::prepareAppend() {
	_perms = isSchemeAllowed(getScheme(), Action::Update);
	return _perms != Permission::Restrict;
}
data::Value ResourceSet::createObject(data::Value &data, apr::array<InputFile> &file) {
	// write object patch
	data::Value extra;
	auto &items = _queries.getItems();
	auto &item = items.back();
	if (items.size() > 1 && item.ref) {
		// has subqueries, try to calculate origin
		if (auto id = items.at(items.size() - 2).query.getSingleSelectId()) {
			extra.setInteger(id, item.ref->getName());
		} else {
			auto ids = getDatabaseId(_queries, _queries.size() - 1);
			if (ids.size() == 1) {
				extra.setInteger(ids.front(), item.ref->getName());
			}
		}
	}
	if (!item.query.getSelectList().empty()) {
		// has select query, try to extract extra data
		for (auto &it : item.query.getSelectList()) {
			if (it.compare == storage::Comparation::Equal) {
				extra.setValue(it.value1, it.field);
			}
		}
	}
	return performCreateObject(data, file, extra);
}

data::Value ResourceSet::appendObject(data::Value &data) {
	// write object patch
	data::Value extra;
	auto &items = _queries.getItems();
	auto &item = items.back();
	if (items.size() > 1 && item.ref) {
		// has subqueries, try to calculate origin
		if (auto id = items.at(items.size() - 2).query.getSingleSelectId()) {
			extra.setInteger(id, item.ref->getName());
		} else {
			auto ids = getDatabaseId(_queries);
			if (ids.size() == 1 && ids.front()) {
				extra.setInteger(ids.front(), item.ref->getName());
			}
		}
	}

	if (extra.empty()) {
		return data::Value();
	}

	// collect object ids from input data
	data::Value val;
	if (data.isDictionary() && data.hasValue(item.ref->getName())) {
		val = std::move(data.getValue(item.ref->getName()));
	} else {
		val = std::move(data);
	}
	Vector<int64_t> ids;
	if (val.isArray()) {
		for (auto &it : val.asArray()) {
			auto i = it.asInteger();
			if (i) {
				ids.push_back(i);
			}
		}
	} else {
		auto i = val.asInteger();
		if (i) {
			ids.push_back(i);
		}
	}

	apr::array<InputFile> files;
	return performUpdate(ids, extra, files);
}


ResourceRefSet::ResourceRefSet(Adapter *a, QueryList &&q)
: ResourceSet(a, move(q)), _sourceScheme(_queries.getSourceScheme()), _field(_queries.getField()) {
	_type = ResourceType::ReferenceSet;
}

bool ResourceRefSet::prepareUpdate() {
	_refPerms = isSchemeAllowed(getScheme(), Action::Reference);
	_perms = min(_refPerms, isSchemeAllowed(*_sourceScheme, Action::Update));
	return _perms != Permission::Restrict;
}
bool ResourceRefSet::prepareCreate() {
	_refPerms = isSchemeAllowed(getScheme(), Action::Reference);
	_perms = min(_refPerms, isSchemeAllowed(*_sourceScheme, Action::Update));
	return _perms != Permission::Restrict;
}
bool ResourceRefSet::prepareAppend() {
	_refPerms = isSchemeAllowed(getScheme(), Action::Reference);
	_perms = min(_refPerms, isSchemeAllowed(*_sourceScheme, Action::Update));
	return _perms != Permission::Restrict;
}
bool ResourceRefSet::removeObject() {
	auto id = getObjectId();
	if (id == 0) {
		return data::Value();
	}

	_refPerms = isSchemeAllowed(getScheme(), Action::Reference);
	_perms = min(_refPerms, isSchemeAllowed(*_sourceScheme, Action::Update));

	if (_perms == Permission::Restrict) {
		return false;
	}

	return _adapter->performInTransaction([&] () -> bool {
		Vector<int64_t> objs;
		if (!isEmptyRequest()) {
			objs = getDatabaseId(_queries);
			if (objs.empty()) {
				return false;
			}
		}
		return doCleanup(id, _perms, objs);
	});
}
data::Value ResourceRefSet::updateObject(data::Value &value, apr::array<InputFile> &files) {
	if (value.isDictionary() && value.hasValue(_field->getName()) && (value.isBasicType(_field->getName()) || value.isArray(_field->getName()))) {
		value = value.getValue(_field->getName());
	}
	if (value.isBasicType() && !value.isNull()) {
		return doAppendObject(value, true);
	} else if (value.isArray()) {
		return doAppendObjects(value, true);
	} else {
		return ResourceSet::updateObject(value, files);
	}
}
data::Value ResourceRefSet::createObject(data::Value &value, apr::array<InputFile> &files) {
	encodeFiles(value, files);
	return appendObject(value);
}
data::Value ResourceRefSet::appendObject(data::Value &value) {
	if (value.isBasicType()) {
		return doAppendObject(value, false);
	} else if (value.isArray()) {
		return doAppendObjects(value, false);
	} else if (value.isDictionary()) {
		return doAppendObject(value, false);
	}
	return data::Value();
}

int64_t ResourceRefSet::getObjectId() {
	if (!_objectId) {
		auto ids = _adapter->performQueryListForIds(_queries, _queries.getItems().size() - 1);
		if (!ids.empty()) {
			_objectId = ids.front();
		}
	}
	return _objectId;
}

data::Value ResourceRefSet::getObjectValue() {
	if (!_objectValue) {
		_objectValue = _sourceScheme->get(_adapter, getObjectId(), false);
	}
	return _objectValue;
}

bool ResourceRefSet::isEmptyRequest() {
	if (_queries.getItems().back().query.empty()) {
		return true;
	}
	return false;
}

Vector<int64_t> ResourceRefSet::prepareAppendList(int64_t id, const data::Value &patch, bool cleanup) {
	Vector<int64_t> ids;
	if (patch.isArray() && patch.size() > 0) {
		auto createPerms = min(isSchemeAllowed(getScheme(), Action::Create), _refPerms);
		for (auto &it : patch.asArray()) {
			data::Value obj;
			if (it.isNull() || (it.isDictionary() && !it.hasValue("__oid"))) {
				if (createPerms == Permission::Full) {
					obj = getScheme().create(_adapter, it);
				} else if (createPerms == Permission::Partial) {
					data::Value val;
					data::Value patch(it);
					if (isObjectAllowed(getScheme(), Action::Create, val, patch)) {
						data::Value tmp = getScheme().create(_adapter, patch);
						if (_refPerms == Permission::Full || (tmp && isObjectAllowed(getScheme(), Action::Reference, tmp))) {
							obj = std::move(tmp);
						}
					}
				}
			} else {
				if (_refPerms == Permission::Full) {
					obj = getScheme().get(_adapter, it);
				} else if (_refPerms == Permission::Partial) {
					data::Value tmp = getScheme().get(_adapter, it);
					if (tmp && isObjectAllowed(getScheme(), Action::Reference, tmp)) {
						obj = std::move(tmp);
					}
				}
			}
			if (obj) {
				if (auto pushId = obj.getInteger("__oid")) {
					ids.push_back(pushId);
				}
			}
		}
	}

	return ids;
}

bool ResourceRefSet::doCleanup(int64_t id, Permission p, const Vector<int64_t> &objs) {
	if (p == Permission::Full) {
		if (objs.empty()) {
			_sourceScheme->clearProperty(_adapter, id, *_field);
		} else {
			data::Value objsData;
			for (auto &it : objs) {
				objsData.addInteger(it);
			}
			_sourceScheme->clearProperty(_adapter, id, *_field, move(objsData));
		}
	} else {
		auto obj = getObjectValue();
		data::Value patch;
		data::Value &arr = patch.emplace(_field->getName());
		for (auto &it : objs) {
			arr.addInteger(it);
		}
		if (isObjectAllowed(*_sourceScheme, Action::Update, obj, patch)) {
			if (patch.isNull(_field->getName())) {
				_sourceScheme->clearProperty(_adapter, id, *_field);
			} else {
				if (!_sourceScheme->clearProperty(_adapter, id, *_field, move(patch.getValue(_field->getName())))) {
					return false;
				}
			}
		}
	}
	return true;
}

data::Value ResourceRefSet::doAppendObject(const data::Value &val, bool cleanup) {
	data::Value arr;
	arr.addValue(val);
	return doAppendObjects(arr, cleanup);
}

data::Value ResourceRefSet::doAppendObjects(const data::Value &val, bool cleanup) {
	data::Value ret;
	_perms = isSchemeAllowed(*_sourceScheme, Action::Update);
	_adapter->performInTransaction([&] { // all or nothing
		return doAppendObjectsTransaction(ret, val, cleanup);
	});

	if (!_queries.getFields().getFields()->empty()) {
		return processResultList(_queries, ret);
	}

	return ret;
}

bool ResourceRefSet::doAppendObjectsTransaction(data::Value &ret, const data::Value &val, bool cleanup) {
	auto id = getObjectId();
	if (id == 0) {
		return false;
	}

	_perms = isSchemeAllowed(*_sourceScheme, Action::Update);

	Vector<int64_t> ids;
	if (_perms == AccessControl::Full) {
		ids = prepareAppendList(id, val, cleanup);
	} else {
		auto obj = getObjectValue();
		data::Value patch;
		patch.setValue(val, _field->getName());
		if (isObjectAllowed(*_sourceScheme, Action::Append, obj, patch)) {
			ids = prepareAppendList(id, patch.getValue(_field->getName()), cleanup);
		}
	}

	if (ids.empty()) {
		messages::error("ResourceRefSet", "Empty changeset id list in update/append action", data::Value{
			pair("sourceScheme", data::Value(_sourceScheme->getName())),
			pair("targetScheme", data::Value(getScheme().getName())),
		});
		return false;
	}

	data::Value patch;
	for (auto &it : ids) {
		patch.addInteger(it);
	}

	if (cleanup) {
		ret = _sourceScheme->setProperty(_adapter, id, *_field, move(patch));
	} else {
		ret = _sourceScheme->appendProperty(_adapter, id, *_field, move(patch));
	}

	return !ret.empty();
}

ResourceProperty::ResourceProperty(Adapter *a, QueryList &&q, const Field *prop)
: Resource(ResourceType::File, a, move(q)), _field(prop) {
	_queries.setProperty(prop);
}

bool ResourceProperty::removeObject() {
	auto perms = isSchemeAllowed(getScheme(), Action::Update);
	switch (perms) {
	case Permission::Full:
		// perform one-line remove
		return _adapter->performInTransaction([&] () -> bool {
			if (auto id = getObjectId()) {
				if (getScheme().update(_adapter, id, data::Value{ pair(_field->getName(), data::Value()) })) {
					return true;
				}
			}
			_status = HTTP_CONFLICT;
			return false;
		});
		break;
	case Permission::Partial:
		return _adapter->performInTransaction([&] () -> bool {
			data::Value patch { pair(_field->getName(), data::Value()) };
			data::Value object(getObject(true));
			if (object && isObjectAllowed(getScheme(), Action::Update, object, patch)) {
				if (getScheme().update(_adapter, object.getInteger("__oid"), patch)) {
					return true;
				} else {
					_status = HTTP_CONFLICT;
				}
			} else {
				_status = HTTP_FORBIDDEN;
			}
			return false;
		});
		break;
	case Permission::Restrict:
		return false;
		break;
	}
	return false;
}

uint64_t ResourceProperty::getObjectId() {
	auto ids = _adapter->performQueryListForIds(_queries);
	return ids.empty() ? 0 : ids.front();
}

data::Value ResourceProperty::getObject(bool forUpdate) {
	data::Value ret = _adapter->performQueryList(_queries, _queries.size(), forUpdate);
	if (ret.isArray() && ret.size() > 0) {
		ret = move(ret.getValue(0));
	}
	return ret;
}


ResourceFile::ResourceFile(Adapter *a, QueryList &&q, const Field *prop)
: ResourceProperty(a, move(q), prop) {
	_type = ResourceType::File;
}

bool ResourceFile::prepareUpdate() {
	_perms = isSchemeAllowed(getScheme(), Action::Update);
	return _perms != Permission::Restrict;
}
bool ResourceFile::prepareCreate() {
	_perms = isSchemeAllowed(getScheme(), Action::Update);
	return _perms != Permission::Restrict;
}
data::Value ResourceFile::updateObject(data::Value &, apr::array<InputFile> &f) {
	if (f.empty()) {
		_status = HTTP_BAD_REQUEST;
		return data::Value();
	}

	InputFile *file = nullptr;
	for (auto &it : f) {
		if (it.name == _field->getName() || it.name == "content") {
			file = &it;
			break;
		} else if (it.name.empty()) {
			it.name = _field->getName();
			file = &it;
			break;
		}
	}

	for (auto &it : f) {
		if (it.name != _field->getName() && &it != file) {
			it.close();
		}
	}

	if (!file) {
		_status = HTTP_BAD_REQUEST;
		return data::Value();
	}

	if (file->name != _field->getName()) {
		file->name = _field->getName();
	}

	data::Value patch;
	patch.setInteger(file->negativeId(), _field->getName());

	if (_perms == Permission::Full) {
		// perform one-line update
		if (auto id = getObjectId()) {
			auto ret = getScheme().update(_adapter, id, patch);
			ret = getFileForObject(ret);
			return ret;
		}
		return data::Value();
	} else if (_perms == Permission::Restrict) {
		return data::Value();
	} else {
		// check on object level
		data::Value ret;
		_adapter->performInTransaction([&] {
			data::Value object(getObject(true));
			if (object && isObjectAllowed(getScheme(), Action::Update, object, patch)) {
				ret = getScheme().update(_adapter, object.getInteger("__oid"), patch);
				ret = getFileForObject(ret);
			} else {
				_status = HTTP_FORBIDDEN;
				return false;
			}
			return true;
		});
		return ret;
	}
	return data::Value();
}
data::Value ResourceFile::createObject(data::Value &val, apr::array<InputFile> &f) {
	// same as update
	return updateObject(val, f);
}

data::Value ResourceFile::getResultObject() {
	if (_field->hasFlag(storage::Flags::Protected)) {
		_status = HTTP_NOT_FOUND;
		return data::Value();
	}
	_perms = isSchemeAllowed(getScheme(), Action::Read);
	if (_perms == Permission::Full) {
		return getDatabaseObject();
	} else if (_perms == Permission::Restrict) {
		return data::Value();
	} else {
		data::Value object(getObject(false));
		return getFileForObject(object);
	}
}

data::Value ResourceFile::getFileForObject(data::Value &object) {
	if (object.isDictionary()) {
		if (isObjectAllowed(getScheme(), Action::Read, object)) {
			auto id = object.getInteger(_field->getName());
			if (id) {
				auto fileScheme = Server(apr::pool::server()).getFileScheme();
				data::Value ret(fileScheme->get(_adapter, id));
				return ret;
			}
		}
	}
	return data::Value();
}

data::Value ResourceFile::getDatabaseObject() {
	return _adapter->performQueryList(_queries, _queries.size(), false, _field);
}

ResourceArray::ResourceArray(Adapter *a, QueryList &&q, const Field *prop)
: ResourceProperty(a, move(q), prop) {
	_type = ResourceType::Array;
}

bool ResourceArray::prepareUpdate() {
	_perms = isSchemeAllowed(getScheme(), Action::Update);
	return _perms != Permission::Restrict;
}
bool ResourceArray::prepareCreate() {
	_perms = isSchemeAllowed(getScheme(), Action::Append);
	return _perms != Permission::Restrict;
}
data::Value ResourceArray::updateObject(data::Value &data, apr::array<InputFile> &) {
	data::Value arr;
	if (data.isDictionary()) {
		auto &newArr = data.getValue(_field->getName());
		if (newArr.isArray()) {
			arr = std::move(newArr);
		} else if (!newArr.isNull()) {
			arr.addValue(newArr);
		}
	} else if (data.isArray()) {
		arr = std::move(data);
	}

	if (!arr.isArray()) {
		_status = HTTP_BAD_REQUEST;
		return data::Value();
	}

	if (_perms == Permission::Full) {
		// perform one-line update
		if (auto id = getObjectId()) {
			return getScheme().setProperty(_adapter, id, *_field, std::move(arr));
		}
		return data::Value();
	} else if (_perms == Permission::Restrict) {
		return data::Value();
	} else {
		// check on object level
		data::Value ret;
		_adapter->performInTransaction([&] {
			data::Value object(getObject(true));
			data::Value patch; patch.setValue(std::move(arr), _field->getName());
			if (object && isObjectAllowed(getScheme(), Action::Update, object, patch)) {
				ret = getScheme().setProperty(_adapter, object.getInteger("__oid"), *_field, std::move(arr));;
			} else {
				_status = HTTP_FORBIDDEN;
				return false;
			}
			return true;
		});
		return ret;
	}
	return data::Value();
}
data::Value ResourceArray::createObject(data::Value &data, apr::array<InputFile> &) {
	data::Value arr;
	if (data.isDictionary()) {
		auto &newArr = data.getValue(_field->getName());
		if (newArr.isArray()) {
			arr = std::move(newArr);
		} else if (!newArr.isNull()) {
			arr.addValue(newArr);
		}
	} else if (data.isArray()) {
		arr = std::move(data);
	} else if (data.isBasicType()) {
		arr.addValue(std::move(data));
	}

	if (!arr.isArray()) {
		_status = HTTP_BAD_REQUEST;
		return data::Value(false);
	}

	if (_perms == Permission::Full) {
		// perform one-line update
		if (auto id = getObjectId()) {
			return getScheme().appendProperty(_adapter, id, *_field, move(arr));
		}
		return data::Value();
	} else if (_perms == Permission::Restrict) {
		return data::Value();
	} else {
		// check on object level
		data::Value ret;
		_adapter->performInTransaction([&] {
			data::Value object(getObject(true));
			data::Value patch; patch.setValue(arr, _field->getName());
			if (object && object.isInteger("__oid") && isObjectAllowed(getScheme(), Action::Append, object, patch)) {
				ret = getScheme().appendProperty(_adapter, object.getInteger("__oid"), *_field, move(arr));
			} else {
				_status = HTTP_FORBIDDEN;
				return false;
			}
			return true;
		});
		return ret;
	}

	return data::Value();
}

data::Value ResourceArray::getResultObject() {
	if (_field->hasFlag(storage::Flags::Protected)) {
		_status = HTTP_NOT_FOUND;
		return data::Value();
	}
	_perms = isSchemeAllowed(getScheme(), Action::Read);
	if (_perms == Permission::Full) {
		return getDatabaseObject();
	} else if (_perms == Permission::Restrict) {
		return data::Value();
	} else {
		data::Value object(getObject(false));
		return getArrayForObject(object);
	}
}

data::Value ResourceArray::getDatabaseObject() {
	return _adapter->performQueryList(_queries, _queries.size(), false, _field);
}

data::Value ResourceArray::getArrayForObject(data::Value &object) {
	if (object.isDictionary()) {
		if (isObjectAllowed(getScheme(), Action::Read, object)) {
			return getScheme().getProperty(_adapter, object, *_field);
		}
	}
	return data::Value();
}


ResourceFieldObject::ResourceFieldObject(Adapter *a, QueryList &&q)
: ResourceObject(a, move(q)), _sourceScheme(_queries.getSourceScheme()), _field(_queries.getField()) {
	_type = ResourceType::ObjectField;
}

bool ResourceFieldObject::prepareUpdate() {
	_refPerms = isSchemeAllowed(getScheme(), Action::Update);
	_perms = min(_refPerms, isSchemeAllowed(*_sourceScheme, Action::Update));
	return _perms != Permission::Restrict;
}

bool ResourceFieldObject::prepareCreate() {
	_refPerms = isSchemeAllowed(getScheme(), Action::Update);
	_perms = min(_refPerms, isSchemeAllowed(*_sourceScheme, Action::Update));
	return _perms != Permission::Restrict;
}

bool ResourceFieldObject::prepareAppend() {
	return false;
}

bool ResourceFieldObject::removeObject() {
	auto id = getObjectId();
	if (id == 0) {
		return data::Value();
	}

	_refPerms = isSchemeAllowed(getScheme(), Action::Remove);
	_perms = min(_refPerms, isSchemeAllowed(*_sourceScheme, Action::Update));

	if (_perms == Permission::Restrict) {
		return false;
	}

	return _adapter->performInTransaction([&] () -> bool {
		return doRemoveObject();
	});
}

data::Value ResourceFieldObject::updateObject(data::Value &val, apr::array<InputFile> &files) {
	// create or update object
	data::Value ret;
	_adapter->performInTransaction([&] () -> bool {
		if (getObjectId()) {
			ret = doUpdateObject(val, files);
		} else {
			ret = doCreateObject(val, files);
		}
		if (ret) {
			return true;
		}
		return false;
	});
	return ret;
}

data::Value ResourceFieldObject::createObject(data::Value &val, apr::array<InputFile> &files) {
	// remove then recreate object
	data::Value ret;
	_adapter->performInTransaction([&] () -> bool {
		if (getObjectId()) {
			if (!doRemoveObject()) {
				return data::Value();
			}
		}
		ret = doCreateObject(val, files);
		if (ret) {
			return true;
		}
		return false;
	});
	return ret;
}

data::Value ResourceFieldObject::appendObject(data::Value &) {
	return data::Value();
}

int64_t ResourceFieldObject::getRootId() {
	if (!_rootId) {
		auto ids = _adapter->performQueryListForIds(_queries, _queries.getItems().size() - 1);
		if (!ids.empty()) {
			_rootId = ids.front();
		}
	}
	return _rootId;
}

int64_t ResourceFieldObject::getObjectId() {
	if (!_objectId) {
		if (auto id = getRootId()) {
			if (auto obj = getScheme().get(_adapter, id, {_field->getName()})) {
				_objectId = obj.getValue(_field->getName());
			}
		}
	}
	return _objectId;
}

data::Value ResourceFieldObject::getRootObject(bool forUpdate) {
	if (auto id = getRootId()) {
		return _sourceScheme->get(_adapter, id, {_field->getName()}, forUpdate);
	}
	return data::Value();
}

data::Value ResourceFieldObject::getTargetObject(bool forUpdate) {
	if (auto id = getObjectId()) {
		return getScheme().get(_adapter, id, { nullptr }, forUpdate);
	}
	return data::Value();
}

bool ResourceFieldObject::doRemoveObject() {
	if (_perms == Permission::Full) {
		return _sourceScheme->clearProperty(_adapter, getRootId(), *_field);
	} else {
		auto rootObj = getRootObject(false);
		auto targetObj = getTargetObject(false);
		if (!rootObj || !targetObj) {
			return false;
		}

		data::Value patch { pair(_field->getName(), data::Value()) };
		if (isObjectAllowed(*_sourceScheme, Action::Update, rootObj, patch)) {
			if (isObjectAllowed(getScheme(), Action::Remove, targetObj)) {
				if (_sourceScheme->clearProperty(_adapter, getRootId(), *_field)) {
					return true;
				}
			}
		}

		return false;
	}
}

data::Value ResourceFieldObject::doUpdateObject(data::Value &val, apr::array<InputFile> &files) {
	encodeFiles(val, files);
	if (_perms == Permission::Full) {
		return getScheme().update(_adapter, getObjectId(), val);
	} else {
		auto rootObj = getRootObject(false);
		auto targetObj = getTargetObject(true);
		if (!rootObj || !targetObj) {
			return data::Value();
		}

		data::Value patch { pair(_field->getName(), data::Value(move(val))) };
		if (isObjectAllowed(*_sourceScheme, Action::Update, rootObj, patch)) {
			if (auto &v = patch.getValue(_field->getName())) {
				if (isObjectAllowed(getScheme(), Action::Update, targetObj, v)) {
					return getScheme().update(_adapter, getObjectId(), v);
				}
			}
		}

		return data::Value();
	}
}

data::Value ResourceFieldObject::doCreateObject(data::Value &val, apr::array<InputFile> &files) {
	encodeFiles(val, files);
	if (_perms == Permission::Full) {
		if (auto ret = getScheme().create(_adapter, val)) {
			if (auto id = ret.getInteger("__oid")) {
				if (_sourceScheme->update(_adapter, getRootId(), data::Value{
					pair(_field->getName(), data::Value(id))
				})) {
					return ret;
				}
			}
		}
	} else {
		auto rootObj = getRootObject(true);
		if (!rootObj) {
			return data::Value();
		}

		data::Value patch { pair(_field->getName(), data::Value(move(val))) };
		if (isObjectAllowed(*_sourceScheme, Action::Update, rootObj, patch)) {
			if (auto &v = patch.getValue(_field->getName())) {
				data::Value p;
				if (isObjectAllowed(getScheme(), Action::Create, p, v)) {
					if (auto ret = getScheme().create(_adapter, v)) {
						if (auto id = ret.getInteger("__oid")) {
							if (_sourceScheme->update(_adapter, getRootId(), data::Value{
								pair(_field->getName(), data::Value(id))
							})) {
								return ret;
							}
						}
					}
				}
			}
		}
	}
	return data::Value();
}


ResourceView::ResourceView(Adapter *h, QueryList &&q)
: ResourceSet(h, move(q)), _field(_queries.getField()) {
	if (_queries.isDeltaApplicable()) {
		auto tag = _queries.getItems().front().query.getSingleSelectId();
		_delta = Time::microseconds(_adapter->getDeltaValue(*_queries.getSourceScheme(), *static_cast<const storage::FieldView *>(_field->getSlot()), tag));
	}
}

bool ResourceView::prepareUpdate() { return false; }
bool ResourceView::prepareCreate() { return false; }
bool ResourceView::prepareAppend() { return false; }
bool ResourceView::removeObject() { return false; }

data::Value ResourceView::updateObject(data::Value &data, apr::array<InputFile> &) { return data::Value(); }
data::Value ResourceView::createObject(data::Value &data, apr::array<InputFile> &) { return data::Value(); }

data::Value ResourceView::getResultObject() {
	auto ret = _adapter->performQueryList(_queries, maxOf<size_t>(), false, _field);
	if (!ret.isArray()) {
		return data::Value();
	}

	return processResultList(_queries, ret);
}

NS_SA_END
