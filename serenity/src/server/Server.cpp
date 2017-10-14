// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/**
Copyright (c) 2016-2017 Roman Katuntsev <sbkarr@stappler.org>

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
#include "Server.h"
#include "Request.h"
#include "RequestHandler.h"
#include "Root.h"

#include "SPFilesystem.h"

#include "ServerComponent.h"
#include "StorageField.h"
#include "StorageScheme.h"
#include "PGHandle.h"
#include "ResourceHandler.h"
#include "MultiResourceHandler.h"
#include "WebSocket.h"
#include "Tools.h"
#include "TemplateCache.h"

APLOG_USE_MODULE(serenity);

NS_SA_BEGIN

#define SA_SERVER_FILE_SCHEME_NAME "__files"
#define SA_SERVER_USER_SCHEME_NAME "__users"

using namespace storage;

struct Server::Config : public AllocPool {

	static Config *get(server_rec *server) {
		if (!server) { return nullptr; }
		auto cfg = (Config *)ap_get_module_config(server->module_config, &serenity_module);
		if (cfg) { return cfg; }

		return apr::pool::perform([&] () -> Config * {
			return new Config(server);
		}, server);
	}

	Config(server_rec *server) : serverNamespace("default") {
		ap_set_module_config(server->module_config, &serenity_module, this);
	}

	void onHandler(Server &serv, const String &name, const String &ifile, const String &symbol, const data::Value &handlerData) {
		String file;
		if (!sourceRoot.empty() && ifile.front() != '/') {
			file = filepath::absolute(filepath::merge(sourceRoot, ifile));
		} else {
			file = filepath::absolute(ifile);
		}

		if (!filesystem::exists(file)) {
			log::format("Server", "No module file: %s", file.c_str());
			return;
		}

		auto pool = apr::pool::acquire();

		/* load new dynamic object */
		apr_dso_handle_t *obj = NULL;
		apr_dso_handle_sym_t sym = NULL;
		apr_status_t err = apr_dso_load(&obj, file.c_str(), pool);
		if (err == APR_SUCCESS) {
			/* loading was successful, export main symbol */
			err = apr_dso_sym(&sym, obj, symbol.c_str());
			if (err == APR_SUCCESS) {
				auto h = ((ServerComponent::Symbol) sym)(serv, name, handlerData);
				if (h) {
					components.emplace(name, h);
				} else {
					log::format("Server", "DSO (%s) returns nullptr handler", name.c_str());
				}
			} else {
				/* fail to load symbol, terminating process */
				char buf[256] = {0};
				log::format("Server", "DSO (%s) error: %s", name.c_str(), apr_dso_error(obj, buf, 255));
				apr_dso_unload(obj);
				return;
			}
		} else {
			/* fail to load object, terminating process */
			char buf[256] = {0};
			log::format("Server", "Fail to load DSO (%s): %s", name.c_str(), apr_dso_error(obj, buf, 255));
			return;
		}
	}

	void initHandlers(Server &serv, data::Value &val) {
		for (auto &it : val.asArray()) {
			if (it.isDictionary()) {
				auto & name = it.getString("name");
				auto & file = it.getString("file");
				auto & symbol = it.getString("symbol");
				auto & handlerData = it.getValue("data");

				onHandler(serv, name, file, symbol, handlerData);
			}
		}
	}

	void initSession(data::Value &val) {
		sessionName = val.getString("name");
		sessionKey = val.getString("key");
		sessionMaxAge = val.getInteger("maxage");
		isSessionSecure = val.getBool("secure");
	}

	void setSessionParam(CharReaderBase &n, CharReaderBase &v) {
		if (n.is("name")) {
			sessionName = v.str();
		} else if (n.is("key")) {
			sessionKey = v.str();
		} else if (n.is("maxage")) {
			sessionMaxAge = v.readInteger();
		} else if (n.is("secure")) {
			if (v.is("true") || v.is("on") || v.is("On")) {
				isSessionSecure = true;
			} else if (v.is("false") || v.is("off") || v.is("Off")) {
				isSessionSecure = false;
			}
		}
	}

	void init(Server &serv) {
		schemes.emplace(userScheme.getName(), &userScheme);
		schemes.emplace(fileScheme.getName(), &fileScheme);

		if (data) {
			if (data.isArray("handlers")) {
				initHandlers(serv, data.getValue("handlers"));
			}

			if (data.isDictionary("session")) {
				initSession(data.getValue("session"));
			}
		}

		if (handlers.isArray()) {
			initHandlers(serv, handlers);
		}
	}

	void onChildInit(Server &serv) {
		childInit = true;
		for (auto &it : components) {
			currentComponent = it.second->getName();
			it.second->onChildInit(serv);
			currentComponent = String();
		}

		auto root = Root::getInstance();
		auto pool = getCurrentPool();
		auto db = root->dbdOpen(pool, serv);
		if (db) {
			pg::Handle h(pool, db);
			h.init(serv, schemes);
			root->dbdClose(serv, db);
		}
	}

	bool childInit = false;

	storage::Scheme userScheme = storage::Scheme(SA_SERVER_USER_SCHEME_NAME, {
		Field::Text("name", Transform::Alias, Flags::Required),
		Field::Password("password", PasswordSalt(config::getDefaultPasswordSalt()), Flags::Required | Flags::Protected),
		Field::Boolean("isAdmin", data::Value(false)),
		Field::Extra("data", Vector<Field>{
			Field::Text("email", Transform::Email),
			Field::Text("public"),
			Field::Text("desc"),
		}),
		Field::Text("email", Transform::Email, Flags::Unique),
	});

	storage::Scheme fileScheme = storage::Scheme(SA_SERVER_FILE_SCHEME_NAME, {
		Field::Text("location", Transform::Url),
		Field::Text("type", Flags::ReadOnly),
		Field::Integer("size", Flags::ReadOnly),
		Field::Integer("mtime", Flags::AutoMTime | Flags::ReadOnly),
		Field::Extra("image", Vector<Field>{
			Field::Integer("width"),
			Field::Integer("height"),
		})
	});

	data::Value handlers;
	String handlerFile;
	String sourceRoot;
	String serverNamespace;
	String currentComponent;
	Vector<Function<int(Request &)>> preRequest;
	Map<String, ServerComponent *> components;
	Map<String, RequestScheme> requests;
	Map<const storage::Scheme *, ResourceScheme> resources;
	Map<String, const storage::Scheme *> schemes;

	Map<String, websocket::Manager *> websockets;
	data::Value data;

	String sessionName = config::getDefaultSessionName();
	String sessionKey = config::getDefaultSessionKey();
	apr_time_t sessionMaxAge = 0;
	bool isSessionSecure = false;

	Time lastDatabaseCleanup;
	int64_t broadcastId = 0;
	tpl::Cache _templateCache;
};

void * Server::merge(void *base, void *add) {
	Config *baseCfg = (Config *)base;
	Config *addCfg = (Config *)add;

	if (!baseCfg->sourceRoot.empty()) {
		addCfg->sourceRoot = baseCfg->sourceRoot;
	}
	return addCfg;
}

Server::Server() : _server(nullptr), _config(nullptr) { }
Server::Server(server_rec *server) : _server(server), _config(Config::get(server)) { }

Server & Server::operator =(server_rec *server) {
	_server = server;
	_config = Config::get(_server);
	return *this;
}

Server::Server(Server &&other) : _server(other._server), _config(other._config) { }
Server & Server::operator =(Server &&other) {
	_server = other._server;
	_config = other._config;
	return *this;
}

Server::Server(const Server &other) : _server(other._server), _config(other._config) { }
Server & Server::operator =(const Server &other) {
	_server = other._server;
	_config = other._config;
	return *this;
}

void Server::onChildInit() {
	if (!_config->handlerFile.empty()) {
		_config->data = data::readFile(_config->handlerFile);
	}

	_config->init(*this);
	_config->onChildInit(*this);

	filesystem::mkdir(filepath::merge(getDocumentRoot(), "/uploads"));

	_config->currentComponent = "root";
	tools::registerTools(config::getServerToolsPrefix(), *this);
	_config->currentComponent = String();
}

void Server::setHandlerFile(const apr::string &file) {
	_config->handlerFile = file;
}
void Server::setSourceRoot(const apr::string &file) {
	_config->sourceRoot = file;
}
void Server::addHanderSource(const apr::string &str) {
	CharReaderBase r(str);
	r.skipChars<CharReaderBase::CharGroup<CharGroupId::WhiteSpace>>();

	CharReaderBase handlerParams;
	if (r.is('"')) {
		++ r;
		handlerParams = r.readUntil<CharReaderBase::Chars<'"'>>();
		if (r.is('"')) {
			++ r;
		}
	} else {
		handlerParams = r.readUntil<CharReaderBase::CharGroup<CharGroupId::WhiteSpace>>();
	}

	CharReaderBase name, file, symbol;
	name = handlerParams.readUntil<CharReaderBase::Chars<':'>>();
	++ handlerParams;
	file = handlerParams.readUntil<CharReaderBase::Chars<':'>>();
	++ handlerParams;
	symbol = handlerParams;

	if (!name.empty() && !file.empty() && !symbol.empty()) {
		data::Value h;
		h.setString(name.str(), "name");
		h.setString(file.str(), "file");
		h.setString(symbol.str(), "symbol");
		data::Value &data = h.emplace("data");

		while (!r.empty()) {
			r.skipChars<CharReaderBase::CharGroup<CharGroupId::WhiteSpace>>();
			CharReaderBase params, n, v;
			if (r.is('"')) {
				++ r;
				params = r.readUntil<CharReaderBase::Chars<'"'>>();
				if (r.is('"')) {
					++ r;
				}
			} else {
				params = r.readUntil<CharReaderBase::CharGroup<CharGroupId::WhiteSpace>>();
			}

			if (!params.empty()) {
				n = params.readUntil<CharReaderBase::Chars<'='>>();
				++ params;
				v = params;

				if (!n.empty() && ! v.empty()) {
					data.setString(v.str(), n.str());
				}
			}
		}

		_config->handlers.addValue(std::move(h));
	}
}

void Server::setSessionParams(const apr::string &str) {
	CharReaderBase r(str);
	r.skipChars<CharReaderBase::CharGroup<CharGroupId::WhiteSpace>>();
	while (!r.empty()) {
		CharReaderBase params, n, v;
		if (r.is('"')) {
			++ r;
			params = r.readUntil<CharReaderBase::Chars<'"'>>();
			if (r.is('"')) {
				++ r;
			}
		} else {
			params = r.readUntil<CharReaderBase::CharGroup<CharGroupId::WhiteSpace>>();
		}

		if (!params.empty()) {
			n = params.readUntil<CharReaderBase::Chars<'='>>();
			++ params;
			v = params;

			if (!n.empty() && ! v.empty()) {
				_config->setSessionParam(n, v);
			}
		}

		r.skipChars<CharReaderBase::CharGroup<CharGroupId::WhiteSpace>>();
	}
}

const apr::string &Server::getHandlerFile() const {
	return _config->handlerFile;
}

apr::weak_string Server::getDefaultName() const {
	return apr::string::make_weak(_server->defn_name, _server->process->pconf);
}

bool Server::isVirtual() const {
	return _server->is_virtual;
}

apr_port_t Server::getServerPort() const {
	return _server->port;
}
apr::weak_string Server::getServerScheme() const {
	return apr::string::make_weak(_server->server_scheme, _server->process->pconf);
}
apr::weak_string Server::getServerAdmin() const {
	return apr::string::make_weak(_server->server_admin, _server->process->pconf);
}
apr::weak_string Server::getServerHostname() const {
	return apr::string::make_weak(_server->server_hostname, _server->process->pconf);
}
apr::weak_string Server::getDocumentRoot() const {
	core_server_config *sconf = (core_server_config *)ap_get_core_module_config(_server->module_config);
    return apr::string::make_weak(sconf->ap_document_root, _server->process->pconf);
}

apr_interval_time_t Server::getTimeout() const {
	return _server->timeout;
}
apr_interval_time_t Server::getKeepAliveTimeout() const {
	return _server->keep_alive_timeout;
}
int Server::getMaxKeepAlives() const {
	return _server->keep_alive_max;
}
bool Server::isUsingKeepAlive() const {
	return _server->keep_alive;
}

apr::weak_string Server::getServerPath() const {
	return apr::string::make_weak(_server->path, _server->pathlen, _server->process->pconf);
}

int Server::getMaxRequestLineSize() const {
	return _server->limit_req_line;
}
int Server::getMaxHeaderSize() const {
	return _server->limit_req_fieldsize;
}
int Server::getMaxHeaders() const {
	return _server->limit_req_fields;
}

const apr::string &Server::getSessionKey() const {
	return _config->sessionKey;
}
const apr::string &Server::getSessionName() const {
	return _config->sessionName;
}
apr_time_t Server::getSessionMaxAge() const {
	return _config->sessionMaxAge;
}
bool Server::isSessionSecure() const {
	return _config->isSessionSecure;
}

tpl::Cache *Server::getTemplateCache() const {
	return &_config->_templateCache;
}

const apr::string &Server::getNamespace() const {
	return _config->serverNamespace;
}

template <typename T>
auto Server_resolvePath(Map<String, T> &map, const String &path) -> typename Map<String, T>::iterator {
	auto it = map.begin();
	auto ret = map.end();
	for (; it != map.end(); it ++) {
		auto &p = it->first;
		if (p.size() - 1 <= path.size()) {
			if (p.back() == '/') {
				if (p.size() == 1 || (path.compare(0, p.size() - 1, p, 0, p.size() - 1) == 0
						&& (path.size() == p.size() - 1 || path.at(p.size() - 1) == '/' ))) {
					if (ret == map.end() || ret->first.size() < p.size()) {
						ret = it;
					}
				}
			} else if (p == path) {
				ret = it;
				break;
			}
		}
	}
	return ret;
}

void Server::onHeartBeat() {
	apr::pool::perform([&] {
		auto now = Time::now();
		auto root = Root::getInstance();
		auto pool = apr::pool::acquire();
		if (auto dbd = root->dbdOpen(pool, _server)) {
			pg::Handle h(pool, dbd);
			if (now - _config->lastDatabaseCleanup > TimeInterval::seconds(60)) {
				_config->lastDatabaseCleanup = now;
				h.makeSessionsCleanup();
			}
			_config->broadcastId = h.processBroadcasts(*this, _config->broadcastId);
			root->dbdClose(_server, dbd);
		}
		_config->_templateCache.update();
	});
}

void Server::onBroadcast(const data::Value &val) {
	if (val.getBool("system")) {
		Root::getInstance()->onBroadcast(val);
		return;
	}

	if (!val.hasValue("data")) {
		return;
	}

	if (val.getBool("message")) {
		String url = String(config::getServerToolsPrefix()) + config::getServerToolsShell();
		auto it = Server_resolvePath(_config->websockets, url);
		if (it != _config->websockets.end() && it->second) {
			it->second->receiveBroadcast(val);
		}
	}

	auto &url = val.getString("url");
	if (!url.empty()) {
		auto it = Server_resolvePath(_config->websockets, url);
		if (it != _config->websockets.end() && it->second) {
			it->second->receiveBroadcast(val.getValue("data"));
		}
	}
}

void Server::onBroadcast(const Bytes &bytes) {
	onBroadcast(data::read(bytes));
}

int Server::onRequest(Request &req) {
	auto &path = req.getUri();

	// Websocket handshake
	auto h = req.getRequestHeaders();
	auto connection = string::tolower(h.at("connection"));
	auto & upgrade = h.at("upgrade");
	if (connection.find("upgrade") != String::npos && upgrade == "websocket") {
		// try websocket
		auto it = Server_resolvePath(_config->websockets, path);
		if (it != _config->websockets.end() && it->second) {
			return it->second->accept(req);
		}
		return HTTP_NOT_FOUND;
	}

	for (auto &it : _config->preRequest) {
		auto ret = it(req);
		if (ret == DONE || ret > 0) {
			return ret;
		}
	}

	auto ret = Server_resolvePath(_config->requests, path);
	if (ret != _config->requests.end() && ret->second.callback) {
		RequestHandler *h = ret->second.callback();
		if (h) {
			String subPath((ret->first.back() == '/')?path.substr(ret->first.size() - 1):"");
			String originPath = subPath.size() == 0 ? String(path) : String(ret->first);
			if (originPath.back() == '/' && !subPath.empty()) {
				originPath.pop_back();
			}
			// preflight request (for CORS implementation)
			int preflight = h->onRequestRecieved(req, std::move(originPath), std::move(subPath), ret->second.data);
			if (preflight > 0 || preflight == DONE) { // cors error or successful preflight
				ap_send_interim_response(req.request(), 1);
				return preflight;
			}
			req.setRequestHandler(h);
		}
	}

	auto &data = req.getParsedQueryArgs();
	auto userIp = req.getUseragentIp();
	if (data.hasValue("basic_auth")) {
		if (req.isSecureConnection() || strncmp(userIp.c_str(), "127.", 4) == 0 || userIp == "::1") {
			if (req.getAuthorizedUser()) {
				return req.redirectTo(String(req.getUri()));
			}
			return HTTP_UNAUTHORIZED;
		}
	}

	return OK;
}

ServerComponent *Server::getComponent(const String &name) const {
	auto it = _config->components.find(name);
	if (it != _config->components.end()) {
		return it->second;
	}
	return nullptr;
}

void Server::addComponent(const String &name, ServerComponent *comp) {
	_config->components.emplace(name, comp);
	if (_config->childInit) {
		comp->onChildInit(*this);
	}
}

void Server::addPreRequest(Function<int(Request &)> &&req) {
	_config->preRequest.emplace_back(std::move(req));
}

void Server::addHandler(const String &path, const HandlerCallback &cb, const data::Value &d) {
	if (!path.empty() && path.front() == '/') {
		_config->requests.emplace(path, RequestScheme{_config->currentComponent, cb, d});
	}
}
void Server::addResourceHandler(const String &path, const storage::Scheme &scheme,
		const data::TransformMap *transform, const AccessControl *a, size_t priority) {
	if (!path.empty() && path.front() == '/') {
		_config->requests.emplace(path, RequestScheme{_config->currentComponent,
			[s = &scheme, transform, a] () -> RequestHandler * { return new ResourceHandler(*s, transform, a, data::Value()); },
			data::Value(), &scheme});
	}
	auto it = _config->resources.find(&scheme);
	if (it == _config->resources.end()) {
		_config->resources.emplace(&scheme, ResourceScheme{priority, path, data::Value()});
	} else {
		if (it->second.priority <= priority) {
			it->second.path = path;
			it->second.priority = priority;
		}
	}
}

void Server::addResourceHandler(const String &path, const storage::Scheme &scheme, const data::Value &val,
		const data::TransformMap *transform, const AccessControl *a, size_t priority) {
	if (!path.empty() && path.front() == '/') {
		_config->requests.emplace(path, RequestScheme{_config->currentComponent,
			[s = &scheme, transform, a, val] () -> RequestHandler * { return new ResourceHandler(*s, transform, a, val); },
			data::Value(), &scheme});
	}
	auto it = _config->resources.find(&scheme);
	if (it == _config->resources.end()) {
		_config->resources.emplace(&scheme, ResourceScheme{priority, path, val});
	} else {
		if (it->second.priority <= priority) {
			it->second.path = path;
			it->second.priority = priority;
		}
	}
}

void Server::addMultiResourceHandler(const String &path, std::initializer_list<Pair<const String, const storage::Scheme *>> &&schemes,
		const data::TransformMap *transform, const AccessControl *a) {
	if (!path.empty() && path.front() == '/') {
		_config->requests.emplace(path, RequestScheme{_config->currentComponent,
			[s = Map<String, const storage::Scheme *>(move(schemes)), transform, a] () -> RequestHandler * {
				return new MultiResourceHandler(s, transform, a);
			}, data::Value()});
	}
}

void Server::addHandler(std::initializer_list<String> paths, const HandlerCallback &cb, const data::Value &d) {
	for (auto &it : paths) {
		if (!it.empty() && it.front() == '/') {
			_config->requests.emplace(std::move(const_cast<String &>(it)), RequestScheme{_config->currentComponent, cb, d});
		}
	}
}

void Server::addWebsocket(const String &str, websocket::Manager *m) {
	_config->websockets.emplace(str, m);
}

const storage::Scheme * Server::exportScheme(const storage::Scheme &scheme) {
	_config->schemes.emplace(scheme.getName(), &scheme);
	return &scheme;
}

const storage::Scheme * Server::getScheme(const String &name) const {
	auto it = _config->schemes.find(name);
	if (it != _config->schemes.end()) {
		return it->second;
	}
	return nullptr;
}

const storage::Scheme * Server::getFileScheme() const {
	return getScheme(SA_SERVER_FILE_SCHEME_NAME);
}

const storage::Scheme * Server::getUserScheme() const {
	return getScheme(SA_SERVER_USER_SCHEME_NAME);
}

const storage::Scheme * Server::defineUserScheme(std::initializer_list<storage::Field> il) {
	_config->userScheme.define(il);
	return &_config->userScheme;
}

String Server::getResourcePath(const storage::Scheme &scheme) const {
	auto it = _config->resources.find(&scheme);
	if (it != _config->resources.end()) {
		return it->second.path;
	}
	return String();
}

const Map<String, const storage::Scheme *> &Server::getSchemes() const {
	return _config->schemes;
}
const Map<const storage::Scheme *, Server::ResourceScheme> &Server::getResources() const {
	return _config->resources;
}

const Map<String, Server::RequestScheme> &Server::getRequestHandlers() const {
	return _config->requests;
}

NS_SA_END
