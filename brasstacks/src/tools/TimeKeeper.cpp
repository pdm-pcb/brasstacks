#include "brasstacks/brasstacks.hpp"
#include "brasstacks/tools/TimeKeeper.hpp"

namespace btx {

uint64_t TimeKeeper::_app_run_time = 0u;
uint64_t TimeKeeper::_delta_time = 0u;

// =============================================================================
void TimeKeeper::update_run_time() {
    static auto last_update = now();

    auto const current_time = now();
    auto const interval =
        static_cast<uint64_t>((current_time - last_update).count());

    _app_run_time += interval;
    _delta_time = interval;

    last_update = current_time;
}

} // namespace btx