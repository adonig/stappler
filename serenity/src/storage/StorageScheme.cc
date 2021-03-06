/**
Copyright (c) 2016-2018 Roman Katuntsev <sbkarr@stappler.org>

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
#include "StorageScheme.h"
#include "StorageObject.h"
#include "StorageFile.h"
#include "StorageAdapter.h"

NS_SA_EXT_BEGIN(storage)

bool Scheme::initSchemes(Server &serv, const Map<String, const Scheme *> &schemes) {
	for (auto &it : schemes) {
		const_cast<Scheme *>(it.second)->initScheme();
		for (auto &fit : it.second->getFields()) {
			if (fit.second.getType() == Type::View) {
				auto slot = static_cast<const FieldView *>(fit.second.getSlot());
				if (slot->scheme) {
					const_cast<Scheme *>(slot->scheme)->addView(it.second, &fit.second);
				}
			}
			if (fit.second.hasFlag(Flags::Composed) && (fit.second.getType() == Type::Object || fit.second.getType() == Type::Set)) {
				auto slot = static_cast<const FieldObject *>(fit.second.getSlot());
				if (slot->scheme) {
					const_cast<Scheme *>(slot->scheme)->addParent(it.second, &fit.second);
				}
			}
		}
	}
	return true;
}

Scheme::Scheme(const String &ns, bool delta) : name(ns), delta(delta) { }

Scheme::Scheme(const String &name, std::initializer_list<Field> il, bool delta) :  Scheme(name, delta) {
	for (auto &it : il) {
		auto &fname = it.getName();
		fields.emplace(fname, std::move(const_cast<Field &>(it)));
	}

	updateLimits();
}

void Scheme::setDelta(bool v) {
	delta = v;
}

bool Scheme::hasDelta() const {
	return delta;
}

void Scheme::define(std::initializer_list<Field> il) {
	for (auto &it : il) {
		auto &fname = it.getName();
		if (it.getType() == Type::Image) {
			auto image = static_cast<const FieldImage *>(it.getSlot());
			auto &thumbnails = image->thumbnails;
			for (auto & thumb : thumbnails) {
				auto new_f = fields.emplace(thumb.name, Field::Image(String(thumb.name), MaxImageSize(thumb.width, thumb.height))).first;
				((FieldImage *)(new_f->second.getSlot()))->primary = false;
			}
		}
		fields.emplace(fname, std::move(const_cast<Field &>(it)));
	}

	updateLimits();
}

void Scheme::define(Vector<Field> &&il) {
	for (auto &it : il) {
		auto &fname = it.getName();
		if (it.getType() == Type::Image) {
			auto image = static_cast<const FieldImage *>(it.getSlot());
			auto &thumbnails = image->thumbnails;
			for (auto & thumb : thumbnails) {
				auto new_f = fields.emplace(thumb.name, Field::Image(String(thumb.name), MaxImageSize(thumb.width, thumb.height))).first;
				((FieldImage *)(new_f->second.getSlot()))->primary = false;
			}
		}
		fields.emplace(fname, std::move(const_cast<Field &>(it)));
	}

	updateLimits();
}

void Scheme::cloneFrom(Scheme *source) {
	for (auto &it : source->fields) {
		fields.emplace(it.first, it.second);
	}
}

const String &Scheme::getName() const {
	return name;
}
bool Scheme::hasAliases() const {
	for (auto &it : fields) {
		if (it.second.getType() == Type::Text && it.second.getTransform() == storage::Transform::Alias) {
			return true;
		}
	}
	return false;
}

bool Scheme::isProtected(const StringView &key) const {
	auto it = fields.find(key);
	if (it != fields.end()) {
		return it->second.isProtected();
	}
	return false;
}

const Set<const Field *> & Scheme::getForceInclude() const {
	return forceInclude;
}

const Map<String, Field> & Scheme::getFields() const {
	return fields;
}

const Field *Scheme::getField(const StringView &key) const {
	auto it = fields.find(key);
	if (it != fields.end()) {
		return &it->second;
	}
	return nullptr;
}

const Field *Scheme::getForeignLink(const FieldObject *f) const {
	if (!f || f->onRemove == RemovePolicy::Reference || f->onRemove == RemovePolicy::StrongReference) {
		return nullptr;
	}
	auto &link = f->link;
	auto nextScheme = f->scheme;
	if (f->linkage == Linkage::Auto) {
		auto &nextFields = nextScheme->getFields();
		for (auto &it : nextFields) {
			auto &nextField = it.second;
			if (nextField.getType() == Type::Object
					|| (nextField.getType() == Type::Set && f->getType() == Type::Object)) {
				auto nextSlot = static_cast<const FieldObject *>(nextField.getSlot());
				if (nextSlot->scheme == this) {
					return &nextField;
				}
			}
		}
	} else if (f->linkage == Linkage::Manual) {
		auto nextField = nextScheme->getField(link);
		if (nextField && (nextField->getType() == Type::Object
				|| (nextField->getType() == Type::Set && f->getType() == Type::Object))) {
			auto nextSlot = static_cast<const FieldObject *>(nextField->getSlot());
			if (nextSlot->scheme == this) {
				return nextField;
			}
		}
	}
	return nullptr;
}

const Field *Scheme::getForeignLink(const Field &f) const {
	if (f.getType() == Type::Set || f.getType() == Type::Object) {
		auto slot = static_cast<const FieldObject *>(f.getSlot());
		return getForeignLink(slot);
	}
	return nullptr;
}
const Field *Scheme::getForeignLink(const StringView &fname) const {
	auto f = getField(fname);
	if (f) {
		return getForeignLink(*f);
	}
	return nullptr;
}

bool Scheme::isAtomicPatch(const data::Value &val) const {
	if (val.isDictionary()) {
		for (auto &it : val.asDict()) {
			auto f = getField(it.first);
			// extra field should use select-update
			if (f && (f->getType() == Type::Extra || forceInclude.find(f) != forceInclude.end())) {
				return false;
			}
		}
		return true;
	}
	return false;
}

uint64_t Scheme::hash(ValidationLevel l) const {
	StringStream stream;
	for (auto &it : fields) {
		it.second.hash(stream, l);
	}
	return std::hash<String>{}(stream.weak());
}

const Vector<Scheme::ViewScheme *> &Scheme::getViews() const {
	return views;
}

Vector<const Field *> Scheme::getPatchFields(const data::Value &patch) const {
	Vector<const Field *> ret; ret.reserve(patch.size());
	for (auto &it : patch.asDict()) {
		if (auto f = getField(it.first)) {
			ret.emplace_back(f);
		}
	}
	return ret;
}

bool Scheme::saveObject(Adapter *adapter, Object *obj) const {
	return adapter->saveObject(*this, obj->getObjectId(), obj->_data, Vector<String>());
}

bool Scheme::hasFiles() const {
	for (auto &it : fields) {
		if (it.second.isFile()) {
			return true;
		}
	}
	return false;
}

static void prepareGetQuery(Query &query, uint64_t oid, bool forUpdate) {
	query.select(oid);
	if (forUpdate) {
		query.forUpdate();
	}
}

static void prepareGetQuery(Query &query, const String &alias, bool forUpdate) {
	query.select(alias);
	if (forUpdate) {
		query.forUpdate();
	}
}

static data::Value reduceGetQuery(data::Value &&ret) {
	if (ret.isArray() && ret.size() >= 1) {
		return ret.getValue(0);
	}
	return data::Value();
}

data::Value Scheme::get(Adapter *adapter, uint64_t oid, bool forUpdate) const {
	Query query;
	prepareGetQuery(query, oid, forUpdate);
	return reduceGetQuery(adapter->selectObjects(*this, query));
}

data::Value Scheme::get(Adapter *adapter, const String &alias, bool forUpdate) const {
	if (!hasAliases()) {
		return data::Value();
	}

	Query query;
	prepareGetQuery(query, alias, forUpdate);
	return reduceGetQuery(adapter->selectObjects(*this, query));
}

data::Value Scheme::get(Adapter *adapter, const data::Value &id, bool forUpdate) const {
	if (id.isDictionary()) {
		if (auto oid = id.getInteger("__oid")) {
			return get(adapter, oid, forUpdate);
		}
	} else {
		if ((id.isString() && valid::validateNumber(id.getString())) || id.isInteger()) {
			if (auto oid = id.getInteger()) {
				return get(adapter, oid, forUpdate);
			}
		}

		auto &str = id.getString();
		if (!str.empty()) {
			return get(adapter, str, forUpdate);
		}
	}
	return data::Value();
}

data::Value Scheme::get(Adapter *adapter, uint64_t oid, std::initializer_list<StringView> &&fields, bool forUpdate) const {
	Query query;
	prepareGetQuery(query, oid, forUpdate);
	for (auto &it : fields) {
		if (auto f = getField(it)) {
			query.include(String(f->getName()));
		}
	}
	return reduceGetQuery(adapter->selectObjects(*this, query));
}
data::Value Scheme::get(Adapter *adapter, const String &alias, std::initializer_list<StringView> &&fields, bool forUpdate) const {
	Query query;
	prepareGetQuery(query, alias, forUpdate);
	for (auto &it : fields) {
		if (auto f = getField(it)) {
			query.include(String(f->getName()));
		}
	}
	return reduceGetQuery(adapter->selectObjects(*this, query));
}
data::Value Scheme::get(Adapter *adapter, const data::Value &id, std::initializer_list<StringView> &&fields, bool forUpdate) const {
	if (id.isDictionary()) {
		if (auto oid = id.getInteger("__oid")) {
			return get(adapter, oid, move(fields), forUpdate);
		}
	} else {
		if ((id.isString() && valid::validateNumber(id.getString())) || id.isInteger()) {
			if (auto oid = id.getInteger()) {
				return get(adapter, oid, move(fields), forUpdate);
			}
		}

		auto &str = id.getString();
		if (!str.empty()) {
			return get(adapter, str, move(fields), forUpdate);
		}
	}
	return data::Value();
}

data::Value Scheme::get(Adapter *adapter, uint64_t oid, std::initializer_list<const Field *> &&fields, bool forUpdate) const {
	Query query;
	prepareGetQuery(query, oid, forUpdate);
	for (auto &it : fields) {
		query.include(String(it->getName()));
	}
	return reduceGetQuery(adapter->selectObjects(*this, query));
}
data::Value Scheme::get(Adapter *adapter, const String &alias, std::initializer_list<const Field *> &&fields, bool forUpdate) const {
	Query query;
	prepareGetQuery(query, alias, forUpdate);
	for (auto &it : fields) {
		query.include(String(it->getName()));
	}
	return reduceGetQuery(adapter->selectObjects(*this, query));
}
data::Value Scheme::get(Adapter *adapter, const data::Value &id, std::initializer_list<const Field *> &&fields, bool forUpdate) const {
	if (id.isDictionary()) {
		if (auto oid = id.getInteger("__oid")) {
			return get(adapter, oid, move(fields), forUpdate);
		}
	} else {
		if ((id.isString() && valid::validateNumber(id.getString())) || id.isInteger()) {
			if (auto oid = id.getInteger()) {
				return get(adapter, oid, move(fields), forUpdate);
			}
		}

		auto &str = id.getString();
		if (!str.empty()) {
			return get(adapter, str, move(fields), forUpdate);
		}
	}
	return data::Value();
}

data::Value Scheme::create(Adapter *adapter, const data::Value &data, bool isProtected) const {
	if (!data.isDictionary()) {
		messages::error("Storage", "Invalid data for object");
		return data::Value();
	}

	data::Value changeSet = data;
	transform(changeSet, isProtected?TransformAction::ProtectedCreate:TransformAction::Create);

	bool stop = false;
	for (auto &it : fields) {
		auto &val = changeSet.getValue(it.first);
		if (val.isNull() && it.second.hasFlag(Flags::Required)) {
			messages::error("Storage", "No value for required field",
					data::Value{ std::make_pair("field", data::Value(it.first)) });
			stop = true;
		}
	}

	if (stop) {
		return data::Value();
	}

	if (adapter->performInTransaction([&] () -> bool {
		data::Value patch(createFilePatch(adapter, data));
		if (patch.isDictionary()) {
			for (auto &it : patch.asDict()) {
				changeSet.setValue(it.second, it.first);
			}
		}

		if (adapter->createObject(*this, changeSet)) {
			touchParents(adapter, changeSet);
			for (auto &it : views) {
				updateView(adapter, changeSet, it);
			}
			return true;
		} else {
			if (patch.isDictionary()) {
				purgeFilePatch(adapter, patch);
			}
		}
		return false;
	})) {
		return changeSet;
	}

	return data::Value();
}

data::Value Scheme::update(Adapter *adapter, uint64_t oid, const data::Value &data, bool isProtected) const {
	bool success = false;
	data::Value changeSet;

	std::tie(success, changeSet) = prepareUpdate(data, isProtected);
	if (!success) {
		return data::Value();
	}

	data::Value ret;
	adapter->performInTransaction([&] () -> bool {
		data::Value filePatch(createFilePatch(adapter, data));
		if (filePatch.isDictionary()) {
			for (auto &it : filePatch.asDict()) {
				changeSet.setValue(it.second, it.first);
			}
		}

		if (changeSet.empty()) {
			messages::error("Storage", "Empty changeset for id", data::Value{ std::make_pair("oid", data::Value((int64_t)oid)) });
			return false;
		}

		ret = patchOrUpdate(adapter, oid, changeSet);
		if (ret.isNull()) {
			if (filePatch.isDictionary()) {
				purgeFilePatch(adapter, filePatch);
			}
			messages::error("Storage", "Fail to update object for id", data::Value{ std::make_pair("oid", data::Value((int64_t)oid)) });
			return false;
		}
		return true;
	});

	return ret;
}

data::Value Scheme::update(Adapter *adapter, const data::Value & obj, const data::Value &data, bool isProtected) const {
	uint64_t oid = obj.getInteger("__oid");
	if (!oid) {
		messages::error("Storage", "Invalid data for object");
		return data::Value();
	}

	bool success = false;
	data::Value changeSet;

	std::tie(success, changeSet) = prepareUpdate(data, isProtected);
	if (!success) {
		return data::Value();
	}

	data::Value ret;
	adapter->performInTransaction([&] () -> bool {
		data::Value filePatch(createFilePatch(adapter, data));
		if (filePatch.isDictionary()) {
			for (auto &it : filePatch.asDict()) {
				changeSet.setValue(it.second, it.first);
			}
		}

		if (changeSet.empty()) {
			messages::error("Storage", "Empty changeset for id", data::Value{ std::make_pair("oid", data::Value((int64_t)oid)) });
			return false;
		}

		ret = patchOrUpdate(adapter, obj, changeSet);
		if (ret.isNull()) {
			if (filePatch.isDictionary()) {
				purgeFilePatch(adapter, filePatch);
			}
			messages::error("Storage", "No object for id", data::Value{ std::make_pair("oid", data::Value((int64_t)oid)) });
			return false;
		}
		return true;
	});

	return ret;
}

void Scheme::mergeValues(const Field &f, data::Value &original, data::Value &newVal) const {
	if (f.getType() == Type::Extra) {
		if (newVal.isDictionary()) {
			auto &extraFields = static_cast<const FieldExtra *>(f.getSlot())->fields;
			for (auto &it : newVal.asDict()) {
				auto f_it = extraFields.find(it.first);
				if (f_it != extraFields.end()) {
					if (!it.second.isNull()) {
						if (auto &val = original.getValue(it.first)) {
							mergeValues(f_it->second, val, it.second);
						} else {
							original.setValue(std::move(it.second), it.first);
						}
					} else {
						original.erase(it.first);
					}
				}
			}
		}
	} else {
		original.setValue(std::move(newVal));
	}
}

Pair<bool, data::Value> Scheme::prepareUpdate(const data::Value &data, bool isProtected) const {
	if (!data.isDictionary()) {
		messages::error("Storage", "Invalid changeset data for object");
		return pair(false, data::Value());
	}

	data::Value changeSet = data;
	transform(changeSet, isProtected?TransformAction::ProtectedUpdate:TransformAction::Update);

	bool stop = false;
	for (auto &it : fields) {
		if (changeSet.hasValue(it.first)) {
			auto &val = changeSet.getValue(it.first);
			if (val.isNull() && it.second.hasFlag(Flags::Required)) {
				messages::error("Storage", "Value for required field can not be removed",
						data::Value{ std::make_pair("field", data::Value(it.first)) });
				stop = true;
			}
		}
	}

	if (stop) {
		return pair(false, data::Value());
	}

	return pair(true, changeSet);
}

void Scheme::touchParents(Adapter *adapter, const data::Value &obj) const {
	if (!parents.empty()) {
		Map<int64_t, const Scheme *> parentsToUpdate;
		extractParents(parentsToUpdate, adapter, obj);
		for (auto &it : parentsToUpdate) {
			it.second->touch(adapter, it.first);
		}
	}
}

void Scheme::extractParents(Map<int64_t, const Scheme *> &parentsToUpdate, Adapter *adapter, const data::Value &obj, bool isChangeSet) const {
	auto id = obj.getInteger("__oid");
	for (auto &it : parents) {
		if (it->backReference) {
			if (auto value = obj.getInteger(it->backReference->getName())) {
				parentsToUpdate.emplace(value, it->scheme);
			}
		} else if (!isChangeSet && id) {
			auto vec = adapter->getReferenceParents(*this, id, it->scheme, it->pointerField);
			for (auto &value : vec) {
				parentsToUpdate.emplace(value, it->scheme);
			}
		}
	}
}

data::Value Scheme::updateObject(Adapter *adapter, data::Value && obj, data::Value &changeSet) const {
	Vector<const ViewScheme *> viewsToUpdate; viewsToUpdate.reserve(views.size());
	Map<int64_t, const Scheme *> parentsToUpdate;

	data::Value replacements;

	if (!parents.empty()) {
		extractParents(parentsToUpdate, adapter, obj);
		extractParents(parentsToUpdate, adapter, changeSet, true);
	}

	// apply changeset
	Vector<String> updatedFields;
	for (auto &it : changeSet.asDict()) {
		auto &fieldName = it.first;
		if (auto f = getField(fieldName)) {
			if (!it.second.isNull()) {
				if (auto &val = obj.getValue(it.first)) {
					mergeValues(*f, val, it.second);
				} else {
					obj.setValue(move(it.second), it.first);
				}
			} else {
				obj.erase(it.first);
			}
			updatedFields.emplace_back(it.first);

			if (forceInclude.find(f) != forceInclude.end()) {
				for (auto &it : views) {
					if (it->fields.find(f) != it->fields.end()) {
						auto lb = std::lower_bound(viewsToUpdate.begin(), viewsToUpdate.end(), it);
						if (lb == viewsToUpdate.end() && *lb != it) {
							viewsToUpdate.emplace(lb, it);
						}
					}
				}
			}
		}
	}

	if (!viewsToUpdate.empty() || !parentsToUpdate.empty()) {
		if (adapter->performInTransaction([&] {
			if (adapter->saveObject(*this, obj.getInteger("__oid"), obj, updatedFields)) {
				for (auto &it : parentsToUpdate) {
					it.second->touch(adapter, it.first);
				}
				for (auto &it : viewsToUpdate) {
					updateView(adapter, obj, it);
				}
				return true;
			}
			return false;
		})) {
			return obj;
		}
	} else if (adapter->saveObject(*this, obj.getInteger("__oid"), obj, updatedFields)) {
		return obj;
	}

	return data::Value();
}

void Scheme::touch(Adapter *adapter, uint64_t id) const {
	data::Value patch;
	transform(patch, TransformAction::Touch);
	patchOrUpdate(adapter, id, patch, EmptyFieldList());
}

void Scheme::touch(Adapter *adapter, const data::Value & obj) const {
	data::Value patch;
	transform(patch, TransformAction::Touch);
	patchOrUpdate(adapter, obj, patch, EmptyFieldList());
}

data::Value Scheme::patchOrUpdate(Adapter *adapter, uint64_t id, data::Value & patch, const FieldVec &fields) const {
	if (!patch.empty()) {
		if (isAtomicPatch(patch)) {
			if (auto ret = adapter->patchObject(*this, id, patch, fields)) {
				touchParents(adapter, ret);
				return ret;
			}
		} else {
			if (auto obj = get(adapter, id, true)) {
				return updateObject(adapter, std::move(obj), patch);
			}
		}
	}
	return data::Value();
}

data::Value Scheme::patchOrUpdate(Adapter *adapter, const data::Value & obj, data::Value & patch, const FieldVec &fields) const {
	if (!patch.empty()) {
		if (isAtomicPatch(patch)) {
			if (auto ret = adapter->patchObject(*this, obj.getInteger("__oid"), patch, fields)) {
				touchParents(adapter, ret);
				return ret;
			}
		} else {
			return updateObject(adapter, data::Value(obj), patch);
		}
	}
	return data::Value();
}

bool Scheme::remove(Adapter *adapter, uint64_t oid) const {
	if (!parents.empty()) {
		return adapter->performInTransaction([&] {
			Query query;
			prepareGetQuery(query, oid, true);
			for (auto &it : parents) {
				if (it->backReference) {
					query.include(String(it->backReference->getName()));
				}
			}
			auto obj = reduceGetQuery(adapter->selectObjects(*this, query));
			touchParents(adapter, obj);
			return adapter->removeObject(*this, oid);
		});
	} else {
		return adapter->removeObject(*this, oid);
	}
}

data::Value Scheme::select(Adapter *a, const Query &q) const {
	return a->selectObjects(*this, q);
}

size_t Scheme::count(Adapter *a) const {
	return a->countObjects(*this, Query());
}
size_t Scheme::count(Adapter *a, const Query &q) const {
	return a->countObjects(*this, q);
}

data::Value Scheme::getProperty(Adapter *a, uint64_t oid, const StringView &s, std::initializer_list<StringView> fields) const {
	auto f = getField(s);
	if (f) {
		return getProperty(a, oid, *f, getFieldSet(*f, fields));
	}
	return data::Value();
}
data::Value Scheme::getProperty(Adapter *a, const data::Value &obj, const StringView &s, std::initializer_list<StringView> fields) const {
	auto f = getField(s);
	if (f) {
		return getProperty(a, obj, *f, getFieldSet(*f, fields));
	}
	return data::Value();
}

data::Value Scheme::getProperty(Adapter *a, uint64_t oid, const StringView &s, const Set<const Field *> &fields) const {
	auto f = getField(s);
	if (f) {
		return getProperty(a, oid, *f, fields);
	}
	return data::Value();
}
data::Value Scheme::getProperty(Adapter *a, const data::Value &obj, const StringView &s, const Set<const Field *> &fields) const {
	auto f = getField(s);
	if (f) {
		return getProperty(a, obj, *f, fields);
	}
	return data::Value();
}

data::Value Scheme::setProperty(Adapter *a, uint64_t oid, const StringView &s, data::Value &&v) const {
	auto f = getField(s);
	if (f) {
		return setProperty(a, oid, *f, std::move(v));
	}
	return data::Value();
}
data::Value Scheme::setProperty(Adapter *a, const data::Value &obj, const StringView &s, data::Value &&v) const {
	auto f = getField(s);
	if (f) {
		return setProperty(a, obj, *f, std::move(v));
	}
	return data::Value();
}
data::Value Scheme::setProperty(Adapter *a, uint64_t oid, const StringView &s, InputFile &file) const {
	auto f = getField(s);
	if (f) {
		return setProperty(a, oid, *f, file);
	}
	return data::Value();
}
data::Value Scheme::setProperty(Adapter *a, const data::Value &obj, const StringView &s, InputFile &file) const {
	return setProperty(a, obj.getInteger(s), s, file);
}

bool Scheme::clearProperty(Adapter *a, uint64_t oid, const StringView &s, data::Value && objs) const {
	if (auto f = getField(s)) {
		return clearProperty(a, oid, *f, move(objs));
	}
	return false;
}
bool Scheme::clearProperty(Adapter *a, const data::Value &obj, const StringView &s, data::Value && objs) const {
	if (auto f = getField(s)) {
		return clearProperty(a, obj, *f, move(objs));
	}
	return false;
}

data::Value Scheme::appendProperty(Adapter *a, uint64_t oid, const StringView &s, data::Value &&v) const {
	auto f = getField(s);
	if (f) {
		return appendProperty(a, oid, *f, std::move(v));
	}
	return data::Value();
}
data::Value Scheme::appendProperty(Adapter *a, const data::Value &obj, const StringView &s, data::Value &&v) const {
	auto f = getField(s);
	if (f) {
		return appendProperty(a, obj, *f, std::move(v));
	}
	return data::Value();
}

data::Value Scheme::getProperty(Adapter *a, uint64_t oid, const Field &f, std::initializer_list<StringView> fields) const {
	return getProperty(a, oid, f, getFieldSet(f, fields));
}
data::Value Scheme::getProperty(Adapter *a, const data::Value &obj, const Field &f, std::initializer_list<StringView> fields) const {
	return getProperty(a, obj, f, getFieldSet(f, fields));
}

data::Value Scheme::getProperty(Adapter *a, uint64_t oid, const Field &f, const Set<const Field *> &fields) const {
	return a->getProperty(*this, oid, f, fields);
}
data::Value Scheme::getProperty(Adapter *a, const data::Value &obj, const Field &f, const Set<const Field *> &fields) const {
	if (f.isSimpleLayout()) {
		return obj.getValue(f.getName());
	} else if (f.isFile() && fields.empty()) {
		return File::getData(a, obj.isInteger() ? obj.asInteger() : obj.getInteger(f.getName()));
	}
	return a->getProperty(*this, obj, f, fields);
}

data::Value Scheme::setProperty(Adapter *a, uint64_t oid, const Field &f, data::Value &&v) const {
	if (v.isNull()) {
		clearProperty(a, oid, f);
		return data::Value();
	}
	if (f.transform(*this, v)) {
		data::Value ret;
		a->performInTransaction([&] () -> bool {
			ret = a->setProperty(*this, oid, f, std::move(v));
			return !ret.isNull();
		});
		return ret;
	}
	return data::Value();
}
data::Value Scheme::setProperty(Adapter *a, const data::Value &obj, const Field &f, data::Value &&v) const {
	if (v.isNull()) {
		clearProperty(a, obj, f);
		return data::Value();
	}
	if (f.transform(*this, v)) {
		data::Value ret;
		a->performInTransaction([&] () -> bool {
			ret = a->setProperty(*this, obj, f, std::move(v));
			return !ret.isNull();
		});
		return ret;
	}
	return data::Value();
}
data::Value Scheme::setProperty(Adapter *a, uint64_t oid, const Field &f, InputFile &file) const {
	if (f.isFile()) {
		data::Value ret;
		a->performInTransaction([&] () -> bool {
			data::Value patch;
			transform(patch, TransformAction::Update);
			auto d = createFile(a, f, file);
			if (d.isInteger()) {
				patch.setValue(d, f.getName());
			} else {
				patch.setValue(std::move(d));
			}
			if (patchOrUpdate(a, oid, patch)) {
				// resolve files
				ret = File::getData(a, patch.getInteger(f.getName()));
				return true;
			} else {
				purgeFilePatch(a, patch);
				return false;
			}
		});
		return ret;
	}
	return data::Value();
}
data::Value Scheme::setProperty(Adapter *a, const data::Value &obj, const Field &f, InputFile &file) const {
	return setProperty(a, obj.getInteger("__oid"), f, file);
}

bool Scheme::clearProperty(Adapter *a, uint64_t oid, const Field &f, data::Value &&objs) const {
	if (!f.hasFlag(Flags::Required)) {
		return a->performInTransaction([&] () -> bool {
			return a->clearProperty(*this, oid, f, move(objs));
		});
	}
	return false;
}
bool Scheme::clearProperty(Adapter *a, const data::Value &obj, const Field &f, data::Value &&objs) const {
	if (!f.hasFlag(Flags::Required)) {
		return a->performInTransaction([&] () -> bool {
			return a->clearProperty(*this, obj, f, move(objs));
		});
	}
	return false;
}

data::Value Scheme::appendProperty(Adapter *a, uint64_t oid, const Field &f, data::Value &&v) const {
	if (f.getType() == Type::Array || (f.getType() == Type::Set && f.isReference())) {
		if (f.transform(*this, v)) {
			data::Value ret;
			a->performInTransaction([&] () -> bool {
				ret = a->appendProperty(*this, oid, f, std::move(v));
				return !ret.isNull();
			});
			return ret;
		}
	}
	return data::Value();
}
data::Value Scheme::appendProperty(Adapter *a, const data::Value &obj, const Field &f, data::Value &&v) const {
	if (f.getType() == Type::Array || (f.getType() == Type::Set && f.isReference())) {
		if (f.transform(*this, v)) {
			data::Value ret;
			a->performInTransaction([&] () -> bool {
				ret = a->appendProperty(*this, obj, f, std::move(v));
				return !ret.isNull();
			});
			return ret;
		}
	}
	return data::Value();
}

data::Value &Scheme::transform(data::Value &d, TransformAction a) const {
	// drop readonly and not existed fields
	auto &dict = d.asDict();
	auto it = dict.begin();
	while (it != dict.end()) {
		auto &fname = it->first;
		auto f_it = fields.find(fname);
		if (f_it == fields.end()
				// we can write into readonly field only in protected mode
				|| (f_it->second.hasFlag(Flags::ReadOnly) && a != TransformAction::ProtectedCreate && a != TransformAction::ProtectedUpdate)

				// we can drop files in all modes...
				|| (f_it->second.isFile() && !it->second.isNull()
						// but we can write files as ints only in protected mode
						&& ((a != TransformAction::ProtectedCreate && a != TransformAction::ProtectedUpdate) || !it->second.isInteger()))) {
			it = dict.erase(it);
		} else {
			it ++;
		}
	}

	// write defaults
	for (auto &it : fields) {
		auto &field = it.second;
		if (a == TransformAction::Create || a == TransformAction::ProtectedCreate) {
			if (field.hasFlag(Flags::AutoMTime)) {
				d.setInteger(Time::now().toMicroseconds(), it.first);
			} else if (field.hasFlag(Flags::AutoCTime)) {
				d.setInteger(Time::now().toMicroseconds(), it.first);
			} else if (field.hasDefault() && !d.hasValue(it.first)) {
				if (auto def = field.getDefault(d)) {
					d.setValue(move(def), it.first);
				}
			}
		} else if ((a == TransformAction::Update || a == TransformAction::ProtectedUpdate || a == TransformAction::Touch)
				&& field.hasFlag(Flags::AutoMTime) && (!d.empty() || a == TransformAction::Touch)) {
			d.setInteger(Time::now().toMicroseconds(), it.first);
		}
	}

	if (!d.empty()) {
		auto &dict = d.asDict();
		auto it = dict.begin();
		while (it != dict.end()) {
			auto &field = fields.at(it->first);
			if (it->second.isNull() && (a == TransformAction::Update || a == TransformAction::ProtectedUpdate || a == TransformAction::Touch)) {
				it ++;
			} else if (!field.transform(*this, it->second)) {
				it = dict.erase(it);
			} else {
				it ++;
			}
		}
	}

	return d;
}

data::Value Scheme::createFile(Adapter *adapter, const Field &field, InputFile &file) const {
	//check if content type is valid
	if (!File::validateFileField(field, file)) {
		return data::Value();
	}

	if (field.getType() == Type::File) {
		return File::createFile(adapter, field, file);
	} else if (field.getType() == Type::Image) {
		return File::createImage(adapter, field, file);
	}
	return data::Value();
}

data::Value Scheme::createFile(Adapter *adapter, const Field &field, const Bytes &data, const StringView &type) const {
	//check if content type is valid
	if (!File::validateFileField(field, type, data)) {
		return data::Value();
	}

	if (field.getType() == Type::File) {
		return File::createFile(adapter, type, data);
	} else if (field.getType() == Type::Image) {
		return File::createImage(adapter, field, type, data);
	}
	return data::Value();
}

// call after object is created, used for custom field initialization
data::Value Scheme::initField(Adapter *, Object *, const Field &, const data::Value &) {
	return data::Value::Null;
}

data::Value Scheme::removeField(Adapter *adapter, data::Value &obj, const Field &f, const data::Value &value) {
	if (f.isFile()) {
		auto scheme = Server(apr::pool::server()).getFileScheme();
		int64_t id = 0;
		if (value.isInteger()) {
			id = value.asInteger();
		} else if (value.isInteger("__oid")) {
			id = value.getInteger("__oid");
		}

		if (id) {
			if (adapter->removeObject(*scheme, id)) {
				return data::Value(id);
			}
		}
		return data::Value();
	}
	return data::Value(true);
}
void Scheme::finalizeField(Adapter *a, const Field &f, const data::Value &value) {
	if (f.isFile()) {
		File::removeFile(a, f, value);
	}
}

static size_t processExtraVarSize(const FieldExtra *s) {
	size_t ret = 256;
	for (auto it : s->fields) {
		auto t = it.second.getType();
		if (t == storage::Type::Text || t == storage::Type::Bytes) {
			auto f = static_cast<const storage::FieldText *>(it.second.getSlot());
			ret = std::max(f->maxLength, ret);
		} else if (t == Type::Extra) {
			auto f = static_cast<const storage::FieldExtra *>(it.second.getSlot());
			ret = std::max(processExtraVarSize(f), ret);
		}
	}
	return ret;
}

static size_t updateFieldLimits(const Map<String, Field> &vec) {
	size_t ret = 256 * vec.size();
	for (auto &it : vec) {
		auto t = it.second.getType();
		if (t == storage::Type::Text || t == storage::Type::Bytes) {
			auto f = static_cast<const storage::FieldText *>(it.second.getSlot());
			ret += f->maxLength;
		} else if (t == Type::Data || t == Type::Array) {
			ret += config::getMaxExtraFieldSize();
		} else if (t == Type::Extra) {
			auto f = static_cast<const storage::FieldExtra *>(it.second.getSlot());
			ret += updateFieldLimits(f->fields);
		} else {
			ret += 256;
		}
	}
	return ret;
}

void Scheme::updateLimits() {
	maxRequestSize = 256 * fields.size();
	for (auto &it : fields) {
		auto t = it.second.getType();
		if (t == storage::Type::File) {
			auto f = static_cast<const storage::FieldFile *>(it.second.getSlot());
			maxFileSize = std::max(f->maxSize, maxFileSize);
			maxRequestSize += f->maxSize + 256;
		} else if (t == storage::Type::Image) {
			auto f = static_cast<const storage::FieldImage *>(it.second.getSlot());
			maxFileSize = std::max(f->maxSize, maxFileSize);
			maxRequestSize += f->maxSize + 256;
		} else if (t == storage::Type::Text || t == storage::Type::Bytes) {
			auto f = static_cast<const storage::FieldText *>(it.second.getSlot());
			maxVarSize = std::max(f->maxLength, maxVarSize);
			maxRequestSize += f->maxLength;
		} else if (t == Type::Data || t == Type::Array) {
			maxRequestSize += config::getMaxExtraFieldSize();
		} else if (t == Type::Extra) {
			auto f = static_cast<const storage::FieldExtra *>(it.second.getSlot());
			maxRequestSize += updateFieldLimits(f->fields);
			maxVarSize = std::max(processExtraVarSize(f), maxVarSize);
		}
	}
}

bool Scheme::validateHint(uint64_t oid, const data::Value &hint) {
	if (!hint.isDictionary()) {
		return false;
	}
	auto hoid = hint.getInteger("__oid");
	if (hoid > 0 && (uint64_t)hoid == oid) {
		return validateHint(hint);
	}
	return false;
}

bool Scheme::validateHint(const String &alias, const data::Value &hint) {
	if (!hint.isDictionary()) {
		return false;
	}
	for (auto &it : fields) {
		if (it.second.getType() == Type::Text && it.second.getTransform() == storage::Transform::Alias) {
			if (hint.getString(it.first) == alias) {
				return validateHint(hint);
			}
		}
	}
	return false;
}

bool Scheme::validateHint(const data::Value &hint) {
	if (hint.size() > 1) {
		// all required fields should exists
		for (auto &it : fields) {
			if (it.second.hasFlag(Flags::Required)) {
				if (!hint.hasValue(it.first)) {
					return false;
				}
			}
		}

		// no fields other then in schemes fields
		for (auto &it : hint.asDict()) {
			if (it.first != "__oid" && fields.find(it.first) == fields.end()) {
				return false;
			}
		}

		return true;
	}
	return false;
}

data::Value Scheme::createFilePatch(Adapter *adapter, const data::Value &val) const {
	data::Value patch;
	for (auto &it : val.asDict()) {
		auto f = getField(it.first);
		if (f && (f->getType() == Type::File || (f->getType() == Type::Image && static_cast<const FieldImage *>(f->getSlot())->primary))) {
			if (it.second.isInteger() && it.second.getInteger() < 0) {
				auto file = InputFilter::getFileFromContext(it.second.getInteger());
				if (file && file->isOpen()) {
					auto d = createFile(adapter, *f, *file);
					if (d.isInteger()) {
						patch.setValue(d, f->getName());
					} else if (d.isDictionary()) {
						for (auto & it : d.asDict()) {
							patch.setValue(std::move(it.second), it.first);
						}
					}
				}
			} else if (it.second.isDictionary()) {
				if (it.second.isBytes("content") && it.second.isString("type")) {
					auto d = createFile(adapter, *f, it.second.getBytes("content"), it.second.getString("type"));
					if (d.isInteger()) {
						patch.setValue(d, f->getName());
					} else if (d.isDictionary()) {
						for (auto & it : d.asDict()) {
							patch.setValue(std::move(it.second), it.first);
						}
					}
				}
			}
		}
	}
	return patch;
}

void Scheme::purgeFilePatch(Adapter *adapter, const data::Value &patch) const {
	for (auto &it : patch.asDict()) {
		if (auto f = getField(it.first)) {
			File::purgeFile(adapter, *f, it.second);
		}
	}
}

void Scheme::initScheme() {
	// init non-linked object fields as StrongReferences
	for (auto &it : fields) {
		switch (it.second.getType()) {
		case Type::Object:
		case Type::Set:
			if (auto slot = it.second.getSlot<FieldObject>()) {
				if (slot->linkage == Linkage::Auto && slot->onRemove == RemovePolicy::Null && !slot->hasFlag(Flags::Reference)) {
					if (!getForeignLink(slot)) {
						// assume strong reference
						auto mutSlot = const_cast<FieldObject *>(slot);
						mutSlot->onRemove = RemovePolicy::StrongReference;
						mutSlot->flags |= Flags::Reference;
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

Set<const Field *> Scheme::getFieldSet(const Field &f, std::initializer_list<StringView> il) const {
	Set<const Field *> ret;
	auto target = f.getForeignScheme();
	for (auto &it : il) {
		ret.emplace(target->getField(it));
	}
	return ret;
}

void Scheme::addView(const Scheme *s, const Field *f) {
	views.emplace_back(new ViewScheme{s, f});
	auto viewScheme = views.back();
	if (auto view = static_cast<const FieldView *>(f->getSlot())) {
		bool linked = false;
		for (auto &it : view->requires) {
			auto fit = fields.find(it);
			if (fit != fields.end()) {
				if (fit->second.getType() == Type::Object && !view->linkage && !linked) {
					// try to autolink from required field
					auto nextSlot = static_cast<const FieldObject *>(fit->second.getSlot());
					if (nextSlot->scheme == s) {
						viewScheme->autoLink = &fit->second;
						linked = true;
					}
				}
				viewScheme->fields.emplace(&fit->second);
				forceInclude.emplace(&fit->second);
			} else {
				messages::error("Scheme", "Field for view not foumd", data::Value{
					pair("view", data::Value(toString(s->getName(), ".", f->getName()))),
					pair("field", data::Value(toString(getName(), ".", it)))
				});
			}
		}
		if (!view->linkage && !linked) {
			// try to autolink from other fields
			for (auto &it : fields) {
				auto &field = it.second;
				if (field.getType() == Type::Object) {
					auto nextSlot = static_cast<const FieldObject *>(field.getSlot());
					if (nextSlot->scheme == s) {
						viewScheme->autoLink = &field;
						viewScheme->fields.emplace(&field);
						forceInclude.emplace(&field);
						linked = true;
						break;
					}
				}
			}
		}
		if (!linked) {
			messages::error("Scheme", "Field to autolink view field", data::Value{
				pair("view", data::Value(toString(s->getName(), ".", f->getName()))),
			});
		}
	}
}

void Scheme::addParent(const Scheme *s, const Field *f) {
	parents.emplace_back(new ParentScheme(s, f));
	auto &p = parents.back();

	auto slot = static_cast<const FieldObject *>(f->getSlot());
	if (f->getType() == Type::Set) {
		auto link = s->getForeignLink(slot);
		if (link) {
			p->backReference = link;
			forceInclude.emplace(p->backReference);
		}
	}
}

void Scheme::updateView(Adapter *adapter, const data::Value & obj, const ViewScheme *scheme) const {
	auto view = static_cast<const FieldView *>(scheme->viewField->getSlot());
	if (!view->viewFn) {
		return;
	}

	auto objId = obj.getInteger("__oid");
	adapter->removeFromView(*view, scheme->scheme, objId);

	Vector<uint64_t> ids; ids.reserve(1);
	if (view->linkage) {
		ids = view->linkage(*scheme->scheme, *this, obj);
	} else if (scheme->autoLink) {
		if (auto id = obj.getInteger(scheme->autoLink->getName())) {
			ids.push_back(id);
		}
	}

	Vector<data::Value> val = view->viewFn(*this, obj);
	for (auto &it : val) {
		if (it.isBool() && it.asBool()) {
			it = data::Value(data::Value::Type::DICTIONARY);
		}

		if (it.isDictionary()) {
			// drop not existed fields
			auto &dict = it.asDict();
			auto d_it = dict.begin();
			while (d_it != dict.end()) {
				auto f_it = view->fields.find(d_it->first);
				if (f_it == view->fields.end()) {
					d_it = dict.erase(d_it);
				} else {
					d_it ++;
				}
			}

			// write defaults
			for (auto &f_it : view->fields) {
				auto &field = f_it.second;
				if (field.hasFlag(Flags::AutoMTime) || field.hasFlag(Flags::AutoCTime)) {
					it.setInteger(Time::now().toMicroseconds(), f_it.first);
				} else if (field.hasDefault() && !it.hasValue(f_it.first)) {
					if (auto def = field.getDefault(it)) {
						it.setValue(move(def), f_it.first);
					}
				}
			}

			// transform
			d_it = dict.begin();
			while (d_it != dict.end()) {
				auto &field = view->fields.at(d_it->first);
				if (d_it->second.isNull() || !field.isSimpleLayout()) {
					d_it ++;
				} else if (!field.transform(*this, d_it->second)) {
					d_it = dict.erase(d_it);
				} else {
					d_it ++;
				}
			}

			it.setInteger(objId, toString(getName(), "_id"));
		} else {
			it = nullptr;
		}
	}

	for (auto &id : ids) {
		for (auto &it : val) {
			if (it) {
				if (scheme->scheme) {
					it.setInteger(id, toString(scheme->scheme->getName(), "_id"));
				}

				adapter->addToView(*view, scheme->scheme, id, it);
			}
		}
	}
}

NS_SA_EXT_END(storage)
