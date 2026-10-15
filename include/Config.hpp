#pragma once
#include <atomic>
#include <string>
#include <cstdint>

struct Config {
    // FOV
    std::atomic<int> max_x{320};
    std::atomic<int> max_y{320};
    std::atomic<int> offset_x{0};
    std::atomic<int> offset_y{0};

    // Tracking toggles
    std::atomic<bool> enable_aim_tracking{false};
    std::atomic<bool> y_axis_freedom{true};
    std::atomic<bool> magnetic_aim_assist{true};

    // Sensitivity / multipliers
    std::atomic<double> head_aim_sensitivity{0.35};
    std::atomic<double> magnetic_acceleration{2.5};
    std::atomic<double> magnetic_correction{0.6};

    // Radial zones
    std::atomic<double> magnetic_lock_radius{8.0};
    std::atomic<double> progressive_slowdown_radius{40.0};
    std::atomic<double> magnetic_acceleration_radius{120.0};
    std::atomic<double> magnetic_friction_radius{160.0};

    // Capture
    std::atomic<int> capture_method{0};

    // Outline detection — yellow
    std::atomic<int> outline_yellow_r{230};
    std::atomic<int> outline_yellow_g{230};
    std::atomic<int> outline_yellow_b{40};

    // Outline detection — cyan
    std::atomic<int> outline_cyan_r{40};
    std::atomic<int> outline_cyan_g{230};
    std::atomic<int> outline_cyan_b{230};

    // Shared tolerance for both colours
    std::atomic<int> outline_tolerance{35};

    // Cluster merge margin
    std::atomic<int> cluster_distance{35};
    std::atomic<int> min_cluster_pixels{5};
    std::atomic<int> min_cluster_width{2};
    std::atomic<int> min_cluster_height{2};

    void loadIni(const std::string& path);
    void saveIni(const std::string& path) const;
    void dump() const;
};
