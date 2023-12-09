// Credit to kainev for this whole shebang:
// https://codereview.stackexchange.com/questions/252884/

#ifndef BRASSTACKS_EVENTS_EVENTCALLBACKS_HPP
#define BRASSTACKS_EVENTS_EVENTCALLBACKS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventListenerHandle.hpp"

namespace btx {

class EventCallbacksBase {
public:
    static constexpr uint32_t MAX_CALLBACKS = 32u;

    virtual void remove(const EventListenerHandle &) { }

    virtual ~EventCallbacksBase() = default;

    EventCallbacksBase(EventCallbacksBase &&other) = delete;
    EventCallbacksBase(const EventCallbacksBase &other) = delete;

    EventCallbacksBase & operator=(EventCallbacksBase &&other) = delete;
    EventCallbacksBase & operator=(const EventCallbacksBase &other) = delete;

protected:
    EventCallbacksBase() = default;
};

template<typename Event>
class EventCallbacks final : public EventCallbacksBase {

using CallbackList = std::list<std::function<void(const Event &)>>;
using CallbackIter = typename CallbackList::iterator;
using HandleMap    = std::unordered_map<uint32_t, CallbackIter>;

public:
    template<typename Callback>
    EventListenerHandle add(Callback callback) {
        _callbacks.emplace_back(callback);
        _handles.emplace(++_next_listener_id, std::prev(_callbacks.end()));
        return {
            .event_id = _event_id,
            .listener_id = _next_listener_id,
        };
    }

    void emit(const Event &event) {
        for(auto const &callback : _callbacks) {
            callback(event);
        }
    }

    void remove(const EventListenerHandle &handle) override {
        auto iter = _handles[handle.listener_id];
        _handles.erase(handle.listener_id);
        _callbacks.erase(iter);
    }

    explicit EventCallbacks(const uint32_t event_id) :
        _event_id { event_id },
        _next_listener_id { 0u }
    { }
    EventCallbacks() = delete;

    ~EventCallbacks() override = default;

    EventCallbacks(EventCallbacks &&other) = delete;
    EventCallbacks(const EventCallbacks &other) = delete;

    EventCallbacks & operator=(EventCallbacks &&other) = delete;
    EventCallbacks & operator=(const EventCallbacks &other) = delete;

private:
    const uint32_t _event_id;
    uint32_t       _next_listener_id;
    CallbackList   _callbacks;
    HandleMap      _handles;
};

} // namespace btx

#endif // BRASSTACKS_EVENTS_EVENTCALLBACKS_HPP