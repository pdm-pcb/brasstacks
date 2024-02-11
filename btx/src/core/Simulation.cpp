#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/Simulation.hpp"

namespace btx {

// =============================================================================
Simulation::Simulation(Application &application, uint32_t ticks_per_second) :
    _application    { application },
    _interval_mutex { },
    _tick_interval  { },
    _thread_running { },
    _loop_running   { }
{
    set_ticks_per_second(ticks_per_second);

    _thread_running.clear();
    _loop_running.clear();
}

// =============================================================================
void Simulation::set_ticks_per_second(uint32_t const ticks_per_second) {
    auto const rate = 1.0 / static_cast<double>(ticks_per_second);
    auto const duration = std::chrono::duration<double>(rate);

    {
        std::unique_lock<std::mutex> interval_lock(_interval_mutex);

        _tick_interval =
            std::chrono::duration_cast<TimeKeeper::Nanoseconds>(duration);

        BTX_INFO("Simulation tick: {}hz, every {}ns", ticks_per_second,
                                                      _tick_interval.count());
    }
}

// =============================================================================
void Simulation::start_thread() {
    BTX_TRACE("Starting simulation thread...");
    _thread_running.test_and_set();
    _thread_running.notify_one();
}

// =============================================================================
void Simulation::stop_thread() {
    BTX_TRACE("Stopping simulation thread...");

    // First, make sure the loop can proceed
    _loop_running.test_and_set();
    _loop_running.notify_one();

    // Next, allow the loop to exit
    _thread_running.clear();
}

// =============================================================================
void Simulation::toggle_loop() {
    BTX_TRACE("Toggling simulation loop...");
    if(!_loop_running.test_and_set()) {
        _loop_running.notify_one();
    }
    else {
        _loop_running.clear();
    }
}

// =============================================================================
void Simulation::run() {
    TimeKeeper::TimePoint tick_start { };
    TimeKeeper::TimePoint next_tick { };

    BTX_TRACE("Simulation ready to run...");
    _thread_running.wait(false);
    BTX_TRACE("Simulation running!");

    while(_thread_running.test()) {
        if(!_loop_running.test()) {
            BTX_TRACE("Simulation loop paused...");
            auto const pause_begin = TimeKeeper::now();
            _loop_running.wait(false);

            BTX_TRACE("Simulation loop playing!");
            TimeKeeper::sim_pause_offset(TimeKeeper::now() - pause_begin);
        }

        tick_start = TimeKeeper::now();

        if(tick_start >= next_tick) {
            _application.update();

            // Find the next step
            {
                std::unique_lock<std::mutex> interval_lock(_interval_mutex);
                next_tick = tick_start + _tick_interval;
            }

            // The work is done
            TimeKeeper::sim_tick_end();
        }
        else {
            std::this_thread::yield();
        }
    }
}

} // namespace btx