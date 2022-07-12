#ifndef BRASSTACKS_EVENTS_EVENTPUBLISHER_HPP
#define BRASSTACKS_EVENTS_EVENTPUBLISHER_HPP

#include "brasstacks/Events/Event.hpp"

#include <map>
#include <vector>

namespace btx {

class EventListener;

class EventPublisher {
    // TODO: convert this silliness to Andre's style of things
    using ListenerMap = std::map<const EventType, std::vector<EventListener *>>;

public:
    void subscribe_to(EventListener *listener, const EventType event_type);
    void unsubscribe(EventListener *listener, const EventType event_type);

    virtual ~EventPublisher() = default;

protected:
    void publish(const EventType event_type, Event &event);

    EventPublisher();

private:
    ListenerMap _subscribers;
};

} // namespace btx

#endif // BRASSTACKS_EVENTS_EVENTPUBLISHER_HPP