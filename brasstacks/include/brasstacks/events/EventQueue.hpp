#ifndef BRASSTACKS_EVENTS_EVENTQUEUE_HPP
#define BRASSTACKS_EVENTS_EVENTQUEUE_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventBus.hpp"

namespace btx {

template <typename EventType>
class EventQueue final {
public:
    template<typename Subscriber, typename CallbackPtr>
    explicit EventQueue(Subscriber &subscriber, CallbackPtr const &callback) :
        _event_queue { },
        _callback {
            [&subscriber, callback](EventBase const &event) {
                (subscriber.*callback)(static_cast<EventType const &>(event));
            }
        },
        _subscriber_handle { }
    {
        EventBus::register_event<EventType>();
    }

    ~EventQueue() = default;

    EventQueue() = delete;

    EventQueue(EventQueue &&) = delete;
    EventQueue(EventQueue const &) = delete;

    EventQueue & operator=(EventQueue &&) = delete;
    EventQueue & operator=(EventQueue const &) = delete;

    void subscribe() {
        _subscriber_handle =
            EventBus::subscribe<EventType>(*this, &EventQueue<EventType>::push);
    }

    void unsubscribe() {
        if(_subscriber_handle.has_value()) {
            EventBus::unsubscribe<EventType>(_subscriber_handle.value());
        }
    }

    void push(EventBase const &event) {
        auto const &event_casted = static_cast<EventType const &>(event);
        _event_queue.push(event_casted);
    }

    void process_queue() {
        while(_event_queue.size() > 0) {
            _callback(_event_queue.front());
            _event_queue.pop();
        }
    }

    void clear() {
        _event_queue = { };
    }

private:
    std::queue<EventType> _event_queue;

    using Callback = std::function<void(EventType const &)>;
    Callback const _callback;

    std::optional<EventBus::QueueCallbackIter> _subscriber_handle;
};

} // namespace btx

#endif // BRASSTACKS_EVENTS_EVENTQUEUE_HPP