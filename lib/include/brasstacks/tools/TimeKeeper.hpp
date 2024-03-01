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

    static void update_run_times();

    static void frame_start();
    static void frame_end();

    static void tick_start();
    static void tick_end();

    static void sim_pause_offset(SteadyClock::duration const &offset);

    inline static auto app_run_time() {
        return 1e-9f * static_cast<float>(_app_run_time.load());
    }

    inline static auto sim_run_time() {
        return 1e-9f * static_cast<float>(_sim_run_time.load());
    }

    inline static auto frame_delta() {
        return 1e-9f * static_cast<float>(_frame_delta.load());
    }

    inline static auto tick_delta() {
        return 1e-9f * static_cast<float>(_tick_delta.load());
    }

    TimeKeeper() = delete;
    ~TimeKeeper() = delete;

    TimeKeeper(TimeKeeper &&other) = delete;
    TimeKeeper(const TimeKeeper &other) = delete;

    TimeKeeper& operator=(TimeKeeper &&other) = delete;
    TimeKeeper& operator=(const TimeKeeper &other) = delete;

private:
    static SteadyClock::time_point _frame_start;
    static SteadyClock::time_point _tick_start;

    static std::atomic<uint64_t> _app_run_time;
    static std::atomic<uint64_t> _sim_run_time;

    static std::atomic<uint64_t> _frame_delta;
    static std::atomic<uint64_t> _tick_delta;

    static uint64_t _last_sim_pause;
};

} // namespace btx

#endif // BRASSTACKS_TOOLS_TIMEKEEPER_HPP