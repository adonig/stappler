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

#ifndef COMMON_UTILS_STEMMER_SPSNOWBALLSTEMMER_H_
#define COMMON_UTILS_STEMMER_SPSNOWBALLSTEMMER_H_

#include "SPCharReader.h"

NS_SP_EXT_BEGIN(search)

struct SN_alloc;

struct StemmerData {
	using symbol = unsigned char;

	SN_alloc *alloc;
	int (*stem)(StemmerData *);

	symbol * p;
	int c; int l; int lb; int bra; int ket;
	symbol * * S;
	int * I;
	unsigned char * B;
};

class Stemmer : public memory::AllocPool {
public:
	enum Language : int {
		Unknown = 0,
		Arabic,
		Danish,
		Dutch,
		English,
		Finnish,
		French,
		German,
		Greek,
		Hungarian,
		Indonesian,
		Irish,
		Italian,
		Lithuanian,
		Nepali,
		Norwegian,
		Portuguese,
		Romanian,
		Russian,
		Spanish,
		Swedish,
		Tamil,
		Turkish
	};

	static StringView getLanguageString(Language);
	static Language parseLanguage(const StringView &);

	Stemmer();

	StringView stem(const StringView &, Language = Unknown);

protected:
	template <typename K, typename V>
	using Map = memory::PoolInterface::MapType<K, V>;

	struct Alloc {
		void *(*memalloc)( void *userData, unsigned int size );
		void (*memfree)( void *userData, void *ptr );
		memory::pool_t * userData;
	};

	Language detectWordLanguage(const StringView &) const;
	StemmerData *getStemmer(Language);

	Alloc _alloc;
	Map<Language, StemmerData> _stemmers;
};

NS_SP_EXT_END(search)

#endif /* COMMON_UTILS_STEMMER_SPSNOWBALLSTEMMER_H_ */
