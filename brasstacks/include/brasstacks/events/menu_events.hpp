#ifndef BRASSTACKS_EVENTS_MENU_EVENTS_HPP
#define BRASSTACKS_EVENTS_MENU_EVENTS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventBase.hpp"

#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

enum class MenuEventType : uint8_t {
    MENU_EXIT = 0u,
    MENU_CHANGE_WINDOW_SIZE
};

struct MenuEvent : public EventBase {
    explicit MenuEvent(MenuEventType const type) :
        EventBase(),
        event_type { type }
    { }

    MenuEvent() = delete;

    MenuEventType const event_type;
};

} //namespace btx

#endif // BRASSTACKS_EVENTS_MENU_EVENTS_HPP