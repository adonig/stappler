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

#ifndef COMMON_APR_SPAPRSTRING_H_
#define COMMON_APR_SPAPRSTRING_H_

#include "SPAprStorageMem.h"

#if SPAPR

#include "city.h"

NS_SP_EXT_BEGIN(apr)

struct char_pointer_test;
template<typename T> struct is_char_pointer {};
template<> struct is_char_pointer<const char *> { typedef char_pointer_test* Type; };
template<> struct is_char_pointer<char *> { typedef char_pointer_test* Type; };
template<> struct is_char_pointer<const char16_t *> { typedef char_pointer_test* Type; };
template<> struct is_char_pointer<char16_t *> { typedef char_pointer_test* Type; };
template<> struct is_char_pointer<const char32_t *> { typedef char_pointer_test* Type; };
template<> struct is_char_pointer<char32_t *> { typedef char_pointer_test* Type; };

template <typename CharType>
class basic_string {
public:
	using allocator_type = Allocator<CharType>;

	using pointer = CharType *;
	using const_pointer = const CharType *;
	using reference = CharType &;
	using const_reference = const CharType &;

	using traits_type = std::char_traits<CharType>;

	using size_type = size_t;
	using charT = CharType;
	using mem_type = storage_mem<CharType>;
	using self = basic_string<CharType>;

	using iterator = typename mem_type::iterator;
	using const_iterator = typename mem_type::const_iterator;
	using reverse_iterator = typename mem_type::reverse_iterator;
	using const_reverse_iterator = typename mem_type::const_reverse_iterator;

	static const size_type npos;

	basic_string() : _mem() { }
	explicit basic_string (const allocator_type& alloc) : _mem(alloc) { }

	basic_string (const self& str) : _mem(str._mem) { }
	basic_string (const self& str, const allocator_type& alloc) : _mem(str._mem, alloc) { }

	basic_string (const self& str, size_type pos, size_type len = npos, const allocator_type& alloc = allocator_type())
	: _mem(str._mem, pos, len, alloc) { }

	basic_string(const charT* s, const allocator_type& alloc = allocator_type())
	: _mem(s, (s?traits_type::length(s):0), alloc) { }

	basic_string (const charT* s, size_type n, const allocator_type& alloc = allocator_type())
	: _mem(s, n, alloc) { }

	basic_string (size_type n, charT c, const allocator_type& alloc = allocator_type()) : _mem(alloc) {
		_mem.fill(n, c);
	}

	template <class InputIterator>
	basic_string (InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type()) : _mem(alloc) {
		auto size = std::distance(first, last);
		_mem.reserve(size);
		for (auto it = first; it != last; it ++) {
			_mem.emplace_back_unsafe(*it);
		}
	}

	basic_string (InitializerList<charT> il, const allocator_type& alloc = allocator_type()) : _mem(alloc) {
		_mem.reserve(il.size());
		for (auto it = il.begin(); it != il.end(); it ++) {
			_mem.emplace_back_unsafe(*it);
		}
	}

	basic_string (basic_string&& str) noexcept : _mem(std::move(str._mem)) { }
	basic_string (basic_string&& str, const allocator_type& alloc) : _mem(std::move(str._mem), alloc) { }

	basic_string& operator=( const basic_string& str ) {
		_mem = str._mem;
		return *this;
	}

	basic_string& operator=( basic_string&& str ) {
		_mem = std::move(str._mem);
		return *this;
	}

	basic_string& operator=( const charT* s ) {
		_mem.assign(s, traits_type::length(s));
		return *this;
	}

	basic_string& operator=( charT ch ) {
		_mem.assign(&ch, 1);
		return *this;
	}

	basic_string& operator=( InitializerList<charT> ilist ) {
		_mem.clear();
		_mem.reserve(ilist.size());
		for (auto &it : ilist) {
			_mem.emplace_back_unsafe(it);
		}
		return *this;
	}

	allocator_type get_allocator() const { return _mem._allocator; }

	iterator begin() { return _mem.begin(); }
	iterator end() { return _mem.end(); }

	const_iterator begin() const { return _mem.begin(); }
	const_iterator end() const { return _mem.end(); }

	const_iterator cbegin() const { return _mem.cbegin(); }
	const_iterator cend() const { return _mem.cend(); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }

    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    const_reverse_iterator crbegin() const { return const_reverse_iterator(cend()); }
    const_reverse_iterator crend() const { return const_reverse_iterator(cbegin()); }

	size_type size() const { return _mem.size(); }
	size_type length() const { return _mem.size(); }
	size_type capacity() const { return _mem.capacity(); }

	void reserve (size_type n = 0) { _mem.reserve(n); }

	void resize (size_type n) { _mem.resize(n); }
	void resize (size_type n, charT c) { _mem.resize(n, c); }

	void clear() noexcept { _mem.clear(); }
	bool empty() const noexcept { return _mem.empty(); }

	void shrink_to_fit() { }  // There is NO SHRINK in APR

	reference operator[] (size_type pos) { return _mem.at(pos); }
	const_reference operator[] (size_type pos) const { return _mem.at(pos); }

	reference at (size_type pos) { return _mem.at(pos); }
	const_reference at (size_type pos) const { return _mem.at(pos); }

	charT& back() { return _mem.back(); }
	const charT& back() const { return _mem.back(); }

	charT& front() { return _mem.front(); }
	const charT& front() const { return _mem.front(); }

	basic_string& append (const basic_string& str) {
		_mem.insert_back(str._mem);
		return *this;
	}
	basic_string& append (const basic_string& str, size_type subpos, size_type sublen = npos) {
		_mem.insert_back(str._mem, subpos, sublen);
		return *this;
	}
	basic_string& append (const charT* s) {
		_mem.insert_back(s, traits_type::length(s));
		return *this;
	}
	basic_string& append (const charT* s, size_type n) {
		_mem.insert_back(s, n);
		return *this;
	}
	basic_string& append (size_type n, charT c) {
		_mem.resize(_mem.size() + n, c);
		return *this;
	}
	template <class InputIterator>
	basic_string& append (InputIterator first, InputIterator last) {
		auto diff = last - first;
		_mem.reserve(_mem.size() + diff, true);
		for (auto it = first; it != last; it ++) {
			_mem.emplace_back_unsafe(*it);
		}
		return *this;
	}
	basic_string& append (InitializerList<charT> il) {
		_mem.reserve(il.size() + _mem.size(), true);
		for (auto it = il.begin(); it != il.end(); it ++) {
			_mem.emplace_back_unsafe(*it);
		}
		return *this;
	}

	basic_string& operator+= (const basic_string& str) { return append(str); }
	basic_string& operator+= (const charT* s) { return append(s); }
	basic_string& operator+= (charT c) { _mem.emplace_back(c); return *this; }
	basic_string& operator+= (InitializerList<charT> il) { return append(il); }

	basic_string& assign (const basic_string& str) {
		_mem.assign(str._mem);
		return *this;
	}
	basic_string& assign (const basic_string& str, size_type subpos, size_type sublen = npos) {
		_mem.assign(str._mem, subpos, sublen);
		return *this;
	}
	basic_string& assign (const charT* s) {
		_mem.assign(s, traits_type::length(s));
		return *this;
	}
	basic_string& assign (const charT* s, size_type n) {
		_mem.assign(s, n);
		return *this;
	}
	basic_string& assign (size_type n, charT c) {
		_mem.fill(n, c);
		return *this;
	}

	template <class InputIterator>
	basic_string& assign (InputIterator first, InputIterator last) {
		_mem.clear();
		auto diff = last - first;
		_mem.reserve(diff);
		for (auto it = first; it != last; it ++) {
			_mem.emplace_back_unsafe(*it);
		}
		return *this;
	}

	basic_string& assign (InitializerList<charT> il) {
		_mem.clear();
		_mem.reserve(il.size());
		for (auto it = il.begin(); it != il.end(); it ++) {
			_mem.emplace_back_unsafe(*it);
		}
		return *this;
	}
	basic_string& assign (basic_string&& str) noexcept {
		_mem = std::move(str._mem);
		return *this;
	}

	void push_back(CharType c) {
		_mem.emplace_back(c);
	}

	void emplace_back(CharType c) {
		_mem.emplace_back(c);
	}

	void pop_back() {
		_mem.pop_back();
	}

	basic_string& insert (size_type pos, const basic_string& str) {
		_mem.insert(pos, str._mem);
		return *this;
	}
	basic_string& insert (size_type pos, const basic_string& str, size_type subpos, size_type sublen = npos) {
		_mem.insert(pos, str._mem, subpos, sublen);
		return *this;
	}

	basic_string& insert (size_type pos, const charT* s) {
		_mem.insert(pos, s, traits_type::length(s));
		return *this;
	}

	basic_string& insert (size_type pos, const charT* s, size_type n) {
		_mem.insert(pos, s, n);
		return *this;
	}

	basic_string& insert (size_type pos, size_type n, charT c) {
		_mem.insert(pos, n, c);
		return *this;
	}
	iterator insert (const_iterator p, size_type n, charT c) {
		return _mem.insert(p, n, c);
	}

	iterator insert (const_iterator p, charT c) {
		return _mem.insert(p, 1, c);
	}

	template< class InputIt >
	iterator insert( const_iterator pos, InputIt first, InputIt last ) {
		return _mem.insert(pos, first, last);
	}

	iterator insert( const_iterator pos, std::initializer_list<charT> init ) {
		return _mem.insert(pos, init.begin(), init.end());
	}

	basic_string& erase (size_type pos = 0, size_type len = npos) {
		_mem.erase(pos, len);
		return *this;
	}

	iterator erase (const_iterator p) {
		return _mem.erase(p);
	}

	iterator erase (const_iterator first, const_iterator last) {
		return _mem.erase(first, last);
	}

	basic_string& replace (size_type pos, size_type len, const basic_string& str) {
		_mem.replace(pos, len, str._mem);
		return *this;
	}
	basic_string& replace (const_iterator i1, const_iterator i2, const basic_string& str) {
		_mem.replace(i1 - _mem._ptr, i2 - i1, str._mem);
		return *this;
	}

	basic_string& replace (size_type pos, size_type len, const basic_string& str,
			size_type subpos, size_type sublen = npos) {
		_mem.replace(pos, len, str._mem, subpos, sublen);
		return *this;
	}

	basic_string& replace (size_type pos, size_type len, const charT* s) {
		_mem.replace(pos, len, s, traits_type::length(s));
		return *this;
	}
	basic_string& replace (const_iterator i1, const_iterator i2, const charT* s) {
		_mem.replace(i1 - _mem._ptr, i2 - i1, s, traits_type::length(s));
		return *this;
	}

	basic_string& replace (size_type pos, size_type len, const charT* s, size_type n) {
		_mem.replace(pos, len, s, n);
		return *this;
	}
	basic_string& replace (const_iterator i1, const_iterator i2, const charT* s, size_type n) {
		_mem.replace(i1 - _mem._ptr, i2 - i1, s, n);
		return *this;
	}

	basic_string& replace (size_type pos, size_type len, size_type n, charT c) {
		_mem.replace(pos, len, n, c);
		return *this;
	}
	basic_string& replace (const_iterator i1, const_iterator i2, size_type n, charT c) {
		_mem.replace(i1 - _mem._ptr, i2 - i1, n, c);
		return *this;
	}

	template< class InputIt >
	basic_string& replace( const_iterator first, const_iterator last, InputIt first2, InputIt last2 ) {
		_mem.replace(first, last, first2, last2);
		return *this;
	}

	basic_string& replace( const_iterator first, const_iterator last, InitializerList<charT> init ) {
		_mem.replace(first, last, init.begin(), init.end());
		return *this;
	}

	void swap (basic_string& str) {
		_mem.swap(str._mem);
	}

	const charT* c_str() const noexcept { return _mem.data(); }
	const charT* data() const noexcept { return _mem.data(); }
	charT* data() noexcept { return _mem.data(); }

	size_type copy (charT* s, size_type len, size_type pos = 0) const {
		len = MIN(len, _mem._used - pos);
		memcpy(s, _mem._ptr + pos, len * sizeof(charT));
		return len;
	}

	basic_string substr (size_type pos = 0, size_type len = npos) const {
		return basic_string(*this, pos, len);
	}


    size_type find(const charT* __s, size_type __pos, size_type __n) const;
    size_type find(charT __c, size_type __pos = 0) const;
    size_type rfind(const charT* __s, size_type __pos, size_type __n) const;
    size_type rfind(charT __c, size_type __pos = npos) const;
    size_type find_first_of(const charT* __s, size_type __pos, size_type __n) const;
    size_type find_last_of(const charT* __s, size_type __pos, size_type __n) const;
    size_type find_first_not_of(const charT* __s, size_type __pos, size_type __n) const;
    size_type find_first_not_of(charT __c, size_type __pos = 0) const;
    size_type find_last_not_of(const charT* __s, size_type __pos, size_type __n) const;
    size_type find_last_not_of(charT __c, size_type __pos = npos) const;

    size_type find(const basic_string& __str, size_type __pos = 0) const {
    	return this->find(__str.data(), __pos, __str.size());
    }

    size_type find(const charT* __s, size_type __pos = 0) const {
    	return this->find(__s, __pos, traits_type::length(__s));
    }

    size_type rfind(const basic_string& __str, size_type __pos = npos) const {
    	return this->rfind(__str.data(), __pos, __str.size());
    }

    size_type rfind(const charT* __s, size_type __pos = npos) const {
    	return this->rfind(__s, __pos, traits_type::length(__s));
    }

    size_type find_first_of(const basic_string& __str, size_type __pos = 0) const {
    	return this->find_first_of(__str.data(), __pos, __str.size());
    }

    size_type find_first_of(const charT* __s, size_type __pos = 0) const {
    	return this->find_first_of(__s, __pos, traits_type::length(__s));
    }

    size_type find_first_of(charT __c, size_type __pos = 0) const {
    	return this->find(__c, __pos);
    }

    size_type find_last_of(const basic_string& __str, size_type __pos = npos) const {
    	return this->find_last_of(__str.data(), __pos, __str.size());
    }

    size_type find_last_of(const charT* __s, size_type __pos = npos) const {
    	return this->find_last_of(__s, __pos, traits_type::length(__s));
    }

    size_type find_last_of(charT __c, size_type __pos = npos) const {
    	return this->rfind(__c, __pos);
    }

    size_type find_first_not_of(const basic_string& __str, size_type __pos = 0) const {
    	return this->find_first_not_of(__str.data(), __pos, __str.size());
    }

    size_type find_first_not_of(const charT* __s, size_type __pos = 0) const {
    	return this->find_first_not_of(__s, __pos, traits_type::length(__s));
    }

    size_type find_last_not_of(const basic_string& __str, size_type __pos = npos) const {
    	return this->find_last_not_of(__str.data(), __pos, __str.size());
    }

    size_type find_last_not_of(const charT* __s, size_type __pos = npos) const {
    	return this->find_last_not_of(__s, __pos, traits_type::length(__s));
    }

	int compare (const basic_string& str) const noexcept {
	    size_type lhs_sz = size();
	    size_type rhs_sz = str.size();
	    return compare(data(), lhs_sz, str.data(), rhs_sz);
	}

	int compare (size_type pos, size_type len, const basic_string& str) const {
	    size_type lhs_sz = min(size() - pos, len);
	    size_type rhs_sz = str.size();
	    return compare(data() + pos, lhs_sz, str.data(), rhs_sz);
	}
	int compare (size_type pos, size_type len, const basic_string& str,
	             size_type subpos, size_type sublen = npos) const {
	    size_type lhs_sz = min(size() - pos, len);
	    size_type rhs_sz = min(str.size() - subpos, sublen);
	    return compare(data() + pos, lhs_sz, str.data() + subpos, rhs_sz);
	}

	int compare (const charT* s) const {
	    size_type lhs_sz = size();
	    size_type rhs_sz = traits_type::length(s);
	    return compare(data(), lhs_sz, s, rhs_sz);
	}
	int compare (size_type pos, size_type len, const charT* s) const {
	    size_type lhs_sz = min(size() - pos, len);
	    size_type rhs_sz = traits_type::length(s);
	    return compare(data() + pos, lhs_sz, s, rhs_sz);
	}

	int compare (size_type pos, size_type len, const charT* s, size_type n) const {
	    size_type lhs_sz = min(size() - pos, len);
	    size_type rhs_sz = n;
	    return compare(data() + pos, lhs_sz, s, rhs_sz);
	}

public: /* APR extensions */

	/* Weak strings - strings, that do not own their memory
	 * It's behave like normal strings, and copy it's data into
	 * new allocated memory block on first mutate call
	 *
	 * Weak string usually used with const qualifier.
	 * Weak strings do not require any dynamic memory for creation
	 *
	 * Weak strings designed as bridge between APR C-like API (const char *)
	 * and Stappler/Serenity C++-like api (const String &)
	 */
	static const basic_string make_weak(const CharType *str, const allocator_type& alloc = allocator_type()) {
		basic_string ret(alloc);
		if (str) {
			ret.assign_weak(str, traits_type::length(str));
		}
		return ret;
	}

	static const basic_string make_weak(const CharType *str, size_type l, const allocator_type& alloc = allocator_type()) {
		basic_string ret(alloc);
		if (str) {
			ret.assign_weak(str, l);
		}
		return ret;
	}

	basic_string& assign_weak(const CharType *str, size_type l) {
		_mem.assign_weak(str, l);
		return *this;
	}

	basic_string& assign_weak(const CharType *str) {
		_mem.assign_weak(str, traits_type::length(str));
		return *this;
	}

	bool is_weak() const {
		return _mem.is_weak();
	}

	template<int N>
	basic_string(const charT (&s)[N], const allocator_type& alloc = allocator_type())
	: _mem(alloc) {
		_mem.assign_weak(s, N);
	}

	template<int N>
	basic_string(charT (&s)[N], const allocator_type& alloc = allocator_type())
	: _mem(alloc) {
		_mem.assign_weak(s, N);
	}


	/* Wrap function used to assign preallocated c-string to apr::string
	 * string will use provided memory for mutate operations, and return it to manager when destroyed
	 */

	static basic_string wrap(CharType *str, const allocator_type& alloc = allocator_type()) {
		basic_string ret(alloc);
		ret.assign_wrap(str, traits_type::length(str));
		return ret;
	}

	static basic_string wrap(CharType *str, size_type l, const allocator_type& alloc = allocator_type()) {
		basic_string ret(alloc);
		ret.assign_wrap(str, l);
		return ret;
	}

	static basic_string wrap(CharType *str, size_type l, size_type nalloc, const allocator_type& alloc = allocator_type()) {
		basic_string ret(alloc);
		ret.assign_wrap(str, l, nalloc);
		return ret;
	}

	basic_string& assign_wrap(CharType *str, size_type l) {
		_mem.assign_mem(str, l);
		return *this;
	}

	basic_string& assign_wrap(CharType *str, size_type l, size_type nalloc) {
		_mem.assign_mem(str, l, nalloc);
		return *this;
	}

	// Force-clear is a clear function, that prevent memory to be returned
	// to manager. It is used when string is moved into non-STL-like container
	// e.g. apr::table.
	//
	// After force_clear current memory block is invalidated, so, new block
	// will be allocated on next mutate call
	void force_clear() {
		_mem.force_clear();
	}

protected:
	static int compare(const charT* s1, size_type len1, const charT* s2, size_type len2) {
	    int result = traits_type::compare(s1, s2, min(len1, len2));
	    if (result != 0)
	        return result;
	    if (len1 < len2)
	        return -1;
	    if (len1 > len2)
	        return 1;
	    return 0;
	}

	storage_mem<CharType> _mem;
};

template< class CharT > basic_string<CharT>
operator+( const basic_string<CharT>& lhs, const basic_string<CharT>& rhs ) {
	basic_string<CharT> ret;
	ret.reserve(lhs.size() + rhs.size());
	ret.assign(lhs);
	ret.append(rhs);
	return ret;
}

template< class CharT > basic_string<CharT>
operator+( const CharT* lhs, const basic_string<CharT> & rhs ) {
	basic_string<CharT> ret;
	ret.reserve(std::char_traits<CharT>::length(lhs) + rhs.size());
	ret.assign(lhs);
	ret.append(rhs);
	return ret;
}

template< class CharT > basic_string<CharT>
operator+( CharT lhs, const basic_string<CharT>& rhs ) {
	basic_string<CharT> ret;
	ret.reserve(rhs.size() + 1);
	ret.assign(1, lhs);
	ret.append(rhs);
	return ret;
}

template< class CharT > basic_string<CharT>
operator+( const basic_string<CharT>& lhs, const CharT* rhs ) {
	basic_string<CharT> ret;
	ret.reserve(lhs.size() + std::char_traits<CharT>::length(rhs));
	ret.assign(lhs);
	ret.append(rhs);
	return ret;
}

template< class CharT > basic_string<CharT>
operator+( const basic_string<CharT>& lhs, CharT rhs ) {
	basic_string<CharT> ret;
	ret.reserve(rhs.size() + 1);
	ret.assign(lhs);
	ret.append(1, rhs);
	return ret;
}

template< class CharT > basic_string<CharT>
operator+( basic_string<CharT>&& lhs, const basic_string<CharT>& rhs ) {
	return std::move(lhs.append(rhs));
}

template< class CharT > basic_string<CharT>
operator+( const basic_string<CharT>& lhs, basic_string<CharT>&& rhs ) {
	return std::move(rhs.insert(0, lhs));
}

template< class CharT > basic_string<CharT>
operator+( basic_string<CharT>&& lhs, basic_string<CharT>&& rhs ) {
	return std::move(lhs.append(rhs));
}

template< class CharT > basic_string<CharT>
operator+(const CharT* lhs, basic_string<CharT>&& rhs ) {
	return std::move(rhs.insert(0, lhs));
}

template< class CharT > basic_string<CharT>
operator+( CharT lhs, basic_string<CharT>&& rhs ) {
	return std::move(rhs.insert(0, 1, lhs));
}

template< class CharT > basic_string<CharT>
operator+( basic_string<CharT>&& lhs, const CharT* rhs ) {
	return std::move(lhs.append(rhs));
}

template< class CharT > basic_string<CharT>
operator+( basic_string<CharT>&& lhs, CharT rhs ) {
	return std::move(lhs.append(1, rhs));
}

template< class CharT > bool operator== ( const basic_string<CharT>& lhs, const basic_string<CharT>& rhs ) {
	return lhs.compare(rhs) == 0;
}

template< class CharT > bool operator!= ( const basic_string<CharT>& lhs, const basic_string<CharT>& rhs ) {
	return lhs.compare(rhs) != 0;
}

template< class CharT > bool operator< ( const basic_string<CharT>& lhs, const basic_string<CharT>& rhs ) {
	return lhs.compare(rhs) < 0;
}

template< class CharT > bool operator<= ( const basic_string<CharT>& lhs, const basic_string<CharT>& rhs ) {
	return lhs.compare(rhs) <= 0;
}

template< class CharT > bool operator> ( const basic_string<CharT>& lhs, const basic_string<CharT>& rhs ) {
	return lhs.compare(rhs) > 0;
}

template< class CharT > bool operator>= ( const basic_string<CharT>& lhs, const basic_string<CharT>& rhs ) {
	return lhs.compare(rhs) >= 0;
}

template< class CharT > bool operator== ( const basic_string<CharT>& lhs, const CharT* rhs ) {
	return lhs.compare(rhs) == 0;
}

template< class CharT > bool operator!= ( const basic_string<CharT>& lhs, const CharT* rhs ) {
	return lhs.compare(rhs) != 0;
}

template< class CharT > bool operator<( const basic_string<CharT>& lhs,  const CharT* rhs ) {
	return lhs.compare(rhs) < 0;
}

template< class CharT > bool operator<= ( const basic_string<CharT>& lhs, const CharT* rhs ) {
	return lhs.compare(rhs) <= 0;
}

template< class CharT > bool operator>( const basic_string<CharT>& lhs, const CharT* rhs ) {
	return lhs.compare(rhs) > 0;
}

template< class CharT > bool operator>=( const basic_string<CharT>& lhs, const CharT* rhs ) {
	return lhs.compare(rhs) >= 0;
}


template< class CharT > bool operator== ( const CharT* lhs, const basic_string<CharT>& rhs ) {
	return rhs.compare(lhs) == 0;
}

template< class CharT > bool operator!= ( const CharT* lhs, const basic_string<CharT>& rhs ) {
	return rhs.compare(lhs) != 0;
}

template< class CharT > bool operator< ( const CharT* lhs, const basic_string<CharT>& rhs ) {
	return rhs.compare(lhs) >= 0;
}

template< class CharT > bool operator<= ( const CharT* lhs, const basic_string<CharT>& rhs ) {
	return rhs.compare(lhs) > 0;
}

template< class CharT > bool operator>( const CharT* lhs, const basic_string<CharT>& rhs ) {
	return rhs.compare(lhs) <= 0;
}

template< class CharT > bool operator>=( const CharT* lhs, const basic_string<CharT>& rhs ) {
	return rhs.compare(lhs) < 0;
}

template< class T >
void swap( basic_string<T> &lhs, basic_string<T> &rhs ) {
	lhs.swap(rhs);
}

template<typename _CharT> typename basic_string<_CharT>::size_type
basic_string<_CharT>:: find(const _CharT* __s, size_type __pos, size_type __n) const {
	const size_type __size = this->size();
	const _CharT* __data = _mem._ptr;

	if (__n == 0) {
		return __pos <= __size ? __pos : npos;
	} else if (__n <= __size) {
		for (; __pos <= __size - __n; ++__pos)
		if (traits_type::eq(__data[__pos], __s[0])
				&& traits_type::compare(__data + __pos + 1,
						__s + 1, __n - 1) == 0)
		return __pos;
	}
	return npos;
}

template<typename _CharT> typename basic_string<_CharT>::size_type
basic_string<_CharT>::find(_CharT __c, size_type __pos) const {
	size_type __ret = npos;
	const size_type __size = this->size();
	if (__pos < __size) {
		const _CharT* __data = _mem._ptr;
		const size_type __n = __size - __pos;
		const _CharT* __p = traits_type::find(__data + __pos, __n, __c);
		if (__p)
			__ret = __p - __data;
	}
	return __ret;
}

template<typename _CharT> typename basic_string<_CharT>::size_type
basic_string<_CharT>::rfind(const _CharT* __s, size_type __pos, size_type __n) const {
	const size_type __size = this->size();
	if (__n <= __size) {
		__pos = min(size_type(__size - __n), __pos);
		const _CharT* __data = _mem._ptr;
		do {
			if (traits_type::compare(__data + __pos, __s, __n) == 0)
				return __pos;
		}
		while (__pos-- > 0);
	}
	return npos;
}

template<typename _CharT> typename basic_string<_CharT>::size_type
basic_string<_CharT>::rfind(_CharT __c, size_type __pos) const {
	size_type __size = this->size();
	if (__size) {
		if (--__size > __pos)
			__size = __pos;
		for (++__size; __size-- > 0; )
			if (traits_type::eq(_mem._ptr[__size], __c))
		return __size;
	}
	return npos;
}

template<typename _CharT> typename basic_string<_CharT>::size_type
basic_string<_CharT>::find_first_of(const _CharT* __s, size_type __pos, size_type __n) const {
	for (; __n && __pos < this->size(); ++__pos) {
		const _CharT* __p = traits_type::find(__s, __n, _mem._ptr[__pos]);
		if (__p)
			return __pos;
	}
	return npos;
}

template<typename _CharT> typename basic_string<_CharT>::size_type
basic_string<_CharT>:: find_last_of(const _CharT* __s, size_type __pos, size_type __n) const {
	size_type __size = this->size();
	if (__size && __n) {
		if (--__size > __pos)
			__size = __pos;
		do {
			if (traits_type::find(__s, __n, _mem._ptr[__size]))
			return __size;
		} while (__size-- != 0);
	}
	return npos;
}

template<typename _CharT> typename basic_string<_CharT>::size_type
basic_string<_CharT>::find_first_not_of(const _CharT* __s, size_type __pos, size_type __n) const  {
	for (; __pos < this->size(); ++__pos)
		if (!traits_type::find(__s, __n, _mem._ptr[__pos]))
			return __pos;
	return npos;
}

template<typename _CharT> typename basic_string<_CharT>::size_type
basic_string<_CharT>::find_first_not_of(_CharT __c, size_type __pos) const {
	for (; __pos < this->size(); ++__pos)
		if (!traits_type::eq(_mem._ptr[__pos], __c))
			return __pos;
	return npos;
}

template<typename _CharT> typename basic_string<_CharT>::size_type
basic_string<_CharT>:: find_last_not_of(const _CharT* __s, size_type __pos, size_type __n) const {
	size_type __size = this->size();
	if (__size)	{
		if (--__size > __pos)
			__size = __pos;
		do {
			if (!traits_type::find(__s, __n, _mem._ptr[__size]))
			return __size;
		} while (__size--);
	}
	return npos;
}

template<typename _CharT> typename basic_string<_CharT>::size_type
basic_string<_CharT>::find_last_not_of(_CharT __c, size_type __pos) const {
	size_type __size = this->size();
	if (__size)	{
		if (--__size > __pos)
			__size = __pos;
		do {
			if (!traits_type::eq(_mem._ptr[__size], __c))
			return __size;
		} while (__size--);
	}
	return npos;
}

template<typename CharType> inline std::basic_ostream<CharType> &
operator << (std::basic_ostream<CharType> & os, const basic_string<CharType> & str) {
	return os.write(str.data(), str.size());
}

template<typename CharType> const typename basic_string<CharType>::size_type
    basic_string<CharType>::npos = maxOf<typename basic_string<CharType>::size_type>();

using string = basic_string<char>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;

using weak_string = const string;

NS_SP_EXT_END(apr)

inline stappler::apr::basic_string<char> operator"" _weak ( const char* str, size_t len) {
	stappler::apr::basic_string<char> ret;
	if (str) {
		ret.assign_weak(str, len);
	}
	return ret;
}

using namespace stappler;

namespace std {

template<>
struct hash<apr::basic_string<char>> {
	size_t operator() (const apr::basic_string<char> & s) const noexcept	{
		if (sizeof(size_t) == 8) {
			return CityHash64(s.data(), s.size());
		} else {
			return CityHash32(s.data(), s.size());
		}
	}
};

template<>
struct hash<apr::basic_string<char16_t>> {
	size_t operator() (const apr::basic_string<char16_t> & s) const noexcept	{
		if (sizeof(size_t) == 8) {
			return CityHash64((char *)s.data(), s.size() * sizeof(char16_t));
		} else {
			return CityHash32((char *)s.data(), s.size() * sizeof(char16_t));
		}
	}
};

}

#endif

#endif /* COMMON_APR_SPAPRSTRING_H_ */