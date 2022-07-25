#ifndef BRASSTACKS_SYSTEM_PROFILER_HPP
#define BRASSTACKS_SYSTEM_PROFILER_HPP

#include <mutex>
#include <string>
#include <chrono>
#include <vector>
#include <array>

constexpr std::size_t LOG_LINES   = 1'048'576;
constexpr std::size_t LINE_LENGTH = 192;

namespace btx {

class Profiler {
public:
    using HRC = std::chrono::high_resolution_clock;

    static void init();
    static void shutdown();

    static Profiler * spawn(const char *name);
    void start();
    void stop(const double threshold_us);

    ~Profiler() = default;

    Profiler(const Profiler &&) = delete;
    Profiler(Profiler &)        = delete;
    Profiler & operator=(const Profiler &&) = delete;
    Profiler & operator=(Profiler &)        = delete;

private:
    static std::mutex _profiler_mutex;
    static std::string _logfile_path;
    static char *_log_data[LOG_LINES];
    static std::uint32_t _log_index;

    std::string     _name;
    std::string     _tid;
    HRC::time_point _start_time;


    Profiler(const char *name);
};

} // namespace btx

#endif // BRASSTACKS_SYSTEM_PROFILER_HPP