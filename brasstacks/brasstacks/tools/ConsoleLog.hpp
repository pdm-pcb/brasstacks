/**
 * @file ConsoleLog.hpp
 * @brief A wrapper class for spdlog with a few customizations.
 */

#ifndef BRASSTACKS_TOOLS_CONSOLELOG_HPP
#define BRASSTACKS_TOOLS_CONSOLELOG_HPP

#include "brasstacks/pch.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace btx {

/**
 * @brief A wrapper class for spdlog with a few customizations.
 *
 * ConsoleLog is the means by which to log to the console. =) The intended API
 * is for everything to use the macros at the bottom of this file.
 */
class ConsoleLog final {
public:
    /**
     * @brief The verbosity levels supported by ConsoleLog.
     */
    enum class Level : uint8_t {
        LOG_TRACE,
        LOG_INFO,
        LOG_WARN,
        LOG_ERROR,
        LOG_CRITICAL
    };

    /**
     * @brief Set the logger's verbosity.
     * @param log_level The chosen level of verbosity
     */
    static void set_level(Level log_level);

    /**
     * @brief Log a trace level message.
     * @param fmt Format string
     * @param args Format string arguments
     */
    template<typename ...T>
    static void btx_trace(fmt::format_string<T...> fmt, T&& ...args);

    /**
     * @brief Log a information level message.
     * @param fmt Format string
     * @param args Format string arguments
     */
    template<typename ...T>
    static void btx_info(fmt::format_string<T...> fmt, T&& ...args);

    /**
     * @brief Log a warning level message.
     * @param fmt Format string
     * @param args Format string arguments
     */
    template<typename ...T>
    static void btx_warn(fmt::format_string<T...> fmt, T&& ...args);

    /**
     * @brief Log an error level message.
     * @param fmt Format string
     * @param args Format string arguments
     */
    template<typename ...T>
    static void btx_error(fmt::format_string<T...> fmt, T&& ...args);

    /**
     * @brief Log a critical level message. This will assert() in debug.
     * @param fmt Format string
     * @param args Format string arguments
     */
    template<typename ...T>
    static void btx_critical(fmt::format_string<T...> fmt, T&& ...args);

    ConsoleLog() = delete;

private:
    /**
     * @brief The logger instance
     */
    static spdlog::logger *_logger;

    /**
     * @brief Bring up the logger instance.
     *
     * This function contains a std::call_once()-controlled lambda to avoid
     * duplicate calls.
     */
	static void _init();
};

// =============================================================================
// Implementations

template<typename ...T>
void ConsoleLog::btx_trace(fmt::format_string<T...> fstring, T&& ...args) {
    _init();
    _logger->trace(fstring, std::forward<T>(args)...);
}

template<typename ...T>
void ConsoleLog::btx_info(fmt::format_string<T...> fstring, T&& ...args) {
    _init();
    _logger->info(fstring, std::forward<T>(args)...);
}

template<typename ...T>
void ConsoleLog::btx_warn(fmt::format_string<T...> fstring, T&& ...args) {
    _init();
    _logger->warn(fstring, std::forward<T>(args)...);
}

template<typename ...T>
void ConsoleLog::btx_error(fmt::format_string<T...> fstring, T&& ...args) {
    _init();
    _logger->error(fstring, std::forward<T>(args)...);
}

template<typename ...T>
void ConsoleLog::btx_critical(fmt::format_string<T...> fstring, T&& ...args) {
    _init();

    auto const errmsg = fmt::format(fstring, std::forward<T>(args)...);

    _logger->critical(errmsg);
    assert(false);
}

} // namespace btx

// =============================================================================
// Convenience wrappers

#define BTX_TRACE(...)    btx::ConsoleLog::btx_trace(__VA_ARGS__)
#define BTX_INFO(...)     btx::ConsoleLog::btx_info(__VA_ARGS__)
#define BTX_WARN(...)     btx::ConsoleLog::btx_warn(__VA_ARGS__)
#define BTX_ERROR(...)    btx::ConsoleLog::btx_error(__VA_ARGS__)
#define BTX_CRITICAL(...) btx::ConsoleLog::btx_critical(__VA_ARGS__)

#endif // BRASSTACKS_TOOLS_CONSOLELOG_HPP