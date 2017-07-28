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

#ifndef __stappler__SPEventHandler__
#define __stappler__SPEventHandler__

#include "SPEventHeader.h"

NS_SP_BEGIN

/* EventHandler - simplified system for event handling

 To handle event we need object, that derived from
 EventHandlerSupport (multiple inherinance is allowed),
 and call sp_onEvent function

 onEvent(event, callback, destroyAfterEvent);
 onEvent(event, [this] (const Event *ev) { }, destroyAfterEvent);

  - register event callback for event (by event handler)
 event - reference to target event header
 callback - std::function<void(const Event *)> - called when event recieved
 destroyAfterEvent - should be set to true if we need only one-time event handling

 */

class EventHandler {
public:
	using Callback = Function<void(const Event &)>;

	EventHandler();
	virtual ~EventHandler();

	void addHandlerNode(EventHandlerNode *handler);
	void removeHandlerNode(EventHandlerNode *handler);

	EventHandlerNode * onEvent(const EventHeader &h, Callback && callback, bool destroyAfterEvent = false);
	EventHandlerNode * onEventWithObject(const EventHeader &h, Ref *obj, Callback && callback, bool destroyAfterEvent = false);

	Ref *getInterface() const;

	void clearEvents();

private:
	Set<EventHandlerNode *> _handlers;
};

class EventHandlerNode : public Ref {
public:
	using Callback = std::function<void(const Event &)>;

	static EventHandlerNode * onEvent(const EventHeader &header, Ref *ref, Callback && callback, EventHandler *obj, bool destroyAfterEvent);

	void setSupport(EventHandler *);

	bool shouldRecieveEventWithObject(EventHeader::EventID eventID, Ref *object) const {
		return _eventID == eventID && (!_obj || object == _obj);
	};

	EventHeader::EventID getEventID() const { return _eventID; }

	void onEventRecieved(const Event &event) const;

	~EventHandlerNode();

private:
	EventHandlerNode(const EventHeader &header, Ref *ref, Callback && callback, EventHandler *obj, bool destroyAfterEvent);

	bool _destroyAfterEvent = false;

	EventHeader::EventID _eventID;
	Callback _callback;
	Ref *_obj = nullptr;

	std::atomic<EventHandler *>_support;
};

NS_SP_END

#endif /* defined(__stappler__SPEventHandler__) */
