#include "brasstacks/System/pch.hpp"
#include "brasstacks/Events/EventPublisher.hpp"

namespace btx {

void EventPublisher::subscribe_to(EventListener *listener,
                                  const EventType event_type) {
    _subscribers[event_type].emplace_back(listener);
}

void EventPublisher::unsubscribe(EventListener *listener,
                                 const EventType event_type) {
    auto iter = std::find(_subscribers[event_type].begin(),
                          _subscribers[event_type].end(), listener);

    if(iter == _subscribers[event_type].end()) {
        return;
    }

    _subscribers[event_type].erase(iter);
}

void EventPublisher::publish(const EventType event_type, Event &event) {
    for(auto listener : _subscribers[event_type]) {
        listener->on_event(event);
    }
}

EventPublisher::EventPublisher() {
    for(auto type : EnumRange<EventType>()) {
        _subscribers.emplace(
            std::make_pair(type, std::vector<EventListener *>())
        );
    }
}

} // namespace btx