#ifndef BRASSTACKS_EVENTS_UI_EVENTS_HPP
#define BRASSTACKS_EVENTS_UI_EVENTS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventBase.hpp"

#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

enum class UIEventType : uint8_t {
    UI_EXIT = 0u,
    UI_WINDOW_RESIZE,
    UI_CHANGE_ANISO,
    UI_CHANGE_MSAA,
    UI_TOGGLE_VSYNC,
};

struct UIEvent : public EventBase {
    explicit UIEvent(UIEventType const event_type) :
        EventBase(),
        type { event_type }
    { }

    UIEvent() = delete;

    UIEventType const type;
};

} //namespace btx

#endif // BRASSTACKS_EVENTS_UI_EVENTS_HPP