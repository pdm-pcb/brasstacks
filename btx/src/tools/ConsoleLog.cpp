#include "brasstacks/brasstacks.hpp"
#include "brasstacks/tools/ConsoleLog.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace btx {

std::shared_ptr<spdlog::logger> ConsoleLog::_btx_logger = nullptr;
std::shared_ptr<spdlog::logger> ConsoleLog::_app_logger = nullptr;

// =============================================================================
void ConsoleLog::init() {
    static std::once_flag initialized;
    std::call_once(initialized, [] {
        // Grab a threadsafe logger that supports colorized output
        _btx_logger = spdlog::stdout_color_mt("btx_logger");
        _app_logger = spdlog::stdout_color_mt("app_logger");

        // Print with color, time with milliseconds, and thread ID
        _btx_logger->set_pattern("%^[%T.%e][%t][BTX]: %v%$");
        _app_logger->set_pattern("%^[%T.%e][%t]: %v%$");

        // Default to everything
        _btx_logger->set_level(spdlog::level::trace);
        _app_logger->set_level(spdlog::level::trace);

        BTX_INFO("spdlog v{}.{}.{}", SPDLOG_VER_MAJOR,
                                     SPDLOG_VER_MINOR,
                                     SPDLOG_VER_PATCH);
    });
}

// =============================================================================
void ConsoleLog::set_level(Level log_level) {
    // Make sure we're set up
    init();

    // Set the severity to print
    switch(log_level) {
        case Level::TRACE:
            _btx_logger->set_level(spdlog::level::trace);
            _app_logger->set_level(spdlog::level::trace);
            break;
        case Level::INFO:
            _btx_logger->set_level(spdlog::level::info);
            _app_logger->set_level(spdlog::level::info);
            break;
        case Level::WARN:
            _btx_logger->set_level(spdlog::level::warn);
            _app_logger->set_level(spdlog::level::warn);
            break;
        case Level::ERROR:
            _btx_logger->set_level(spdlog::level::err);
            _app_logger->set_level(spdlog::level::err);
            break;
        case Level::CRITICAL:
            _btx_logger->set_level(spdlog::level::critical);
            _app_logger->set_level(spdlog::level::critical);
            break;
    }
}

} // namespace btx