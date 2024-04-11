#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/Simulation.hpp"

namespace btx {

Application *Simulation::_application;

// =============================================================================
void Simulation::init(Application *application) {
    _application = application;
}

// =============================================================================
void Simulation::run() {
    _application->update();
}

} // namespace btx