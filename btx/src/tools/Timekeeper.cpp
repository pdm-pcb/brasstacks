#include "brasstacks/brasstacks.hpp"
#include "brasstacks/tools/Timekeeper.hpp"

namespace btx {

Timekeeper::TimePoint Timekeeper::_app_start = Timekeeper::SteadyClock::now();
Timekeeper::TimePoint Timekeeper::_frame_start = Timekeeper::_app_start;

uint64_t Timekeeper::_run_time   = 0u;
uint64_t Timekeeper::_frame_time = 0u;

void Timekeeper::update() {
    auto interval = SteadyClock::now() - _app_start;
    _run_time = static_cast<uint64_t>(
        std::chrono::duration_cast<Nanoseconds>(interval).count()
    );
}

void Timekeeper::frame_start() {
    _frame_start = SteadyClock::now();
}

void Timekeeper::frame_end() {
    auto interval = SteadyClock::now() - _frame_start;
    _frame_time = static_cast<uint64_t>(
        std::chrono::duration_cast<Nanoseconds>(interval).count()
    );
}

} // namespace btx