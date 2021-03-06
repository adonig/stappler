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

#ifndef SERENITY_SRC_FILTER_INPUTFILE_H_
#define SERENITY_SRC_FILTER_INPUTFILE_H_

#include "Define.h"

NS_SA_BEGIN

struct InputFile : public AllocPool {
	String path;
	String name;
	String type;
	String encoding;
	String original;
	apr::file file;

	size_t writeSize;
	size_t headerSize;
	int64_t id;

	InputFile(String && name, String && type, String && enc, String && orig, size_t s, int64_t id);
	~InputFile();

	bool isOpen() const;

	size_t write(const char *, size_t);
	void close();

	bool save(const String &) const;

	int64_t negativeId() const { return - id - 1; }

	InputFile(const InputFile &) = delete;
	InputFile(InputFile &&) = delete;

	InputFile &operator=(const InputFile &) = delete;
	InputFile &operator=(InputFile &&) = delete;
};

NS_SA_END

#endif /* SERENITY_SRC_FILTER_INPUTFILE_H_ */
