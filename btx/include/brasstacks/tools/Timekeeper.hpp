#ifndef BRASSTACKS_TOOLS_TIMEKEEPER_HPP
#define BRASSTACKS_TOOLS_TIMEKEEPER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class TimeKeeper final {
public:
    using SteadyClock = std::chrono::steady_clock;
    using TimePoint   = SteadyClock::time_point;
    using Nanoseconds = std::chrono::nanoseconds;

    inline static void start_app_run_time() {
        _app_run_time_start = SteadyClock::now();
    }

    inline static void start_sim_run_time() {
        _sim_run_time_start = SteadyClock::now();
    }

    static void update_app_run_time();

    static void frame_start();
    static void frame_end();

    static TimePoint const sim_tick_start();
    static void sim_tick_end();

    inline static auto now() { return SteadyClock::now(); }

    inline static auto app_run_time() {
        return 1e-9f * static_cast<float>(_app_run_time.load());
    }

    inline static auto sim_run_time() {
        return 1e-9f * static_cast<float>(_sim_run_time.load());
    }

    inline static auto frame_time() {
        return 1e-9f * static_cast<float>(_frame_time.load());
    }

    inline static auto sim_tick_time() {
        return 1e-9f * static_cast<float>(_sim_tick_time.load());
    }

    inline static auto sim_tick_delta() {
        return 1e-9f * static_cast<float>(_sim_tick_delta.load());
    }

    TimeKeeper() = delete;
    ~TimeKeeper() = delete;

    TimeKeeper(TimeKeeper &&other) = delete;
    TimeKeeper(const TimeKeeper &other) = delete;

    TimeKeeper& operator=(TimeKeeper &&other) = delete;
    TimeKeeper& operator=(const TimeKeeper &other) = delete;

private:
    static SteadyClock::time_point _app_run_time_start;
    static SteadyClock::time_point _sim_run_time_start;

    static SteadyClock::time_point _frame_start;
    static SteadyClock::time_point _sim_tick_start;
    static SteadyClock::time_point _sim_tick_end;

    static std::atomic<uint64_t> _app_run_time;
    static std::atomic<uint64_t> _sim_run_time;

    static std::atomic<uint64_t> _frame_time;
    static std::atomic<uint64_t> _sim_tick_time;
    static std::atomic<uint64_t> _sim_tick_delta;
};

} // namespace btx

#endif // BRASSTACKS_TOOLS_TIMEKEEPER_HPP