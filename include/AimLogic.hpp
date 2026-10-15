#pragma once
#include "Config.hpp"

struct MouseDelta { double dx = 0.0; double dy = 0.0; };

struct AimOutput {
    double multiplier = 1.0;
    bool   bypassY = false;
    double distance = 0.0;
    bool   movingToward = false;
};

class AimLogic {
public:
    static AimOutput compute(const Config& cfg,
                             double targetX, double targetY,
                             double crosshairX, double crosshairY,
                             const MouseDelta& mouse);
};
