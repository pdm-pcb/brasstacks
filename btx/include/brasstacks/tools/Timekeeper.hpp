#ifndef BRASSTACKS_TOOLS_TIMEKEEPER_HPP
#define BRASSTACKS_TOOLS_TIMEKEEPER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class Timekeeper final {
public:
    using HRC = std::chrono::high_resolution_clock;
    using TimePoint = HRC::time_point;
    using Microseconds = std::chrono::microseconds;

    static void update();

    static void frame_start();
    static void frame_end();

    inline static auto run_time() {
        return 1e-6f * static_cast<float>(_run_time);
    }

    inline static auto frame_time() {
        return 1e-6f * static_cast<float>(_frame_time);
    }

    inline static auto now() { return HRC::now(); }

    Timekeeper() = delete;
    ~Timekeeper() = delete;

    Timekeeper(Timekeeper &&other) = delete;
    Timekeeper(const Timekeeper &other) = delete;

    Timekeeper& operator=(Timekeeper &&other) = delete;
    Timekeeper& operator=(const Timekeeper &other) = delete;

private:
    static HRC::time_point _app_start;
    static HRC::time_point _frame_start;

    static uint64_t _run_time;
    static uint64_t _frame_time;
};

} // namespace btx

#endif // BRASSTACKS_TOOLS_TIMEKEEPER_HPP