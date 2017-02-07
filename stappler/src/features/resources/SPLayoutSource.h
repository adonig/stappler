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

#ifndef STAPPLER_SRC_FEATURES_RESOURCES_SPLAYOUTSOURCE_H_
#define STAPPLER_SRC_FEATURES_RESOURCES_SPLAYOUTSOURCE_H_

#include "SLStyle.h"
#include "SLDocument.h"
#include "SPFont.h"
#include "SPAsset.h"
#include "SPEventHeader.h"
#include "SPDataListener.h"

NS_LAYOUT_BEGIN

class Source : public font::FontController {
public:
	static EventHeader onError;
	static EventHeader onDocument;

	enum Error : int64_t /* event compatible */ {
		DocumentError,
		NetworkError,
	};

	using StringDocument = ValueWrapper<String, class DocumentStringDocumentTag>;

	virtual ~Source();

	virtual bool init();
	virtual bool init(const StringDocument &str);
	virtual bool init(const FilePath &file);
	virtual bool init(const String &url, const String &path,
			TimeInterval ttl = TimeInterval(), const String &cacheDir = "", const Asset::DownloadCallback & = nullptr);
	virtual bool init(Asset *a, bool enabled = true);

	Document *getDocument() const;
	Asset *getAsset() const;

	bool isReady() const;
	bool isActual() const;
	bool isDocumentLoading() const;

	void refresh();

	bool tryReadLock(Ref *);
	void retainReadLock(Ref *, const Function<void()> &);
	void releaseReadLock(Ref *);

	void setEnabled(bool val);
	bool isEnabled() const;

protected:
	virtual void onDocumentAsset(Asset *);
	virtual void onDocumentAssetUpdated(data::Subscription::Flags);
	virtual void onDocumentLoaded(Document *);

	virtual void tryLoadDocument();
	virtual void updateDocument();

	virtual Rc<font::FontSource> makeSource(AssetMap &&, bool schedule = false) override;

	virtual Rc<Document> openDocument(const String &path, const String &ct);

	String _string;
	String _file;

	data::Listener<Asset> _documentAsset;
	Rc<Document> _document;

	uint64_t _loadedAssetMTime = 0;
	bool _documentLoading = false;
	bool _enabled = true;
};

NS_LAYOUT_END

#endif /* STAPPLER_SRC_FEATURES_RESOURCES_SPLAYOUTSOURCE_H_ */