#ifndef BRASSTACKS_PLATFORM_WIN32_WIN32TOBTXKEYS_HPP
#define BRASSTACKS_PLATFORM_WIN32_WIN32TOBTXKEYS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/system/input/btx_keycodes.hpp"

namespace btx {

struct Win32ToBTXKeys final {
    static std::unordered_map<::USHORT, Keycode> const map;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_WIN32_WIN32TOBTXKEYS_HPP