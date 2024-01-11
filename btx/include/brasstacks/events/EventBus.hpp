#ifndef BRASSTACKS_EVENTS_EVENTBUS_HPP
#define BRASSTACKS_EVENTS_EVENTBUS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventBase.hpp"
#include "brasstacks/tools/ConsoleLog.hpp"

namespace btx {

class EventBus final {
public:
    template<typename EventType, typename Queue, typename PushFnPtr>
    static void subscribe(Queue &queue, PushFnPtr callback) {
        auto const event_id = _event_id<EventType>();
        BTX_TRACE("Subscribing to event {}", event_id);
        _queues_by_event[event_id].emplace_back(
            [&queue, callback](const EventBase &event) {
                (queue.*callback)(event);
            }
        );
    }

    template <typename EventType>
    static void publish(EventType const &event) {
        auto const event_id = _event_id<EventType>();
        for(auto const &callback : _queues_by_event[event_id]) {
            callback(event);
        }
    }

private:
    using PushFn = std::function<void(EventBase const &)>;
    static std::vector<std::vector<PushFn>> _queues_by_event;

    template <typename Event>
    static uint32_t _event_id() {
        static uint32_t const event_id = _register_event();
        _queues_by_event.push_back({ });
        return event_id;
    }

    static uint32_t _register_event() {
        auto const event_id = static_cast<uint32_t>(_queues_by_event.size());
        BTX_TRACE("Registered event with ID {}", event_id);
        return event_id;
    }
};

} // namespace btx

#endif // BRASSTACKS_EVENTS_EVENTBUS_HPP