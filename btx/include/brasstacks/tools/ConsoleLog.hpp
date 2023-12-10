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
    // Client logging
    template<typename ...T>
    static void app_trace(std::format_string<T...> fmt, T&& ...args);

    template<typename ...T>
    static void app_info(std::format_string<T...> fmt, T&& ...args);

    template<typename ...T>
    static void app_warn(std::format_string<T...> fmt, T&& ...args);

    template<typename ...T>
    static void app_error(std::format_string<T...> fmt, T&& ...args);

    template<typename ...T>
    static void app_critical(std::format_string<T...> fmt, T&& ...args);

    // -------------------------------------------------------------------------
    ConsoleLog() = delete;

private:
    static std::shared_ptr<spdlog::logger> _btx_logger;
    static std::shared_ptr<spdlog::logger> _app_logger;
};

// =============================================================================
// Library logging
template<typename ...T>
void ConsoleLog::btx_trace(std::format_string<T...> fmt, T&& ...args) {
    _btx_logger->trace(fmt, std::forward<T>(args)...);
}

template<typename ...T>
void ConsoleLog::btx_info(std::format_string<T...> fmt, T&& ...args) {
    _btx_logger->info(fmt, std::forward<T>(args)...);
}

template<typename ...T>
void ConsoleLog::btx_warn(std::format_string<T...> fmt, T&& ...args) {
    _btx_logger->warn(fmt, std::forward<T>(args)...);
}

template<typename ...T>
void ConsoleLog::btx_error(std::format_string<T...> fmt, T&& ...args) {
    _btx_logger->error(fmt, std::forward<T>(args)...);
}

template<typename ...T>
void ConsoleLog::btx_critical(std::format_string<T...> fmt, T&& ...args) {
    _btx_logger->critical(fmt, std::forward<T>(args)...);
    std::abort();
}

// =============================================================================
// Client logging
template<typename ...T>
void ConsoleLog::app_trace(std::format_string<T...> fmt, T&& ...args) {
    _app_logger->trace(fmt, std::forward<T>(args)...);
}

template<typename ...T>
void ConsoleLog::app_info(std::format_string<T...> fmt, T&& ...args) {
    _app_logger->info(fmt, std::forward<T>(args)...);
}

template<typename ...T>
void ConsoleLog::app_warn(std::format_string<T...> fmt, T&& ...args) {
    _app_logger->warn(fmt, std::forward<T>(args)...);
}

template<typename ...T>
void ConsoleLog::app_error(std::format_string<T...> fmt, T&& ...args) {
    _app_logger->error(fmt, std::forward<T>(args)...);
}

template<typename ...T>
void ConsoleLog::app_critical(std::format_string<T...> fmt, T&& ...args) {
    _app_logger->critical(fmt, std::forward<T>(args)...);
    std::abort();
}

} // namespace btx

// Convenience wrappers
#define BTX_TRACE(...)    btx::ConsoleLog::btx_trace(__VA_ARGS__)
#define BTX_INFO(...)     btx::ConsoleLog::btx_info(__VA_ARGS__)
#define BTX_WARN(...)     btx::ConsoleLog::btx_warn(__VA_ARGS__)
#define BTX_ERROR(...)    btx::ConsoleLog::btx_error(__VA_ARGS__)
#define BTX_CRITICAL(...) btx::ConsoleLog::btx_critical(__VA_ARGS__);

#define CONSOLE_TRACE(...)    btx::ConsoleLog::app_trace(__VA_ARGS__)
#define CONSOLE_INFO(...)     btx::ConsoleLog::app_info(__VA_ARGS__)
#define CONSOLE_WARN(...)     btx::ConsoleLog::app_warn(__VA_ARGS__)
#define CONSOLE_ERROR(...)    btx::ConsoleLog::app_error(__VA_ARGS__)
#define CONSOLE_CRITICAL(...) btx::ConsoleLog::app_critical(__VA_ARGS__);

#endif // BRASSTACKS_TOOLS_CONSOLELOG_HPP