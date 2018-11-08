/**
Copyright (c) 2018 Roman Katuntsev <sbkarr@stappler.org>

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

#include "SPSnowballStemmer.h"

NS_SP_EXT_BEGIN(search)

struct SN_alloc {
	void *(*memalloc)( void *userData, unsigned int size );
	void (*memfree)( void *userData, void *ptr );
	void* userData;	// User data passed to the allocator functions.
};

struct stemmer_modules {
	Stemmer::Language name;
	StemmerData * (*create)(StemmerData *);
	void (*close)(StemmerData *, SN_alloc *);
	int (*stem)(StemmerData *);
};

SP_EXTERN_C struct stemmer_modules * sb_stemmer_get(Stemmer::Language lang);
SP_EXTERN_C const unsigned char * sb_stemmer_stem(StemmerData * z, const unsigned char * word, int size);

static void * staticPoolAlloc(void* userData, unsigned int size) {
	memory::pool_t *pool = (memory::pool_t *)userData;
	size_t s = size;
	auto mem = memory::pool::alloc(pool, s);
	memset(mem,0, s);
	return mem;
}

static void staticPoolFree(void * userData, void * ptr) { }

StringView Stemmer::getLanguageString(Language lang) {
	switch (lang) {
	case Unknown: return StringView(); break;
	case Arabic: return StringView("arabic"); break;
	case Danish: return StringView("danish"); break;
	case Dutch: return StringView("dutch"); break;
	case English: return StringView("english"); break;
	case Finnish: return StringView("finnish"); break;
	case French: return StringView("french"); break;
	case German: return StringView("german"); break;
	case Greek: return StringView("greek"); break;
	case Hungarian: return StringView("hungarian"); break;
	case Indonesian: return StringView("indonesian"); break;
	case Irish: return StringView("irish"); break;
	case Italian: return StringView("italian"); break;
	case Lithuanian: return StringView("lithuanian"); break;
	case Nepali: return StringView("nepali"); break;
	case Norwegian: return StringView("norwegian"); break;
	case Portuguese: return StringView("portuguese"); break;
	case Romanian: return StringView("romanian"); break;
	case Russian: return StringView("russian"); break;
	case Spanish: return StringView("spanish"); break;
	case Swedish: return StringView("swedish"); break;
	case Tamil: return StringView("tamil"); break;
	case Turkish: return StringView("turkish"); break;
	}
	return StringView();
}

Stemmer::Language Stemmer::parseLanguage(const StringView &lang) {
	if (lang == "arabic") { return Arabic; }
	else if (lang == "danish") { return Danish; }
	else if (lang == "dutch") { return Dutch; }
	else if (lang == "english") { return English; }
	else if (lang == "finnish") { return Finnish; }
	else if (lang == "french") { return French; }
	else if (lang == "german") { return German; }
	else if (lang == "greek") { return Greek; }
	else if (lang == "hungarian") { return Hungarian; }
	else if (lang == "indonesian") { return Indonesian; }
	else if (lang == "irish") { return Irish; }
	else if (lang == "italian") { return Italian; }
	else if (lang == "nepali") { return Nepali; }
	else if (lang == "norwegian") { return Norwegian; }
	else if (lang == "portuguese") { return Portuguese; }
	else if (lang == "romanian") { return Romanian; }
	else if (lang == "russian") { return Russian; }
	else if (lang == "spanish") { return Spanish; }
	else if (lang == "swedish") { return Swedish; }
	else if (lang == "tamil") { return Tamil; }
	else if (lang == "turkish") { return Turkish; }
	return Unknown;
}

Stemmer::Stemmer() : _alloc{&staticPoolAlloc, &staticPoolFree, memory::pool::acquire()} { }

StringView Stemmer::stem(const StringView &word, Language lang) {
	if (lang == Unknown) {
		lang = detectWordLanguage(word);
		if (lang == Unknown) {
			return StringView();
		}
	}

	if (auto stemmer = getStemmer(lang)) {
		auto w = sb_stemmer_stem(stemmer, (const unsigned char *)word.data(), int(word.size()));
		return StringView((const char *)w,  size_t(stemmer->l));
	}

	return StringView();
}

Stemmer::Language Stemmer::detectWordLanguage(const StringView &word) const {
	StringViewUtf8 r(word.data(), word.size());
	while (!r.empty()) {
		r.skipUntil<StringViewUtf8::MatchCharGroup<CharGroupId::Latin>,
			StringViewUtf8::MatchCharGroup<CharGroupId::Cyrillic>,
			StringViewUtf8::MatchCharGroup<CharGroupId::GreekBasic>>();
		if (r.is<StringViewUtf8::MatchCharGroup<CharGroupId::Latin>>()) {
			return English;
		} else if (r.is<StringViewUtf8::MatchCharGroup<CharGroupId::Cyrillic>>()) {
			return Russian;
		} else if (r.is<StringViewUtf8::MatchCharGroup<CharGroupId::GreekBasic>>()) {
			return Greek;
		}
	}
	return Unknown;
}

StemmerData *Stemmer::getStemmer(Language lang) {
	auto it = _stemmers.find(lang);
	if (it == _stemmers.end()) {
		auto mod = sb_stemmer_get(lang);

		auto it = _stemmers.emplace(lang, StemmerData{}).first;
		it->second.alloc = (SN_alloc *)&_alloc;
		if (auto env = mod->create(&it->second)) {
			env->stem = mod->stem;
			return env;
		} else {
			_stemmers.erase(it);
		}
		return nullptr;
	} else {
		return &it->second;
	}
}

NS_SP_EXT_END(search)
