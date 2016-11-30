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

#ifndef COMMON_APR_SPAPRVECTOR_H_
#define COMMON_APR_SPAPRVECTOR_H_

#include "SPAprStorageMem.h"

#if SPAPR
NS_SP_EXT_BEGIN(apr)

template <typename Type>
class vector : public AllocPool {
public:
	using allocator_type = Allocator<Type>;

	using pointer = Type *;
	using const_pointer = const Type *;
	using reference = Type &;
	using const_reference = const Type &;

	using size_type = size_t;
	using value_type = Type;
	using mem_type = storage_mem<Type>;
	using self = vector<Type>;

	using iterator = typename mem_type::iterator;
	using const_iterator = typename mem_type::const_iterator;
	using reverse_iterator = typename mem_type::reverse_iterator;
	using const_reverse_iterator = typename mem_type::const_reverse_iterator;

public:
	vector() {}
	explicit vector( const allocator_type& alloc ) : _mem(alloc) { }
	explicit vector( size_type count, const Type& value, const allocator_type& alloc = allocator_type()) : _mem(alloc) {
		_mem.fill(count, value);
	}
	explicit vector( size_type count, const allocator_type& alloc = allocator_type() ) : _mem(alloc) {
		_mem.fill(count);
	}

	template< class InputIt >
	vector( InputIt first, InputIt last, const allocator_type& alloc = allocator_type() ) : _mem(alloc) {
		auto size = std::distance(first, last);
		_mem.reserve(size);
		for (auto it = first; it != last; it ++) {
			_mem.emplace_back_unsafe(*it);
		}
	}

	vector( const vector& other ) : _mem(other._mem) { }
	vector( const vector& other, const allocator_type& alloc ) : _mem(other._mem, alloc) { }
	vector( vector&& other ) : _mem(std::move(other._mem)) { }
	vector( vector&& other, const allocator_type& alloc ) : _mem(std::move(other._mem), alloc) { }
	vector( InitializerList<Type> init, const allocator_type& alloc = allocator_type() ) : _mem(alloc) {
		_mem.reserve(init.size());
		for (auto & it : init) {
			_mem.emplace_back_unsafe(it);
		}
	}

	vector& operator=( const vector& other ) {
		_mem = other._mem;
		return *this;
	}
	vector& operator=( vector&& other ) {
		_mem = std::move(other._mem);
		return *this;
	}
	vector& operator=( InitializerList<Type> init ) {
		_mem.clear();
		_mem.reserve(init.size());
		for (auto & it : init) {
			_mem.emplace_back_unsafe(it);
		}
		return *this;
	}

	void assign( size_type count, const Type& value ) {
		_mem.fill(count, value);
	}

	template< class InputIt >
	void assign( InputIt first, InputIt last ) {
		auto size = std::distance(first, last);
		_mem.reserve(size);
		for (auto it = first; it != last; it ++) {
			_mem.emplace_back_unsafe(*it);
		}
	}

	void assign( InitializerList<Type> init ) {
		_mem.clear();
		_mem.reserve(init.size());
		for (auto & it : init) {
			_mem.emplace_back_unsafe(it);
		}
	}

	void assign_strong(Type *t, size_type s) {
		_mem.assign_strong(t, s);
	}

	allocator_type get_allocator() const { return _mem._allocator; }

	reference at( size_type pos ) { return _mem.at(pos); }
	const_reference at( size_type pos ) const { return _mem.at(pos); }

	reference operator[]( size_type pos ) { return _mem.at(pos); }
	const_reference operator[]( size_type pos ) const { return _mem.at(pos); }

	reference front() { return _mem.front(); }
	const_reference front() const { return _mem.front(); }

	reference back() { return _mem.back(); }
	const_reference back() const { return _mem.back(); }

	pointer data() { return _mem.data(); }
	const_pointer data() const { return _mem.data(); }

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
	size_type capacity() const { return _mem.capacity(); }
	bool empty() const { return _mem.empty(); }
	void reserve( size_type new_cap ) { _mem.reserve(new_cap); }
	void shrink_to_fit() { }

	void clear() { _mem.clear(); }

	template< class... Args >
	iterator emplace( const_iterator pos, Args&&... args ) {
		return _mem.emplace(pos, std::forward<Args>(args)...);
	}

	iterator insert( const_iterator pos, const Type& value ) {
		return emplace(pos, value);
	}
	iterator insert( const_iterator pos, Type&& value ) {
		return emplace(pos, std::move(value));
	}
	iterator insert( const_iterator pos, size_type count, const Type& value ) {
		return _mem.insert(pos, count, value);
	}

	template< class InputIt >
	iterator insert( const_iterator pos, InputIt first, InputIt last ) {
		return _mem.insert(pos, first, last);
	}

	iterator insert( const_iterator pos, InitializerList<Type> init ) {
		return _mem.insert(pos, init.begin(), init.end());
	}

	iterator erase( const_iterator pos ) {
		return _mem.erase(pos);
	}

	iterator erase( const_iterator first, const_iterator last ) {
		return _mem.erase(first, last);
	}

	template< class... Args >
	void emplace_back( Args&&... args ) {
		_mem.emplace_back(std::forward<Args>(args)...);
	}

	void push_back( const Type& value ) {
		emplace_back(value);
	}
	void push_back( Type&& value ) {
		emplace_back(std::move(value));
	}

	void pop_back() {
		_mem.pop_back();
	}

	void resize( size_type count ) {
		_mem.resize(count);
	}
	void resize( size_type count, const value_type& value ) {
		_mem.resize(count, value);
	}

	void swap( vector& other ) {
		_mem.swap(other._mem);
	}

public:
	static const vector make_weak(const Type *str, size_type l, const allocator_type& alloc = allocator_type()) {
		vector ret(alloc);
		if (str) {
			ret.assign_weak(str, l);
		}
		return ret;
	}

	vector& assign_weak(const Type *str, size_type l) {
		_mem.assign_weak(str, l);
		return *this;
	}

	bool is_weak() const {
		return _mem.is_weak();
	}

	void force_clear() {
		_mem.force_clear();
	}
protected:
	storage_mem<Type> _mem;
};

template<typename _Tp> inline bool
operator==(const vector<_Tp>& __x, const vector<_Tp>& __y) {
	return (__x.size() == __y.size() && std::equal(__x.begin(), __x.end(), __y.begin()));
}

template<typename _Tp> inline bool
operator<(const vector<_Tp>& __x, const vector<_Tp>& __y) {
	return std::lexicographical_compare(__x.begin(), __x.end(), __y.begin(), __y.end());
}

template<> inline bool
operator<(const vector<uint8_t>& x, const vector<uint8_t>& y) {
	auto len1 = x.size();
	auto len2 = y.size();
    int result = memcmp(x.data(), y.data(), min(len1, len2));
    if (result != 0)
        return result < 0;
    if (len1 < len2)
        return true;
    if (len1 > len2)
        return false;
    return false;
}

/// Based on operator==
template<typename _Tp> inline bool
operator!=(const vector<_Tp>& __x, const vector<_Tp>& __y) {
	return !(__x == __y);
}

/// Based on operator<
template<typename _Tp> inline bool
operator>(const vector<_Tp>& __x, const vector<_Tp>& __y) {
	return __y < __x;
}

/// Based on operator<
template<typename _Tp> inline bool
operator<=(const vector<_Tp>& __x, const vector<_Tp>& __y) {
	return !(__y < __x);
}

/// Based on operator<
template<typename _Tp> inline bool
operator>=(const vector<_Tp>& __x, const vector<_Tp>& __y) {
	return !(__x < __y);
}

/// See std::vector::swap().
template<typename _Tp> inline void
swap(vector<_Tp>& __x, vector<_Tp>& __y) {
	__x.swap(__y);
}

NS_SP_EXT_END(apr)
#endif

#endif /* COMMON_APR_SPAPRVECTOR_H_ */
