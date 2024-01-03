#ifndef BRASSTACKS_EVENTS_WINDOW_EVENTS_HPP
#define BRASSTACKS_EVENTS_WINDOW_EVENTS_HPP

#include "brasstacks/pch.hpp"

namespace btx {

// Empty struct to carry the signal
struct WindowCloseEvent final { };

struct WindowSizeEvent final { };

struct WindowMinimizeEvent final { };

struct WindowRestoreEvent final { };

} //namespace btx

#endif // BRASSTACKS_EVENTS_WINDOW_EVENTS_HPP