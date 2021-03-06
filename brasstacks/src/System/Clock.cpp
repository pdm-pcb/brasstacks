#include "brasstacks/System/pch.hpp"
#include "brasstacks/System/Clock.hpp"

using std::chrono::microseconds;

namespace btx {

Clock::HRC::time_point Clock::_start_time  = Clock::HRC::now();
Clock::HRC::time_point Clock::_frame_tick  = Clock::_start_time;
Clock::HRC::time_point Clock::_update_tick = Clock::_start_time;

std::atomic<float> Clock::_true_runtime = 0.0f;
std::atomic<float> Clock::_game_runtime = 0.0f;
std::atomic<float> Clock::_update_time  = 0.0f;
std::atomic<float> Clock::_render_time  = 0.0f;
std::atomic<float> Clock::_frame_time   = 0.0f;

void Clock::render_tick() {
    _frame_tick = HRC::now();
}

void Clock::render_tock() {
    _render_time.store(1e-3f * static_cast<float>(
        duration_cast<microseconds>(HRC::now() - _frame_tick).count()
    ));
}

void Clock::frame_time_tock() {
    _frame_time.store(1e-3f * static_cast<float>(
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

void Clock::precise_sleep(double sleep_seconds) {
    static double estimate = 5e-3;
    static double mean = 5e-3;
    static double m2 = 0;
    static int64_t count = 1;

    while (sleep_seconds > estimate) {
        auto start = HRC::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto end = HRC::now();

        double observed = (end - start).count() / 1e9;
        sleep_seconds -= observed;

        ++count;
        double delta = observed - mean;
        mean += delta / count;
        m2   += delta * (observed - mean);
        double stddev = sqrt(m2 / (count - 1));
        estimate = mean + stddev;
    }

    // spin lock
    auto start = HRC::now();
    while ((HRC::now() - start).count() / 1e9 < sleep_seconds);
}

} // namespace btx