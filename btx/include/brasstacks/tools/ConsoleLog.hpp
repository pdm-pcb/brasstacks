#ifndef BRASSTACKS_TOOLS_CONSOLELOG_HPP
#define BRASSTACKS_TOOLS_CONSOLELOG_HPP

#include "brasstacks/pch.hpp"
#include <spdlog/spdlog.h>

namespace btx {

class ConsoleLog final {
public:
    // -------------------------------------------------------------------------
    // Setup
	static void init();

    enum class Level : std::uint8_t {
        LOG_TRACE,
        LOG_INFO,
        LOG_WARN,
        LOG_ERROR,
        LOG_CRITICAL
    };

    static void set_level(Level log_level);

    // -------------------------------------------------------------------------
    // Library logging
    template<typename ...T>
    static void btx_trace(std::format_string<T...> fmt, T&& ...args);

    template<typename ...T>
    static void btx_info(std::format_string<T...> fmt, T&& ...args);

    template<typename ...T>
    static void btx_warn(std::format_string<T...> fmt, T&& ...args);

    template<typename ...T>
    static void btx_error(std::format_string<T...> fmt, T&& ...args);

    template<typename ...T>
    static void btx_critical(std::format_string<T...> fmt, T&& ...args);

    // -------------------------------------------------------------------------
    ConsoleLog() = delete;

private:
    static std::shared_ptr<spdlog::logger> _logger;
};

// =============================================================================
// Library logging
template<typename ...T>
void ConsoleLog::btx_trace(std::format_string<T...> fmt, T&& ...args) {
    _logger->trace(fmt, std::forward<T>(args)...);
}

template<typename ...T>
void ConsoleLog::btx_info(std::format_string<T...> fmt, T&& ...args) {
    _logger->info(fmt, std::forward<T>(args)...);
}

template<typename ...T>
void ConsoleLog::btx_warn(std::format_string<T...> fmt, T&& ...args) {
    _logger->warn(fmt, std::forward<T>(args)...);
}

template<typename ...T>
void ConsoleLog::btx_error(std::format_string<T...> fmt, T&& ...args) {
    _logger->error(fmt, std::forward<T>(args)...);
}

template<typename ...T>
void ConsoleLog::btx_critical(std::format_string<T...> fmt, T&& ...args) {
    _logger->critical(fmt, std::forward<T>(args)...);
    assert(false);
}

} // namespace btx

// Convenience wrappers
#define BTX_TRACE(...)    btx::ConsoleLog::btx_trace(__VA_ARGS__)
#define BTX_INFO(...)     btx::ConsoleLog::btx_info(__VA_ARGS__)
#define BTX_WARN(...)     btx::ConsoleLog::btx_warn(__VA_ARGS__)
#define BTX_ERROR(...)    btx::ConsoleLog::btx_error(__VA_ARGS__)
#define BTX_CRITICAL(...) btx::ConsoleLog::btx_critical(__VA_ARGS__)

#endif // BRASSTACKS_TOOLS_CONSOLELOG_HPP