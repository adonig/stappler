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

#ifndef COMMON_APR_SPAPRPOINTERITERATOR_H_
#define COMMON_APR_SPAPRPOINTERITERATOR_H_

#include "SPCore.h"

#if SPAPR
NS_APR_BEGIN

template<class Type, class Pointer, class Reference>
class pointer_iterator : public std::iterator<std::random_access_iterator_tag, Type, std::ptrdiff_t, Pointer, Reference> {
public:
	using size_type = size_t;
	using pointer = Pointer;
	using reference = Reference;
	using iterator = pointer_iterator<Type, Pointer, Reference>;
	using difference_type = std::ptrdiff_t;
	using value_type = typename std::remove_cv<Type>::type;

	pointer_iterator() : current(nullptr) {}
	pointer_iterator(const iterator&other) : current(other.current) {}
	explicit pointer_iterator(pointer p) : current(p) {}
	~pointer_iterator() {}

	iterator& operator=(const iterator&other) {current = other; return *this;}
	bool operator==(const iterator&other) const {return current == other.current;}
	bool operator!=(const iterator&other) const {return current != other.current;}
	bool operator<(const iterator&other) const {return current < other.current;}
	bool operator>(const iterator&other) const {return current > other.current;}
	bool operator<=(const iterator&other) const {return current <= other.current;}
	bool operator>=(const iterator&other) const {return current >= other.current;}

	iterator& operator++() {++current; return *this;}
	iterator& operator++(int) {++current; return *this;}
	iterator& operator--() {--current; return *this;}
	iterator& operator--(int) {--current; return *this;}
	iterator& operator+= (size_type n) { current += n; return *this; }
	iterator operator+(size_type n) const {return iterator(current + n);}
	iterator& operator-=(size_type n) {current -= n; return *this;}
	iterator operator-(size_type n) const {return iterator(current - n);}
	difference_type operator-(const iterator &other) const {return current - other.current;}

	reference operator*() const {return *current;}
	pointer operator->() const {return current;}
	reference operator[](size_type n) const {return *(current + n);}

	size_type operator-(pointer p) const {return current - p;}

	// const_iterator cast
	operator pointer_iterator<value_type, const value_type *, const value_type &> () const {
		return pointer_iterator<value_type, const value_type *, const value_type &>(current);
	}

	operator pointer () const { return current; }

	// friend iterator operator+(size_type, const iterator&); //optional

protected:
	pointer current;
};

NS_APR_END
#endif

#endif /* COMMON_APR_SPAPRPOINTERITERATOR_H_ */