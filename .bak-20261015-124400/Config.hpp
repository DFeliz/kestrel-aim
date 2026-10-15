#pragma once
#include <atomic>
#include <string>
#include <cstdint>

struct Config {
    std::atomic<int> max_x{320};
    std::atomic<int> max_y{320};
    std::atomic<int> offset_x{0};
    std::atomic<int> offset_y{0};
    std::atomic<bool> enable_aim_tracking{false};
    std::atomic<bool> y_axis_freedom{true};
    std::atomic<bool> magnetic_aim_assist{true};
    std::atomic<double> head_aim_sensitivity{0.35};
    std::atomic<double> magnetic_acceleration{2.5};
    std::atomic<double> magnetic_correction{0.6};
    std::atomic<double> magnetic_lock_radius{8.0};
    std::atomic<double> progressive_slowdown_radius{40.0};
    std::atomic<double> magnetic_acceleration_radius{120.0};
    std::atomic<double> magnetic_friction_radius{160.0};
    std::atomic<int> capture_method{0};

    void loadIni(const std::string& path);
    void saveIni(const std::string& path) const;
    void dump() const;
};
