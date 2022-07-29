#ifndef BRASSTACKS_SYSTEM_CLOCK_HPP
#define BRASSTACKS_SYSTEM_CLOCK_HPP

#include <chrono>
#include <atomic>

namespace btx {

class Clock final {
public:
    using HRC = std::chrono::high_resolution_clock;

    static void render_tick();
    static void render_tock();
    static void frame_time_tock();
    static void update_tick();
    static void update_tock();

    static void update();

    static float runtime()      { return _game_runtime.load();         }
    static float true_runtime() { return _true_runtime.load();         }
    static float render_time()  { return _render_time.load();          }
    static float frame_time()   { return _frame_time.load();           }
    static float update_time()  { return _update_time.load();          }
    static float frame_delta()  { return _frame_time.load() * 0.001f; }

    static HRC::time_point now() { return HRC::now(); }
    static void precise_sleep(double sleep_seconds);

    Clock()  = delete;
    ~Clock() = delete;

    Clock(const Clock &&) = delete;
    Clock(Clock &)        = delete;

    Clock & operator=(const Clock &&) = delete;
    Clock & operator=(Clock &)        = delete;

private:
    static HRC::time_point _start_time;
    static HRC::time_point _frame_tick;
    static HRC::time_point _update_tick;

    static std::atomic<float> _true_runtime;
    static std::atomic<float> _game_runtime;
    static std::atomic<float> _update_time;
    static std::atomic<float> _render_time;
    static std::atomic<float> _frame_time;
};

} // namespace btx

#endif // BRASSTACKS_SYSTEM_CLOCK_HPP