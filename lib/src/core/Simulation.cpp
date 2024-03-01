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
    _loop_running.test_and_set();
    _loop_running.notify_one();

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
    TimeKeeper::TimePoint next_tick { };

    BTX_TRACE("Simulation ready to run...");
    _thread_running.wait(false);
    BTX_TRACE("Simulation running!");

    while(_thread_running.test()) {
        if(!_loop_running.test()) {
            BTX_TRACE("Simulation loop paused...");
            auto const pause_begin = TimeKeeper::now();
            _loop_running.wait(false);

            BTX_TRACE("Simulation loop resumed!");
            TimeKeeper::sim_pause_offset(TimeKeeper::now() - pause_begin);
        }

        auto const now = TimeKeeper::now();

        if(now >= next_tick) {
            // The last tick must now be done
            TimeKeeper::tick_end();

            // And we'll begin the next
            TimeKeeper::tick_start();

            // Do the work
            _application.update();

            // And find the next step
            {
                std::unique_lock<std::mutex> interval_lock(_interval_mutex);
                next_tick = now + _tick_interval;
            }

        }
        else {
            std::this_thread::yield();
        }
    }
}

} // namespace btx