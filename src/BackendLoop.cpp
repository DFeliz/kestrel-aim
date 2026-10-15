#include "BackendLoop.hpp"
#include "GameCapture.hpp"
#include "ColorDetection.hpp"
#include "AimLogic.hpp"
#include <chrono>
#include <thread>
#include <memory>
#include <cmath>

struct SensitivityState { void updateDynamic(double, bool){} };
extern SensitivityState* g_sensState;
extern void getLatestMouseDelta(double& dx, double& dy);

void BackendLoop(Config& cfg,
                 std::atomic<bool>& shutdown,
                 std::atomic<double>& outFps,
                 std::atomic<int>& outTargets,
                 std::atomic<bool>& outLocked){
    int lastFovW=-1, lastFovH=-1, lastOffX=0, lastOffY=0;
    std::unique_ptr<GameCapture> capture;
    using clock = std::chrono::steady_clock;
    auto last = clock::now();
    int frames = 0;

    while(!shutdown.load(std::memory_order_relaxed)){
        if(!cfg.enable_aim_tracking.load()){
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            continue;
        }
        int fovW=cfg.max_x.load(), fovH=cfg.max_y.load();
        int offX=cfg.offset_x.load(), offY=cfg.offset_y.load();
        if(!capture || fovW!=lastFovW || fovH!=lastFovH || offX!=lastOffX || offY!=lastOffY){
            capture.reset();
            capture = std::make_unique<GameCapture>(fovW, fovH, offX, offY);
            lastFovW=fovW; lastFovH=fovH; lastOffX=offX; lastOffY=offY;
            if(!capture->ok()){
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                continue;
            }
        }
        FrameView fv = capture->get_frame();
        if(!fv.valid){ std::this_thread::sleep_for(std::chrono::milliseconds(1)); continue; }

        TargetResult tr = ColorDetection::detect(fv, 35, 5, 3, 3);
        outTargets.store(tr.clusterCount);

        if(tr.found && g_sensState){
            double crossX = (double)fv.width * 0.5;
            double crossY = (double)fv.height * 0.5;
            MouseDelta md{};
            getLatestMouseDelta(md.dx, md.dy);
            AimOutput ao = AimLogic::compute(cfg,
                (double)tr.headX, (double)tr.headY, crossX, crossY, md);
            g_sensState->updateDynamic(ao.multiplier, ao.bypassY);
            outLocked.store(ao.distance < cfg.magnetic_lock_radius.load());
        } else {
            outLocked.store(false);
            if(g_sensState) g_sensState->updateDynamic(1.0, false);
        }

        ++frames;
        auto now = clock::now();
        double dt = std::chrono::duration<double>(now - last).count();
        if(dt >= 0.5){ outFps.store((double)frames/dt); frames=0; last=now; }
    }
}
