#ifndef BRASSTACKS_EVENTS_EVENTBUS_HPP
#define BRASSTACKS_EVENTS_EVENTBUS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventBase.hpp"
#include "brasstacks/tools/ConsoleLog.hpp"

namespace btx {

class EventBus final {
public:
    template<typename EventType>
    static void register_event() {
        auto const event_id = _event_id<EventType>();

        static std::mutex registration_mutex;
        std::unique_lock<std::mutex> lock(registration_mutex);

        if(event_id == _callbacks_by_event.size()) {
            _callbacks_by_event.emplace_back(0);
            BTX_TRACE("Registered event type with ID {}", event_id);
        }
    }

    using QueueCallback = std::function<void(EventBase const &)>;
    using QueueCallbacks = std::list<QueueCallback>;
    using QueueCallbackIter = QueueCallbacks::iterator;

    template<typename EventType, typename Queue, typename Callback>
    static QueueCallbackIter const subscribe(Queue &queue, Callback callback) {
        auto const event_id = _event_id<EventType>();

        if(event_id >= _callbacks_by_event.size()) {
            BTX_CRITICAL("Attempting to subscribe to unknown event ID {}.",
                          event_id);
            return _callbacks_by_event[event_id].end();
        }

        _callbacks_by_event[event_id].emplace_back(
            [&queue, callback](const EventBase &event) {
                (queue.*callback)(event);
            }
        );

        return --_callbacks_by_event[event_id].end();
    }

    template<typename EventType>
    static void unsubscribe(QueueCallbackIter const iter) {
        auto const event_id = _event_id<EventType>();

        if(event_id >= _callbacks_by_event.size()) {
            BTX_CRITICAL("Attempting to unsubscribe to unknown event ID {}.",
                          event_id);
            return;
        }

        _callbacks_by_event[event_id].erase(iter);
    }

    template <typename EventType>
    static void publish(EventType const &event) {
        auto const event_id = _event_id<EventType>();
        for(auto const &callback : _callbacks_by_event[event_id]) {
            callback(event);
        }
    }

private:
    static std::vector<QueueCallbacks> _callbacks_by_event;

    template <typename Event>
    static inline uint32_t _event_id() {
        static auto const event_id =
            static_cast<uint32_t>(_callbacks_by_event.size());
        return event_id;
    }

    };

} // namespace btx

#endif // BRASSTACKS_EVENTS_EVENTBUS_HPP