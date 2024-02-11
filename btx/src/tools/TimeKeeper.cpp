#include "brasstacks/brasstacks.hpp"
#include "brasstacks/tools/TimeKeeper.hpp"

namespace btx {

TimeKeeper::TimePoint TimeKeeper::_app_start_time { TimeKeeper::now() };

TimeKeeper::TimePoint TimeKeeper::_frame_start    { };
TimeKeeper::TimePoint TimeKeeper::_sim_tick_start { };
TimeKeeper::TimePoint TimeKeeper::_sim_tick_end   { };

std::atomic<uint64_t> TimeKeeper::_app_run_time = 0u;
std::atomic<uint64_t> TimeKeeper::_sim_run_time = 0u;

std::atomic<uint64_t> TimeKeeper::_frame_delta    = 0u;
std::atomic<uint64_t> TimeKeeper::_sim_tick_delta = 0u;

// =============================================================================
void TimeKeeper::update_app_run_time() {
    auto const interval = now() - _app_start_time;
    _app_run_time.store(static_cast<uint64_t>(interval.count()));
}

// =============================================================================
void TimeKeeper::frame_start() {
    _frame_start = now();
}

// =============================================================================
void TimeKeeper::frame_end() {
    auto const interval = now() - _frame_start;
    _frame_delta.store(static_cast<uint64_t>(interval.count()));
}

// =============================================================================
void TimeKeeper::sim_tick_end() {
    auto const tick_end = now();
    auto const duration = tick_end - _sim_tick_end;
    auto const delta = static_cast<uint64_t>(duration.count());

    _sim_tick_delta.store(delta);
    _sim_run_time.store(_sim_run_time.load() + delta);

    _sim_tick_end = tick_end;
}

// =============================================================================
void TimeKeeper::sim_pause_offset(SteadyClock::duration const &offset) {
    auto const offset_ns = static_cast<uint64_t>(offset.count());

    BTX_TRACE("Offsetting simulation run time by -{}ns", offset_ns);
    if(offset_ns > _sim_run_time.load()) {
        _sim_run_time.store(0);
    }
    else {
        _sim_run_time.store(_sim_run_time.load() - offset_ns);
    }
    BTX_TRACE("New run time: {}ns", _sim_run_time.load());
}

} // namespace btx