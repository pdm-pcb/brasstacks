#ifndef BRASSTACKS_TOOLS_TIMEKEEPER_HPP
#define BRASSTACKS_TOOLS_TIMEKEEPER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class Timekeeper final {
public:
    using SteadyClock = std::chrono::steady_clock;
    using TimePoint = SteadyClock::time_point;
    using Nanoseconds = std::chrono::nanoseconds;

    static void app_start() { _app_start = SteadyClock::now(); }
    static void sim_start() { _sim_start = SteadyClock::now(); }

    static void update_app_run_time();
    static void update_sim_run_time();

    static void frame_start();
    static void frame_end();

    inline static auto app_run_time() {
        return 1e-9f * static_cast<float>(_app_run_time.load());
    }

    inline static auto sim_run_time() {
        return 1e-9f * static_cast<float>(_sim_run_time.load());
    }

    inline static auto frame_time() {
        return 1e-9f * static_cast<float>(_frame_time.load());
    }

    inline static auto now() { return SteadyClock::now(); }

    Timekeeper() = delete;
    ~Timekeeper() = delete;

    Timekeeper(Timekeeper &&other) = delete;
    Timekeeper(const Timekeeper &other) = delete;

    Timekeeper& operator=(Timekeeper &&other) = delete;
    Timekeeper& operator=(const Timekeeper &other) = delete;

private:
    static SteadyClock::time_point _app_start;
    static SteadyClock::time_point _sim_start;
    static SteadyClock::time_point _frame_start;

    static std::atomic<uint64_t> _app_run_time;
    static std::atomic<uint64_t> _sim_run_time;
    static std::atomic<uint64_t> _frame_time;
};

} // namespace btx

#endif // BRASSTACKS_TOOLS_TIMEKEEPER_HPP