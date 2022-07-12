#ifndef BRASSTACKS_UTILITY_LOGGER_HPP
#define BRASSTACKS_UTILITY_LOGGER_HPP

#define SPDLOG_NO_THREAD_ID
#include <spdlog/spdlog.h>

namespace btx {

class Logger {
public:
    static std::shared_ptr<spdlog::logger> & engine() { return _engine; }
    static std::shared_ptr<spdlog::logger> & client() { return _client; }

    static void init();

    Logger(const Logger &&) = delete;
    Logger(Logger &)        = delete;

    Logger& operator=(const Logger &&) = delete;
    Logger& operator=(Logger &)        = delete;

private:
    static std::shared_ptr<spdlog::logger> _engine;
    static std::shared_ptr<spdlog::logger> _client;

    Logger()  = default;
    ~Logger() = default;
};

} // namespace btx

// #ifdef DEBUG
    #define BTX_ENGINE_TRACE(...)    ::btx::Logger::engine()->trace(__VA_ARGS__)
    #define BTX_ENGINE_INFO(...)     ::btx::Logger::engine()->info(__VA_ARGS__)
    #define BTX_ENGINE_WARN(...)     ::btx::Logger::engine()->warn(__VA_ARGS__)
    #define BTX_ENGINE_ERROR(...)    ::btx::Logger::engine()->error(__VA_ARGS__);
    #define BTX_ENGINE_CRITICAL(...) ::btx::Logger::engine()->critical(__VA_ARGS__);

    #define BTX_TRACE(...)           ::btx::Logger::client()->trace(__VA_ARGS__)
    #define BTX_INFO(...)            ::btx::Logger::client()->info(__VA_ARGS__)
    #define BTX_WARN(...)            ::btx::Logger::client()->warn(__VA_ARGS__)
    #define BTX_ERROR(...)           ::btx::Logger::client()->error(__VA_ARGS__);
    #define BTX_CRITICAL(...)        ::btx::Logger::client()->critical(__VA_ARGS__);

    #define BTX_ASSERT(x, ...) assert(x && __VA_ARGS__)
// #else
//     #define BTX_ENGINE_TRACE(...)    
//     #define BTX_ENGINE_INFO(...)     
//     #define BTX_ENGINE_WARN(...)     
//     #define BTX_ENGINE_ERROR(...)    
//     #define BTX_ENGINE_CRITICAL(...) 

//     #define BTX_TRACE(...)           
//     #define BTX_INFO(...)            
//     #define BTX_WARN(...)            
//     #define BTX_ERROR(...)           
//     #define BTX_CRITICAL(...)

//     #define BTX_ASSERT(x, ...)
// #endif // DEBUG

#endif // BRASSTACKS_UTILITY_LOGGER_HPP