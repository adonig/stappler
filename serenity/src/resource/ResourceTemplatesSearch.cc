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
#include "SPCharGroup.h"

NS_SA_BEGIN

ResourceSearch::ResourceSearch(Adapter *a, QueryList &&q, const Field *prop)
: ResourceObject(a, move(q)), _field(prop) {
	_type = ResourceType::Search;
}

data::Value ResourceSearch::getResultObject() {
	auto slot = _field->getSlot<storage::FieldFullTextView>();
	if (auto &searchData = _queries.getExtraData().getValue("search")) {
		Vector<storage::FullTextData> q;
		if (slot->queryFn) {
			q = slot->queryFn(searchData);
		} else {
			q = parseQueryDefault(searchData);
		}

		if (!q.empty()) {
			_queries.setFullTextQuery(_field, move(q));
			auto ret = _adapter->performQueryList(_queries, _queries.size(), false, _field);
			if (!ret.isArray()) {
				return data::Value();
			}

			return processResultList(_queries, ret);
		}
	}
	return data::Value();
}

Vector<String> ResourceSearch::stemQuery(const Vector<storage::FullTextData> &query) {
	Vector<String> ret; ret.reserve(256 / sizeof(String)); // memory manager hack

	for (auto &it : query) {
		StringView r(it.buffer);
		r.split<StringView::CharGroup<CharGroupId::WhiteSpace>>([&] (StringView &iword) {
			StringViewUtf8 word(iword.data(), iword.size());
			word.trimUntil<StringViewUtf8::MatchCharGroup<CharGroupId::Cyrillic>, StringViewUtf8::MatchCharGroup<CharGroupId::Alphanumeric>>();
			if (word.size() > 3) {
				ret.emplace_back(_stemmer.stem(word, it.language).str());
			}
		});
	}

	return ret;
}

Vector<storage::FullTextData> ResourceSearch::parseQueryDefault(const data::Value &data) const {
	if (data.isString()) {
		StringViewUtf8 r(data.getString());
		r.skipUntil<StringViewUtf8::MatchCharGroup<CharGroupId::Latin>, StringViewUtf8::MatchCharGroup<CharGroupId::Cyrillic>>();
		if (r.is<StringViewUtf8::MatchCharGroup<CharGroupId::Latin>>()) {
			return Vector<storage::FullTextData>{storage::FullTextData{data.getString(), storage::FullTextData::Language::English}};
		} else if (r.is<StringViewUtf8::MatchCharGroup<CharGroupId::Cyrillic>>()) {
			return Vector<storage::FullTextData>{storage::FullTextData{data.getString(), storage::FullTextData::Language::Russian}};
		}
	}
	return Vector<storage::FullTextData>();
}

NS_SA_END
