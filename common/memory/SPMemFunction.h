/**
Copyright (c) 2017 Roman Katuntsev <sbkarr@stappler.org>

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

#ifndef COMMON_MEMORY_SPMEMFUNCTION_H_
#define COMMON_MEMORY_SPMEMFUNCTION_H_

#include "SPMemAlloc.h"

NS_SP_EXT_BEGIN(memory)

// Function - реализация std::function, использующая память из apr_pool_t
// some sources from https://github.com/prograholic/blog/blob/master/cxx_function/main.cpp

template <typename, typename, typename = void>
struct check_signature : std::false_type {};

template <typename Func, typename Ret, typename... Args>
struct check_signature<Func, Ret(Args...),
    typename std::enable_if<
        std::is_convertible<
            decltype(std::declval<Func>()(std::declval<Args>()...)),
            Ret
        >::value, void>::type>
    : std::true_type {};

template <typename UnusedType>
class function;

template <typename ReturnType, typename ... ArgumentTypes>
class function <ReturnType (ArgumentTypes ...)> : public AllocPool {
public:
	using signature_type = ReturnType (ArgumentTypes ...);
	using allocator_type = Allocator<void *>;

	function(const allocator_type &alloc = allocator_type()) noexcept : mAllocator(alloc), mInvoker(nullptr) { }

	function(nullptr_t, const allocator_type &alloc = allocator_type()) noexcept : mAllocator(alloc), mInvoker(nullptr) { }
	function &operator= (nullptr_t) noexcept { mInvoker = nullptr; return *this; }

	template <typename FunctionT>
	function(FunctionT && f, const allocator_type &alloc = allocator_type()) noexcept
		: mAllocator(alloc), mInvoker(
				new (alloc)free_function_holder<
						typename std::remove_cv<typename std::remove_reference<FunctionT>::type>::type
				>(std::forward<FunctionT>(f))) { }

	template <typename FunctionT>
	function & operator= (FunctionT f) noexcept {
		mInvoker = new (mAllocator) free_function_holder<
				typename std::remove_cv<typename std::remove_reference<FunctionT>::type>::type
		>(f);
		return *this;
	}

	template <typename FunctionType, typename ClassType>
	function(FunctionType ClassType::* f, const allocator_type &alloc = allocator_type()) noexcept
		:mAllocator(alloc),  mInvoker(new (alloc) member_function_holder<FunctionType, ArgumentTypes ...>(f)) { }

	template <typename FunctionType, typename ClassType>
	function & operator= (FunctionType ClassType::* f) noexcept {
		mInvoker = new (mAllocator) member_function_holder<FunctionType, ArgumentTypes ...>(f);
		return *this;
	}


	function(const function & other, const allocator_type &alloc = allocator_type()) noexcept
	: mAllocator(alloc), mInvoker(other.mInvoker?other.mInvoker->clone(alloc):nullptr) { }

	function & operator = (const function & other) noexcept {
		if (other.mInvoker) {
			mInvoker = other.mInvoker->clone(mAllocator);
		} else {
			mInvoker = nullptr;
		}
		return *this;
	}


	function(function && other, const allocator_type &alloc = allocator_type()) noexcept
	: mAllocator(alloc),
	  mInvoker((alloc==other.mAllocator) ? other.mInvoker : (other.mInvoker?other.mInvoker->clone(mAllocator):nullptr)) {
		other.mInvoker = nullptr;
	}
	function & operator = (function && other) noexcept {
		mInvoker = (mAllocator==other.mAllocator) ? other.mInvoker : (other.mInvoker?other.mInvoker->clone(mAllocator):nullptr);
		other.mInvoker = nullptr;
		return *this;
	}

	ReturnType operator () (ArgumentTypes ... args) const {
		return mInvoker->invoke(std::forward<ArgumentTypes>(args) ...);
	}

	inline operator bool () const noexcept { return mInvoker != nullptr; }

	const allocator_type &get_allocator() const { return mAllocator; }

private:
	class function_holder_base : public AllocPool {
	public:
		function_holder_base() noexcept { }
		virtual ~function_holder_base() noexcept { }

		virtual ReturnType invoke(ArgumentTypes && ... args) = 0;
		virtual function_holder_base * clone(const allocator_type &) = 0;

		function_holder_base(const function_holder_base & ) = delete;
		void operator = (const function_holder_base &)  = delete;
	};

	// Мы не используем умные указатели, поскольку память из pool_t не требует освобождения
	using invoker_t = function_holder_base *;

	template <typename FunctionT>
	class free_function_holder : public function_holder_base {
	public:
		free_function_holder(const FunctionT &func) noexcept : function_holder_base(), mFunction(func) { }
		free_function_holder(FunctionT &&func) noexcept : function_holder_base(), mFunction(std::move(func)) { }

		virtual ReturnType invoke(ArgumentTypes && ... args) override {
			return mFunction(std::forward<ArgumentTypes>(args)...);
		}

		virtual invoker_t clone(const allocator_type &alloc) override {
			return invoker_t(new (alloc) free_function_holder(mFunction));
		}

	private:
		FunctionT mFunction;
	};

	template <typename FunctionType, typename ClassType, typename ... RestArgumentTypes>
	class member_function_holder : public function_holder_base {
	public:
		using member_function_signature_t = FunctionType ClassType::*;

		member_function_holder(member_function_signature_t f) noexcept : mFunction(f) {}

		virtual ReturnType invoke(ClassType obj, RestArgumentTypes &&... restArgs) override {
			return (obj.*mFunction)(std::forward<RestArgumentTypes>(restArgs) ...);
		}

		virtual invoker_t clone(const allocator_type &alloc) override {
			return invoker_t(new (alloc) member_function_holder(mFunction));
		}

	private:
		member_function_signature_t mFunction;
	};

	allocator_type mAllocator;
	invoker_t mInvoker;
};

NS_SP_EXT_END(memory)

#endif /* COMMON_MEMORY_SPMEMFUNCTION_H_ */
