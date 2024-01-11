#ifndef BRASSTACKS_EVENTS_EVENTQUEUE_HPP
#define BRASSTACKS_EVENTS_EVENTQUEUE_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventBus.hpp"
#include "brasstacks/events/EventBase.hpp"

namespace btx {

template <typename EventType>
class EventQueue final {
public:
    template<typename Subscriber, typename CallbackPtr>
    explicit EventQueue(Subscriber &subscriber, CallbackPtr const &callback) :
        _queue_a     { },
        _queue_b     { },
        _front_queue { &_queue_a },
        _back_queue  { &_queue_b },
        _callback {
            [&subscriber, callback](EventBase const &event) {
                (subscriber.*callback)(reinterpret_cast<EventType const &>(event));
            }
        }
    {
        EventBus::subscribe<EventType>(*this, &EventQueue<EventType>::push);
    }

    ~EventQueue() = default;

    EventQueue() = delete;

    EventQueue(EventQueue &&) = delete;
    EventQueue(EventQueue const &) = delete;

    EventQueue & operator=(EventQueue &&) = delete;
    EventQueue & operator=(EventQueue const &) = delete;

    void push(EventBase const &event) {
        auto const &event_casted = reinterpret_cast<EventType const &>(event);
        std::unique_lock<std::mutex> push_lock(_front_queue_mutex);
        _front_queue->push(event_casted);
    }

    void process_queue() {
        assert(_back_queue->empty());
        {
            std::unique_lock<std::mutex> swap_lock(_front_queue_mutex);
            if(_front_queue->empty()) {
                return;
            }
            std::swap(_front_queue, _back_queue);
        }

        auto const message_count = _back_queue->size();
        for(size_t count = 0; count < message_count; ++count) {
            auto const event = _back_queue->front();
            _back_queue->pop();
            _callback(event);
        }

        assert(_back_queue->empty());
    }

private:
    std::queue<EventType> _queue_a;
    std::queue<EventType> _queue_b;

    std::queue<EventType> *_front_queue;
    std::queue<EventType> *_back_queue;

    std::mutex _front_queue_mutex;

    using Callback = std::function<void(EventType const &)>;
    Callback const _callback;
};

} // namespace btx

#endif // BRASSTACKS_EVENTS_EVENTQUEUE_HPP