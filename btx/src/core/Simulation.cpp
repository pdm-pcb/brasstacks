#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/Simulation.hpp"

namespace btx {

Simulation::Simulation(Application &application, uint32_t ticks_per_second) :
    _application    { application },
    _interval_mutex { },
    _tick_interval  { },
    _run_flag       { }
{
    set_ticks_per_second(ticks_per_second);
}

void Simulation::set_ticks_per_second(uint32_t const ticks_per_second) {
    auto const rate = 1.0 / static_cast<double>(ticks_per_second);
    auto const duration = std::chrono::duration<double>(rate);

    {
        std::unique_lock<std::mutex> interval_lock(_interval_mutex);

        _tick_interval =
            std::chrono::duration_cast<TimeKeeper::Nanoseconds>(duration);

        BTX_INFO("Simulation tick: {}hz, every {}", ticks_per_second,
                                                    _tick_interval);
    }
}

void Simulation::start() {
    BTX_TRACE("Starting simulation...");
    _run_flag.test_and_set();
    _run_flag.notify_one();
}

void Simulation::stop() {
    BTX_TRACE("Stopping simulation...");
    _run_flag.clear();
}

void Simulation::run() {
    TimeKeeper::TimePoint tick_start { };
    TimeKeeper::TimePoint next_tick { };

    BTX_TRACE("Simulation ready to run...");
    _run_flag.wait(false);
    BTX_TRACE("Simulation running!");

    TimeKeeper::start_sim_run_time();

    while(_run_flag.test()) {
        tick_start = TimeKeeper::sim_tick_start();

        if(tick_start >= next_tick) {
            _application.update();

            // Find the next step
            {
                std::unique_lock<std::mutex> interval_lock(_interval_mutex);
                next_tick = tick_start + _tick_interval;
            }

            // The work is done
            TimeKeeper::sim_tick_end();
            // BTX_TRACE("Sim tick: {:.09f}", TimeKeeper::sim_tick_time());
        }
        else {
            std::this_thread::yield();
        }
    }
}

} // namespace btx