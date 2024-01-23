#ifndef BRASSTACKS_PLATFORM_WIN32_WIN32MSGTOSTR_HPP
#define BRASSTACKS_PLATFORM_WIN32_WIN32MSGTOSTR_HPP

#ifdef BTX_WINDOWS

#include "brasstacks/pch.hpp"
#include "brasstacks/tools/ConsoleLog.hpp"

namespace btx {

class Win32MsgToStr final {
public:
    inline std::string_view const translate(::UINT const msg) const {
        auto const result = _map.find(msg);
        if(result == _map.end()) {
            BTX_WARN("Unknown win32 msg: {:#x}", msg);
            return _map.find(WM_NULL)->second;
        }

        return result->second;
    }

    Win32MsgToStr();
    ~Win32MsgToStr() = default;

    Win32MsgToStr(Win32MsgToStr &&) = delete;
    Win32MsgToStr(const Win32MsgToStr &) = delete;

    Win32MsgToStr & operator=(Win32MsgToStr &&) = delete;
    Win32MsgToStr & operator=(const Win32MsgToStr &) = delete;

private:
    std::unordered_map<::UINT, std::string> const _map;
};

} // namespace btx

#endif // BTX_WINDOWS

#endif // BRASSTACKS_PLATFORM_WIN32_WIN32MSGTOSTR_HPP