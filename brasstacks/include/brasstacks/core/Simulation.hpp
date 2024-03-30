#ifndef BRASSTACKS_CORE_SIMULATION_HPP
#define BRASSTACKS_CORE_SIMULATION_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/tools/TimeKeeper.hpp"

namespace btx {

class Application;

class Simulation {
public:
    static void init(Application *application);
    static void shutdown() { }

    static void run();

    Simulation() = delete;
    ~Simulation() = delete;

    Simulation(Simulation &&) = delete;
    Simulation(Simulation const &) = delete;

    Simulation & operator=(Simulation &&) = delete;
    Simulation & operator=(Simulation const &) = delete;

private:
    static Application *_application;
};

} // namespace btx

#endif // BRASSTACKS_CORE_SIMULATION_HPP