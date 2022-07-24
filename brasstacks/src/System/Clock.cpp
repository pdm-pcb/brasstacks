#include "brasstacks/System/pch.hpp"
#include "brasstacks/System/Clock.hpp"

using std::chrono::microseconds;

namespace btx {

Clock::HRC::time_point Clock::_start_time  = Clock::HRC::now();
Clock::HRC::time_point Clock::_frame_tick  = Clock::_start_time;
Clock::HRC::time_point Clock::_update_tick = Clock::_start_time;

std::atomic<float> Clock::_true_runtime = 0.0f;
std::atomic<float> Clock::_game_runtime = 0.0f;
std::atomic<float> Clock::_frame_time   = 0.0f;
std::atomic<float> Clock::_frame_delta  = 0.0f;
std::atomic<float> Clock::_update_time  = 0.0f;

void Clock::frame_tick() {
    _frame_tick = HRC::now();
}

void Clock::frame_tock() {
    _frame_time.store(1e-3f * static_cast<float>(
        duration_cast<microseconds>(HRC::now() - _frame_tick).count()
    ));
}

void Clock::frame_delta_tock() {
    _frame_delta.store(1e-6f * static_cast<float>(
        duration_cast<microseconds>(HRC::now() - _frame_tick).count()
    ));
}

void Clock::update_tick() {
    _update_tick = HRC::now();
}

void Clock::update_tock() {
    _update_time.store(1e-3f * static_cast<float>(
        duration_cast<microseconds>(HRC::now() - _update_tick).count()
    ));
}

void Clock::update() {
    _true_runtime.store(1e-6f * static_cast<float>(
            duration_cast<microseconds>(HRC::now() - _start_time).count()
    ));

    _game_runtime.store(_true_runtime.load(std::memory_order_relaxed));
}

} // namespace btx