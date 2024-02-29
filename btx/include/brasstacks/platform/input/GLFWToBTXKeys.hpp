#ifndef BRASSTACKS_PLATFORM_INPUT_GLFWTOBTXKEYS_HPP
#define BRASSTACKS_PLATFORM_INPUT_GLFWTOBTXKEYS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/input/btx_keycodes.hpp"
#include "brasstacks/tools/ConsoleLog.hpp"

namespace btx {

class GLFWToBTXKeys final {
public:
    inline auto translate(int const key) const {
        auto const result = _map.find(key);
        if(result == _map.end()) {
            BTX_WARN("Unknown GLFW key: {}", key);
            return BTX_KB_UNKNOWN;
        }

        return result->second;
    }

    GLFWToBTXKeys();
    ~GLFWToBTXKeys() = default;

    GLFWToBTXKeys(GLFWToBTXKeys &&) = delete;
    GLFWToBTXKeys(const GLFWToBTXKeys &) = delete;

    GLFWToBTXKeys & operator=(GLFWToBTXKeys &&) = delete;
    GLFWToBTXKeys & operator=(const GLFWToBTXKeys &) = delete;

private:
    std::unordered_map<int, Keycode> const _map;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_INPUT_GLFWTOBTXKEYS_HPP