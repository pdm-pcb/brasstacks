// Credit to kainev for this whole shebang:
// https://codereview.stackexchange.com/questions/252884/

#ifndef BRASSTACKS_EVENTS_EVENTBROKER_HPP
#define BRASSTACKS_EVENTS_EVENTBROKER_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventCallbacks.hpp"

namespace btx {

class EventBroker {
public:
    // =========================================================================
    template<typename Event, typename Listener, typename Callback>
    static EventListenerHandle subscribe(Listener *handler, Callback callback) {
        return static_cast<EventCallbacks<Event> *>(
            _callbacks[_event_id<Event>()])->add(
            [handler, callback](const Event &event) {
                (handler->*callback)(event);
            }
        );
    }

    // =========================================================================
    template<typename Event, typename... EventParams>
    static void emit(EventParams ...event_args) {
        Event event { event_args... };
        static_cast<EventCallbacks<Event> *>(
            _callbacks[_event_id<Event>()]
        )->emit(event);
    }

    // =========================================================================
    static void unsubscribe(const EventListenerHandle &handle) {
        _callbacks[handle.event_id]->remove(handle);
    }

    // =========================================================================
    static void init() {
        _callbacks.reserve(EventCallbacksBase::MAX_CALLBACKS);
    }

    // =========================================================================
    static void shutdown() {
        for(auto const *callback : _callbacks) {
            delete callback;
        }
    }

    // =========================================================================
    EventBroker() = delete;
    ~EventBroker() = delete;

    EventBroker(EventBroker &&other) = delete;
    EventBroker(const EventBroker &other) = delete;

    EventBroker & operator=(EventBroker &&other) = delete;
    EventBroker & operator=(const EventBroker &other) = delete;

private:
    using CallbackList = std::vector<EventCallbacksBase *>;
    static CallbackList _callbacks;
    static uint32_t _next_event_id;

    // =========================================================================
    template<typename Event>
    static uint32_t _event_id() {
        static uint32_t event_id = _register_event<Event>();
        return event_id;
    }

    // =========================================================================
    template<typename Event>
    static uint32_t _register_event() {
        _callbacks.emplace_back(new EventCallbacks<Event>(_next_event_id));
        BTX_TRACE("Event callbacks registered: {}", _callbacks.size());
        return _next_event_id++;
    }
};

} // namespace btx

#endif //  BRASSTACKS_EVENTS_EVENTBROKER_HPP