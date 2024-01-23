#ifndef BRASSTACKS_PLATFORM_X11_X11TOBTXKEYS_HPP
#define BRASSTACKS_PLATFORM_X11_X11TOBTXKEYS_HPP

#ifdef BTX_LINUX

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/input/btx_keycodes.hpp"
#include "brasstacks/tools/ConsoleLog.hpp"

namespace btx {

class X11ToBTXKeys final {
public:
    inline auto translate(uint32_t const vkey) const {
        auto const result = _map.find(vkey);
        if(result == _map.end()) {
            BTX_WARN("Unknown x11 vkey: {}", vkey);
            return BTX_KB_UNKNOWN;
        }

        return result->second;
    }

    X11ToBTXKeys();
    ~X11ToBTXKeys() = default;

    X11ToBTXKeys(X11ToBTXKeys &&) = delete;
    X11ToBTXKeys(const X11ToBTXKeys &) = delete;

    X11ToBTXKeys & operator=(X11ToBTXKeys &&) = delete;
    X11ToBTXKeys & operator=(const X11ToBTXKeys &) = delete;

private:
    std::unordered_map<uint32_t, Keycode> const _map;
};

} // namespace btx

#endif // BTX_LINUX

#endif // BRASSTACKS_PLATFORM_X11_X11TOBTXKEYS_HPP