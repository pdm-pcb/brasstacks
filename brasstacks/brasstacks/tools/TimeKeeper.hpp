#ifndef BRASSTACKS_TOOLS_TIMEKEEPER_HPP
#define BRASSTACKS_TOOLS_TIMEKEEPER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class TimeKeeper final {
public:
    using SteadyClock = std::chrono::steady_clock;
    using TimePoint   = SteadyClock::time_point;
    using Nanoseconds = std::chrono::nanoseconds;

    inline static auto now() { return SteadyClock::now(); }

    static void update_run_time();

    inline static auto delta_time() {
        return 1e-9f * static_cast<float>(_delta_time);
    }

    inline static auto app_run_time() {
        return 1e-9f * static_cast<float>(_app_run_time);
    }

    TimeKeeper() = delete;
    ~TimeKeeper() = delete;

    TimeKeeper(TimeKeeper &&other) = delete;
    TimeKeeper(const TimeKeeper &other) = delete;

    TimeKeeper& operator=(TimeKeeper &&other) = delete;
    TimeKeeper& operator=(const TimeKeeper &other) = delete;

private:
    static uint64_t _app_run_time;
    static uint64_t _delta_time;
};

} // namespace btx

#endif // BRASSTACKS_TOOLS_TIMEKEEPER_HPP