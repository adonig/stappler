// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

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

#include "SPCore.h"
#include "SPAprAllocator.h"

#ifdef SPAPR
NS_SP_EXT_BEGIN(apr)

namespace pool {

static server_rec *getServerFromContext(pool_t *p, uint32_t tag, void *ptr) {
	switch (tag) {
	case uint32_t(Server): return (server_rec *)ptr; break;
	case uint32_t(Connection): return ((conn_rec *)ptr)->base_server; break;
	case uint32_t(Request): return ((request_rec *)ptr)->server; break;
	case uint32_t(Pool): return nullptr; break;
	}
	return nullptr;
}

server_rec *server() {
	server_rec *ret = nullptr;
	foreach_info(&ret, [] (void *ud, pool_t *p, uint32_t tag, void *data) -> bool {
		auto ptr = getServerFromContext(p, tag, data);
		if (ptr) {
			*((server_rec **)ud) = ptr;
			return false;
		}
		return true;
	});

	return ret;
}

request_rec *request() {
	request_rec *ret = nullptr;
	foreach_info(&ret, [] (void *ud, pool_t *p, uint32_t tag, void *data) -> bool {
		if (tag == uint32_t(Request)) {
			*((request_rec **)ud) = (request_rec *)data;
			return false;
		}

		return true;
	});

	return ret;
}

struct Pool_StoreHandle : AllocPool {
	void *pointer;
	Function<void()> callback;
};

static apr_status_t sa_request_store_custom_cleanup(void *ptr) {
	if (ptr) {
		auto ref = (Pool_StoreHandle *)ptr;
		if (ref->callback) {
			memory::pool::push(ref->callback.get_allocator());
			ref->callback();
			memory::pool::pop();
		}
	}
	return APR_SUCCESS;
}

void store(pool_t *pool, void *ptr, const String &key, Function<void()> &&cb) {
	memory::pool::push(pool);
	auto h = new (pool) Pool_StoreHandle();
	h->pointer = ptr;
	if (cb) {
		h->callback = std::move(cb);
	}
	memory::pool::pop();
	apr_pool_userdata_set(h, key.data(), h->callback ? sa_request_store_custom_cleanup : nullptr, pool);
}

}

NS_SP_EXT_END(apr)

#include "SPLog.h"

NS_SP_EXT_BEGIN(log)

static void __log2(const StringView &tag, const StringView &str) {
	auto log = apr::pool::info();
	switch (log.first) {
	case uint32_t(apr::pool::Info::Pool):
		ap_log_perror(APLOG_MARK, 0, 0, (apr_pool_t *)log.second, "%s: %.*s", tag.data(), int(str.size()), str.data());
		break;
	case uint32_t(apr::pool::Info::Server):
		ap_log_error(APLOG_MARK, 0, 0, (server_rec *)log.second, "%s: %.*s", tag.data(), int(str.size()), str.data());
		break;
	case uint32_t(apr::pool::Info::Connection):
		ap_log_cerror(APLOG_MARK, 0, 0, (conn_rec *)log.second, "%s: %.*s", tag.data(), int(str.size()), str.data());
		break;
	case uint32_t(apr::pool::Info::Request):
		ap_log_rerror(APLOG_MARK, 0, 0, (request_rec *)log.second, "%s: %.*s", tag.data(), int(str.size()), str.data());
		break;
	}
}

static void __log(const StringView &tag, CustomLog::Type t, CustomLog::VA &va) {
	if (t == CustomLog::Text) {
		if (!va.text.empty()) {
			__log2(tag, va.text);
		}
	} else {
		auto pool = apr::pool::acquire();
		auto str = apr_pvsprintf(pool, va.format.format, va.format.args);
		__log2(tag, str);
	}
}

static CustomLog AprLog(&__log);

NS_SP_EXT_END(log)
#endif
