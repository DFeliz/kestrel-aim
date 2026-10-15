#include "AimLogic.hpp"
#include <cmath>
#include <algorithm>

AimOutput AimLogic::compute(const Config& cfg,
                            double targetX, double targetY,
                            double crosshairX, double crosshairY,
                            const MouseDelta& mouse){
    AimOutput out{};
    double vx = targetX - crosshairX;
    double vy = targetY - crosshairY;
    double dist = std::sqrt(vx*vx + vy*vy);
    out.distance = dist;

    if(!cfg.magnetic_aim_assist.load()){ out.multiplier = 1.0; return out; }

    double lockR  = cfg.magnetic_lock_radius.load();
    double slowR  = cfg.progressive_slowdown_radius.load();
    double accR   = cfg.magnetic_acceleration_radius.load();
    double fricR  = cfg.magnetic_friction_radius.load();
    double headSens = cfg.head_aim_sensitivity.load();
    double maxAccel = cfg.magnetic_acceleration.load();
    double corr     = cfg.magnetic_correction.load();

    double mdx = mouse.dx, mdy = mouse.dy;
    double mlen = std::sqrt(mdx*mdx + mdy*mdy);
    double dot = 0.0;
    if(mlen > 1e-9 && dist > 1e-9)
        dot = (mdx*vx + mdy*vy) / (mlen*dist);
    out.movingToward = (dot > 0.0);

    double mult = 1.0;
    if(dist < lockR){
        mult = headSens;
    } else if(dist < slowR){
        double t = (dist - lockR) / std::max(1e-9, (slowR - lockR));
        double curve = std::pow(t, 2.0);
        mult = headSens + (1.0 - headSens) * curve;
    } else if(dist < accR){
        if(out.movingToward && mlen > 1e-9){
            double t = (dist - slowR) / std::max(1e-9, (accR - slowR));
            mult = 1.0 + (maxAccel - 1.0) * t;
        } else mult = 1.0;
    } else if(dist < fricR){
        if(!out.movingToward && mlen > 1e-9){
            double inv = std::clamp(-dot, 0.0, 1.0);
            mult = corr * std::exp(-3.0 * inv);
            if(mult < 0.05) mult = 0.05;
        } else mult = 1.0;
    }

    bool bypassY = false;
    if(cfg.y_axis_freedom.load() && dist < slowR &&
       std::abs(vx) < std::abs(vy)*0.5 && mouse.dy > 0.0)
        bypassY = true;

    out.multiplier = mult;
    out.bypassY = bypassY;
    return out;
}
