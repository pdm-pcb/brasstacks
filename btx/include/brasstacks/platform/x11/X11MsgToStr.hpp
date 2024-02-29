#ifdef BTX_UNUSED

#ifndef BRASSTACKS_PLATFORM_X11_X11MSGTOSTR_HPP
#define BRASSTACKS_PLATFORM_X11_X11MSGTOSTR_HPP

#ifdef BTX_LINUX

#include "brasstacks/pch.hpp"
#include "brasstacks/tools/ConsoleLog.hpp"

namespace btx {

class X11MsgToStr final {
public:
    inline std::string_view const translate(uint32_t const msg) const {
        auto const result = _map.find(msg);
        if(result == _map.end()) {
            BTX_WARN("Unknown win32 msg: {:#x}", msg);
            return _map.begin()->second;
        }

        return result->second;
    }

    X11MsgToStr();
    ~X11MsgToStr() = default;

    X11MsgToStr(X11MsgToStr &&) = delete;
    X11MsgToStr(const X11MsgToStr &) = delete;

    X11MsgToStr & operator=(X11MsgToStr &&) = delete;
    X11MsgToStr & operator=(const X11MsgToStr &) = delete;

private:
    std::unordered_map<uint32_t, std::string> const _map;
};

} // namespace btx

#endif // BTX_LINUX

#endif // BRASSTACKS_PLATFORM_X11_X11MSGTOSTR_HPP

#endif // BTX_UNUSED