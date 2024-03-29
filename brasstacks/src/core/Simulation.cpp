#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/Simulation.hpp"

namespace btx {

// =============================================================================
Simulation::Simulation(Application &application) :
    _application    { application }
{ }

// =============================================================================
void Simulation::run() {
    _application.update();
}

} // namespace btx