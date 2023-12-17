#include "brasstacks/tools/ConsoleLog.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace btx {

spdlog::logger *ConsoleLog::_logger = nullptr;

// =============================================================================
void ConsoleLog::_init() {
    // Ensure the logger is only set up once
    static std::once_flag initialized;

    std::call_once(initialized, [] {
        // Grab a threadsafe logger that supports colorized output
        _logger = spdlog::stdout_color_mt("btx_logger").get();

        // Print with color, time with milliseconds, and thread ID
        _logger->set_pattern("%^[%T.%e][%t]: %v%$");

        // Default to everything
        _logger->set_level(spdlog::level::trace);
    });
}

// =============================================================================
void ConsoleLog::set_level(Level log_level) {
    _init(); // Make sure we're set up

    // Set the severity to print
    switch(log_level) {
        case Level::LOG_TRACE: _logger->set_level(spdlog::level::trace); break;
        case Level::LOG_INFO : _logger->set_level(spdlog::level::info);  break;
        case Level::LOG_WARN : _logger->set_level(spdlog::level::warn);  break;
        case Level::LOG_ERROR: _logger->set_level(spdlog::level::err);   break;
        case Level::LOG_CRITICAL:
            _logger->set_level(spdlog::level::critical);
            break;
    }
}

} // namespace btx