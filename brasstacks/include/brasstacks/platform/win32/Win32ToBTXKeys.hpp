#ifdef BTX_UNUSED

#ifndef BRASSTACKS_PLATFORM_WIN32_WIN32TOBTXKEYS_HPP
#define BRASSTACKS_PLATFORM_WIN32_WIN32TOBTXKEYS_HPP

#ifdef BTX_WINDOWS

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/input/btx_keycodes.hpp"
#include "brasstacks/tools/ConsoleLog.hpp"

namespace btx {

class Win32ToBTXKeys final {
public:
    inline auto translate(::USHORT const vkey) const {
        auto const result = _map.find(vkey);
        if(result == _map.end()) {
            BTX_WARN("Unknown win32 vkey: {}", vkey);
            return BTX_KB_UNKNOWN;
        }

        return result->second;
    }

    Win32ToBTXKeys();
    ~Win32ToBTXKeys() = default;

    Win32ToBTXKeys(Win32ToBTXKeys &&) = delete;
    Win32ToBTXKeys(const Win32ToBTXKeys &) = delete;

    Win32ToBTXKeys & operator=(Win32ToBTXKeys &&) = delete;
    Win32ToBTXKeys & operator=(const Win32ToBTXKeys &) = delete;

private:
    std::unordered_map<::USHORT, Keycode> const _map;
};

} // namespace btx

#endif // BTX_WINDOWS

#endif // BRASSTACKS_PLATFORM_WIN32_WIN32TOBTXKEYS_HPP

#endif // BTX_UNUSED