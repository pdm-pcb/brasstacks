#ifndef BRASSTACKS_EVENTS_EVENTLISTENER_HPP
#define BRASSTACKS_EVENTS_EVENTLISTENER_HPP

namespace btx {

class Event;

class EventListener {
public:
    virtual void on_event(Event &event) = 0;

protected:
    EventListener() = default;
};

} // namespace btx

#endif // BRASSTACKS_EVENTS_EVENTLISTENER_HPP