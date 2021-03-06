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

#ifndef SERENITY_SRC_REQUEST_USER_H_
#define SERENITY_SRC_REQUEST_USER_H_

#include "StorageObject.h"

NS_SA_BEGIN

class User : public storage::Object {
public:
	static User *create(storage::Adapter *, const String &name, const String &password);
	static User *setup(storage::Adapter *, const String &name, const String &password);
	static User *create(storage::Adapter *, data::Value &&);

	static User *get(storage::Adapter *, const String &name, const String &password);
	static User *get(storage::Adapter *, const storage::Scheme &scheme, const String &name, const String &password);

	static User *get(storage::Adapter *, uint64_t oid);
	static User *get(storage::Adapter *, const storage::Scheme &scheme, uint64_t oid);

	User(data::Value &&, const storage::Scheme &);

	bool validatePassword(const String &passwd) const;
	void setPassword(const String &passwd);

	bool isAdmin() const;

	const String & getName() const;
};

NS_SA_END

#endif /* SERENITY_SRC_REQUEST_USER_H_ */
