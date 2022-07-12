#include "brasstacks/Utility/Logger.hpp"

#include <spdlog/fmt/xchar.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace btx {

std::shared_ptr<spdlog::logger> Logger::_engine;
std::shared_ptr<spdlog::logger> Logger::_client;

void Logger::init() {
    spdlog::set_pattern("%^[%T] %n: %v%$");
    spdlog::set_level(spdlog::level::trace);

    _engine = spdlog::stdout_color_mt("Engine");
    _client = spdlog::stdout_color_mt("Client");

    BTX_ENGINE_INFO("spdlog v{}.{}.{}", SPDLOG_VER_MAJOR,
                                       SPDLOG_VER_MINOR,
                                       SPDLOG_VER_PATCH);
}

} // namespace btx