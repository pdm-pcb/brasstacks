#ifndef BRASSTACKS_CORE_SIMULATION_HPP
#define BRASSTACKS_CORE_SIMULATION_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/tools/TimeKeeper.hpp"

namespace btx {

class Application;

class Simulation {
public:
    Simulation(Application &application);
    ~Simulation() = default;

    void run();

    Simulation() = delete;

    Simulation(Simulation &&) = delete;
    Simulation(Simulation const &) = delete;

    Simulation & operator=(Simulation &&) = delete;
    Simulation & operator=(Simulation const &) = delete;

private:
    Application &_application;
};

} // namespace btx

#endif // BRASSTACKS_CORE_SIMULATION_HPP