#include "brasstacks/brasstacks.hpp"
#include "brasstacks/tools/TimeKeeper.hpp"

namespace btx {

TimeKeeper::TimePoint TimeKeeper::_app_run_time_start { };
TimeKeeper::TimePoint TimeKeeper::_sim_run_time_start { };

TimeKeeper::TimePoint TimeKeeper::_frame_start    { };
TimeKeeper::TimePoint TimeKeeper::_sim_tick_start { };
TimeKeeper::TimePoint TimeKeeper::_sim_tick_end   { };

std::atomic<uint64_t> TimeKeeper::_app_run_time = 0u;
std::atomic<uint64_t> TimeKeeper::_sim_run_time = 0u;

std::atomic<uint64_t> TimeKeeper::_frame_time     = 0u;
std::atomic<uint64_t> TimeKeeper::_sim_tick_time  = 0u;
std::atomic<uint64_t> TimeKeeper::_sim_tick_delta = 0u;

void TimeKeeper::update_app_run_time() {
    auto const interval = SteadyClock::now() - _app_run_time_start;
    _app_run_time.store(static_cast<uint64_t>(
        std::chrono::duration_cast<Nanoseconds>(interval).count()
    ));
}

void TimeKeeper::frame_start() {
    _frame_start = SteadyClock::now();
}

void TimeKeeper::frame_end() {
    auto const interval = SteadyClock::now() - _frame_start;
    _frame_time.store(static_cast<uint64_t>(
        std::chrono::duration_cast<Nanoseconds>(interval).count()
    ));
}

TimeKeeper::TimePoint const TimeKeeper::sim_tick_start() {
    _sim_tick_start = SteadyClock::now();

    auto const interval = _sim_tick_start - _sim_run_time_start;
    _sim_run_time.store(static_cast<uint64_t>(
        std::chrono::duration_cast<Nanoseconds>(interval).count()
    ));

    return _sim_tick_start;
}

void TimeKeeper::sim_tick_end() {
    auto const tick_end = SteadyClock::now();

    auto const interval = tick_end - _sim_tick_start;
    _sim_tick_time.store(static_cast<uint64_t>(
        std::chrono::duration_cast<Nanoseconds>(interval).count()
    ));

    auto const delta = tick_end - _sim_tick_end;
    _sim_tick_delta.store(static_cast<uint64_t>(
        std::chrono::duration_cast<Nanoseconds>(delta).count()
    ));

    _sim_tick_end = tick_end;
}

} // namespace btx