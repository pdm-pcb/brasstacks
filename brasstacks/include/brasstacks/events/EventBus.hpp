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

    template<typename EventType, typename Queue, typename Callback>
    static void subscribe(Queue &queue, Callback callback) {
        auto const event_id = _event_id<EventType>();

        if(event_id >= _callbacks_by_event.size()) {
            BTX_CRITICAL("Attempting to subscribe to unknown event ID {}. "
                         "Did you forget to call EventBus::init()?",
                          event_id);
            return;
        }

        _callbacks_by_event[event_id].emplace_back(
            [&queue, callback](const EventBase &event) {
                (queue.*callback)(event);
            }
        );
    }

    template <typename EventType>
    static void publish(EventType const &event) {
        auto const event_id = _event_id<EventType>();
        for(auto const &callback : _callbacks_by_event[event_id]) {
            callback(event);
        }
    }

private:
    using QueueCallback = std::function<void(EventBase const &)>;
    using QueueCallbacks = std::vector<QueueCallback>;

    static std::vector<QueueCallbacks> _callbacks_by_event;

    template <typename Event>
    static uint32_t _event_id() {
        static auto const event_id =
            static_cast<uint32_t>(_callbacks_by_event.size());
        return event_id;
    }

    };

} // namespace btx

#endif // BRASSTACKS_EVENTS_EVENTBUS_HPP