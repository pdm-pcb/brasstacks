#include "brasstacks/brasstacks.hpp"
#include "brasstacks/tools/TimeKeeper.hpp"

namespace btx {

TimeKeeper::TimePoint TimeKeeper::_frame_start   { };
TimeKeeper::TimePoint TimeKeeper::_tick_start    { };

std::atomic<uint64_t> TimeKeeper::_app_run_time = 0u;
std::atomic<uint64_t> TimeKeeper::_sim_run_time = 0u;

std::atomic<uint64_t> TimeKeeper::_frame_delta = 0u;
std::atomic<uint64_t> TimeKeeper::_tick_delta  = 0u;

uint64_t TimeKeeper::_last_render_pause = 0u;
uint64_t TimeKeeper::_last_sim_pause    = 0u;

// =============================================================================
void TimeKeeper::update_run_times() {
    static auto last_update = now();

    auto const current_time = now();
    auto const interval = (current_time - last_update).count();

    _app_run_time.store(_app_run_time.load() + static_cast<uint64_t>(interval));
    _sim_run_time.store(_sim_run_time.load() + static_cast<uint64_t>(interval));

    last_update = current_time;
}

// =============================================================================
void TimeKeeper::frame_start() {
    _frame_start = now();
}

// =============================================================================
void TimeKeeper::frame_end() {
    auto const interval = static_cast<uint64_t>((now() - _frame_start).count());
    _frame_delta.store(interval - _last_render_pause);
    _last_render_pause = 0u;
}

// =============================================================================
void TimeKeeper::tick_start() {
    _tick_start = now();
}

// =============================================================================
void TimeKeeper::tick_end() {
    auto const interval = static_cast<uint64_t>((now() - _tick_start).count());
    _tick_delta.store(interval - _last_sim_pause);
    _last_sim_pause = 0u;
}

// =============================================================================
void TimeKeeper::render_pause_offset(SteadyClock::duration const &offset) {
    _last_render_pause = static_cast<uint64_t>(offset.count());
}

// =============================================================================
void TimeKeeper::sim_pause_offset(SteadyClock::duration const &offset) {
    _last_sim_pause = static_cast<uint64_t>(offset.count());

    if(_last_sim_pause > _sim_run_time.load()) {
        _sim_run_time.store(0);
    }
    else {
        _sim_run_time.store(_sim_run_time.load() - _last_sim_pause);
    }
}

} // namespace btx