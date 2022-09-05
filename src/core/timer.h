#pragma once

#include <chrono>

namespace phrosty {

    using ClockTime = decltype(std::chrono::high_resolution_clock::now());

    ClockTime inline current_time() { return std::chrono::high_resolution_clock::now(); }

    long inline timediff_msec(const ClockTime& new_time, const ClockTime& old_time) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(new_time - old_time).count();
    }
}
