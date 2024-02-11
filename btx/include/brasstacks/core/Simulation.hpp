#ifndef BRASSTACKS_CORE_SIMULATION_HPP
#define BRASSTACKS_CORE_SIMULATION_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/tools/TimeKeeper.hpp"

namespace btx {

class Application;

class Simulation {
public:
    Simulation(Application &application, uint32_t ticks_per_second);
    ~Simulation() = default;

    void set_ticks_per_second(uint32_t const ticks_per_second);

    void start();
    void stop();
    void run();

    Simulation() = delete;

    Simulation(Simulation &&) = delete;
    Simulation(Simulation const &) = delete;

    Simulation & operator=(Simulation &&) = delete;
    Simulation & operator=(Simulation const &) = delete;

private:
    Application &_application;

    std::mutex _interval_mutex;
    TimeKeeper::Nanoseconds _tick_interval;

    std::atomic_flag _run_flag;
};

} // namespace btx

#endif // BRASSTACKS_CORE_SIMULATION_HPP