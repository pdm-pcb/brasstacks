#include "brasstacks/brasstacks.hpp"
#include "brasstacks/tools/Timekeeper.hpp"

namespace btx {

Timekeeper::TimePoint Timekeeper::_app_start   { };
Timekeeper::TimePoint Timekeeper::_sim_start   { };
Timekeeper::TimePoint Timekeeper::_frame_start { };

std::atomic<uint64_t> Timekeeper::_app_run_time = 0u;
std::atomic<uint64_t> Timekeeper::_sim_run_time = 0u;
std::atomic<uint64_t> Timekeeper::_frame_time   = 0u;

void Timekeeper::update_app_run_time() {
    auto interval = SteadyClock::now() - _app_start;
    _app_run_time.store(static_cast<uint64_t>(
        std::chrono::duration_cast<Nanoseconds>(interval).count()
    ));
}

void Timekeeper::update_sim_run_time() {
    auto interval = SteadyClock::now() - _sim_start;
    _sim_run_time.store(static_cast<uint64_t>(
        std::chrono::duration_cast<Nanoseconds>(interval).count()
    ));
}

void Timekeeper::frame_start() {
    _frame_start = SteadyClock::now();
}

void Timekeeper::frame_end() {
    auto interval = SteadyClock::now() - _frame_start;
    _frame_time.store(static_cast<uint64_t>(
        std::chrono::duration_cast<Nanoseconds>(interval).count()
    ));
}

} // namespace btx