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
#include "Resource.h"
#include "Request.h"
#include "StorageScheme.h"
#include "StorageFile.h"

#include "User.h"
#include "Session.h"

NS_SA_BEGIN

Resource::Resource(ResourceType t, Adapter *a, QueryList &&list) : _type(t), _adapter(a), _queries(move(list)) {
	if (_queries.isDeltaApplicable()) {
		_delta = Time::microseconds(_adapter->getDeltaValue(*_queries.getScheme()));
	}
}

ResourceType Resource::getType() const {
	return _type;
}

const storage::Scheme &Resource::getScheme() const { return *_queries.getScheme(); }
int Resource::getStatus() const { return _status; }

bool Resource::isDeltaApplicable() const {
	return _queries.isDeltaApplicable();
}

bool Resource::hasDelta() const {
	if (_queries.isDeltaApplicable()) {
		if (_queries.isView()) {
			return static_cast<const storage::FieldView *>(_queries.getItems().front().field->getSlot())->delta;
		} else {
			return _queries.getScheme()->hasDelta();
		}
	}
	return false;
}

void Resource::setQueryDelta(Time d) {
	_queries.setDelta(d);
}

Time Resource::getQueryDelta() const {
	return _queries.getDelta();
}

Time Resource::getSourceDelta() const { return _delta; }

void Resource::setTransform(const data::TransformMap *t) {
	_transform = t;
}
const data::TransformMap *Resource::getTransform() const {
	return _transform;
}

void Resource::resolveOptionForString(const String &str) {
	if (str.empty()) {
		return;
	}

	string::split(str, ",", [&] (const StringView &v) {
		StringView r(v);
		r.trimChars<StringView::CharGroup<CharGroupId::WhiteSpace>>();
		String token(toString('$', r));
		_resolve |= Query::decodeResolve(token);
		_extraResolves.emplace_back(move(token));
	});
}

void Resource::setAccessControl(const AccessControl *a) {
	_access = a;
}
void Resource::setUser(User *u) {
	_user = u;
}

void Resource::setFilterData(const data::Value &val) {
	_filterData = val;
}

void Resource::setResolveOptions(const data::Value & opts) {
	if (opts.isArray()) {
		for (auto &it : opts.asArray()) {
			if (it.isString()) {
				resolveOptionForString(it.getString());
			}
		}
	} else if (opts.isString()) {
		resolveOptionForString(opts.getString());
	}
}
void Resource::setResolveDepth(size_t size) {
	_queries.setResolveDepth(std::min(uint16_t(size), config::getResourceResolverMaxDepth()));
}

void Resource::setPageFrom(size_t value) {
	if (value > 0) {
		_queries.offset(_queries.getScheme(), value);
	}
}
void Resource::setPageCount(size_t value) {
	if (value > 0 && value != maxOf<size_t>()) {
		_queries.limit(_queries.getScheme(), value);
	}
}

void Resource::applyQuery(const data::Value &query) {
	_queries.apply(query);
}

void Resource::prepare() {
	_queries.resolve(_extraResolves);
}

bool Resource::prepareUpdate() { return false; }
bool Resource::prepareCreate() { return false; }
bool Resource::prepareAppend() { return false; }
bool Resource::removeObject() { return false; }
data::Value Resource::updateObject(data::Value &, apr::array<InputFile> &) { return data::Value(); }
data::Value Resource::createObject(data::Value &, apr::array<InputFile> &) { return data::Value(); }
data::Value Resource::appendObject(data::Value &) { return data::Value(); }
data::Value Resource::getResultObject() { return data::Value(); }
void Resource::resolve(const Scheme &, data::Value &) { }

size_t Resource::getMaxRequestSize() const {
	return getRequestScheme().getMaxRequestSize();
}
size_t Resource::getMaxVarSize() const {
	return getRequestScheme().getMaxVarSize();
}
size_t Resource::getMaxFileSize() const {
	return getRequestScheme().getMaxFileSize();
}

void Resource::encodeFiles(data::Value &data, apr::array<InputFile> &files) {
	for (auto &it : files) {
		const storage::Field * f = getRequestScheme().getField(it.name);
		if (f && f->isFile()) {
			if (!data.hasValue(it.name)) {
				data.setInteger(it.negativeId(), it.name);
			}
		}
	}
}

static bool Resource_isIdRequest(const storage::QueryFieldResolver &next, ResolveOptions opts, ResolveOptions target) {
	if (next.getResolves().empty()) {
		if (auto vec = next.getIncludeVec()) {
			if (vec->size() == 1 && vec->front().name == "$id") {
				return true;
			}
		}
	}

	if ((opts & ResolveOptions::Ids) != ResolveOptions::None) {
		if ((opts & target) == ResolveOptions::None) {
			return true;
		}
	}
	return false;
}

void Resource::resolveSet(const QueryFieldResolver &res, int64_t id, const storage::Field &field, data::Value &fobj) {
	QueryFieldResolver next(res.next(field.getName()));
	if (next) {
		auto perms = isSchemeAllowed(*(next.getScheme()), AccessControl::Read);
		if (perms != AccessControl::Restrict) {
			auto &fields = next.getResolves();
			bool idOnly = Resource_isIdRequest(next, ResolveOptions::None, ResolveOptions::Sets);

			auto objs = idOnly
					? res.getScheme()->getProperty(_adapter, fobj, field, Set<const Field *>{(const Field *)nullptr})
					: res.getScheme()->getProperty(_adapter, fobj, field, fields);
			if (objs.isArray()) {
				data::Value arr;
				for (auto &sit : objs.asArray()) {
					if (sit.isDictionary()) {
						if (perms == AccessControl::Full || isObjectAllowed(*(next.getScheme()), AccessControl::Read, sit)) {
							auto id = sit.getInteger("__oid");
							if (idOnly) {
								arr.addInteger(id);
							} else {
								if (_resolveObjects.insert(id).second == false) {
									sit.setInteger(id);
								}
								arr.addValue(std::move(sit));
							}
						}
					}
				}
				fobj = std::move(arr);
				return;
			}
		}
	}
	fobj = data::Value();
}

void Resource::resolveObject(const QueryFieldResolver &res, int64_t id, const storage::Field &field, data::Value &fobj) {
	QueryFieldResolver next(res.next(field.getName()));
	if (next && _resolveObjects.find(fobj.asInteger()) == _resolveObjects.end()) {
		auto perms = isSchemeAllowed(*(next.getScheme()), AccessControl::Read);
		if (perms != AccessControl::Restrict) {
			auto &fields = next.getResolves();
			if (!Resource_isIdRequest(next, _resolve, ResolveOptions::Objects)) {
				data::Value obj = res.getScheme()->getProperty(_adapter, fobj, field, fields);
				if (obj.isDictionary() && (perms == AccessControl::Full || isObjectAllowed(*(next.getScheme()), AccessControl::Read, obj))) {
					auto id = obj.getInteger("__oid");
					if (_resolveObjects.insert(id).second == false) {
						fobj.setInteger(id);
					} else {
						fobj.setValue(move(obj));
					}
					return;
				}
			} else {
				return;
			}
		}
		fobj.setNull();
	}
}

void Resource::resolveArray(const QueryFieldResolver &res, int64_t id, const storage::Field &field, data::Value &fobj) {
	fobj.setValue(res.getScheme()->getProperty(_adapter, fobj, field));
}

void Resource::resolveFile(const QueryFieldResolver &res, int64_t id, const storage::Field &field, data::Value &fobj) {
	QueryFieldResolver next(res.next(field.getName()));
	if (next) {
		auto perms = isSchemeAllowed(*(next.getScheme()), AccessControl::Read);
		if (perms != AccessControl::Restrict) {
			auto fields = next.getResolves();
			if (!Resource_isIdRequest(next, _resolve, ResolveOptions::Files)) {
				data::Value obj = res.getScheme()->getProperty(_adapter, fobj, field, fields);
				if (obj.isDictionary() && (perms == AccessControl::Full || isObjectAllowed(*(next.getScheme()), AccessControl::Read, obj))) {
					fobj.setValue(move(obj));
					return;
				}
			} else {
				return;
			}
		}
	}
	fobj.setNull();
}

static void Resource_resolveExtra(const storage::QueryFieldResolver &res, data::Value &obj) {
	auto &fields = res.getResolves();
	auto &fieldData = res.getResolvesData();
	auto &dict = obj.asDict();
	auto it = dict.begin();
	while (it != dict.end()) {
		auto f = res.getField(it->first);
		if (fieldData.find(it->first) != fieldData.end()) {
			storage::QueryFieldResolver next(res.next(it->first));
			if (next) {
				Resource_resolveExtra(next, it->second);
			}
			it ++;
		} else if (!f || f->isProtected() || (fields.find(f) == fields.end())) {
			it = dict.erase(it);
		} else if (f->getType() == storage::Type::Extra && it->second.isDictionary()) {
			storage::QueryFieldResolver next(res.next(it->first));
			if (next) {
				Resource_resolveExtra(next, it->second);
			}
			it ++;
		} else {
			it ++;
		}
	}
}

int64_t Resource::processResolveResult(const QueryFieldResolver &res, const Set<const Field *> &fields, data::Value &obj) {
	int64_t id = 0;
	auto &dict = obj.asDict();
	auto it = dict.begin();

	while (it != dict.end()) {
		if (it->first == "__oid") {
			id = it->second.asInteger();
			++ it;
			continue;
		} else if (it->first == "__delta") {
			if (res.getMeta() == QueryFieldResolver::Meta::None) {
				if (it->second.getString("action") == "delete") {
					it->second.setString("delete");
				} else {
					it = dict.erase(it);
					continue;
				}
			} else {
				auto meta = res.getMeta();
				if ((meta & QueryFieldResolver::Meta::Action) == QueryFieldResolver::Meta::None) {
					it->second.erase("action");
				} else if ((meta & QueryFieldResolver::Meta::Time) == QueryFieldResolver::Meta::None) {
					it->second.erase("time");
				}
			}
			++ it;
			continue;
		} else if (it->first == "__views") {
			auto meta = res.getMeta();
			if ((meta & QueryFieldResolver::Meta::View) == QueryFieldResolver::Meta::None) {
				it = dict.erase(it);
				continue;
			} else {
				++ it;
				continue;
			}
		}

		auto f = res.getField(it->first);
		if (!f || f->isProtected() || (fields.find(f) == fields.end())) {
			it = dict.erase(it);
		} else if ((f->getType() == storage::Type::Extra || f->getType() == storage::Type::Data) && it->second.isDictionary()) {
			QueryFieldResolver next(res.next(it->first));
			if (next) {
				Resource_resolveExtra(next, it->second);
			}
			it ++;
		} else {
			it ++;
		}
	}
	return id;
}

void Resource::resolveResult(const QueryFieldResolver &res, data::Value &obj, uint16_t depth, uint16_t max) {
	auto &searchField = res.getResolves();

	int64_t id = processResolveResult(res, searchField, obj);

	if (res && depth <= max) {
		auto & fields = *res.getFields();
		for (auto &it : fields) {
			const Field &f = it.second;
			auto type = f.getType();

			if (f.isSimpleLayout() || searchField.find(&f) == searchField.end()) {
				if (type == storage::Type::Bytes && f.getTransform() == storage::Transform::Uuid) {
					auto &fobj = obj.getValue(it.first);
					if (fobj.isBytes()) {
						fobj.setString(apr::uuid::getStringFromBytes(fobj.getBytes()));
					}
				}
				continue;
			}

			if (!obj.hasValue(it.first) && (type == storage::Type::Set || type == storage::Type::Array || type == storage::Type::View)) {
				obj.setInteger(id, it.first);
			}

			auto &fobj = obj.getValue(it.first);
			if (type == storage::Type::Object && fobj.isInteger()) {
				resolveObject(res, id, f, fobj);
			} else if ((type == storage::Type::Set || type == storage::Type::View) && fobj.isInteger()) {
				resolveSet(res, id, f, fobj);
			} else if (type == storage::Type::Array && fobj.isInteger()) {
				resolveArray(res, id, f, fobj);
			} else if ((type == storage::Type::File || type == storage::Type::Image) && fobj.isInteger()) {
				resolveFile(res, id, f, fobj);
			}
		}

		for (auto &it : fields) {
			auto &f = it.second;
			auto type = f.getType();

			if ((type == storage::Type::Object && obj.isDictionary(it.first))
					|| ((type == storage::Type::Set || type == storage::Type::View) && obj.isArray(it.first))) {
				QueryFieldResolver next(res.next(it.first));
				if (next) {
					if (type == storage::Type::Set || type == storage::Type::View) {
						auto &fobj = obj.getValue(it.first);
						for (auto &sit : fobj.asArray()) {
							if (sit.isDictionary()) {
								resolveResult(next, sit, depth + 1, max);
							}
						}
					} else {
						auto &fobj = obj.getValue(it.first);
						if (fobj.isDictionary()) {
							resolveResult(next, fobj, depth + 1, max);
						}
					}
				}
			} else if (f.isFile() && obj.isDictionary()) {
				auto &dict = obj.asDict();
				auto f_it = dict.find(it.first);
				if (f_it != dict.end() && f_it->second.isNull()) {
					dict.erase(f_it);
				}
			}
		}
	} else if (obj.isDictionary()) {
		auto &dict = obj.asDict();
		auto it = dict.begin();
		while (it != dict.end()) {
			auto f = res.getField(it->first);
			if (f && f->isFile()) {
				it = dict.erase(it);
			} else {
				++ it;
			}
		}
	}

	if (_transform) {
		auto it = _transform->find(res.getScheme()->getName());
		if (it != _transform->end()) {
			it->second.output.transform(obj);
		}
	}
}

void Resource::resolveResult(const QueryList &l, data::Value &obj) {
	resolveResult(l.getFields(), obj, 0, l.getResolveDepth());
}

AccessControl::Permission Resource::isSchemeAllowed(const Scheme &s, AccessControl::Action a) const {
	if (_access) {
		return _access->onScheme(_user, s, a);
	}
	return (_user &&_user->isAdmin())
			? AccessControl::Full
			: (a == AccessControl::Read
					? AccessControl::Full
					: AccessControl::Restrict);
}

bool Resource::isObjectAllowed(const Scheme &s, AccessControl::Action a, data::Value &obj) const {
	data::Value p;
	return isObjectAllowed(s, a, obj, p);
}
bool Resource::isObjectAllowed(const Scheme &s, AccessControl::Action a, data::Value &current, data::Value &patch) const {
	if (_access) {
		return _access->onObject(_user, s, a, current, patch);
	}
	return (_user &&_user->isAdmin())
			? true
			: (a == AccessControl::Read
					? true
					: false);
}

const storage::Scheme &Resource::getRequestScheme() const {
	return getScheme();
}

NS_SA_END
