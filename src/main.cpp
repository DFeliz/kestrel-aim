#include "Config.hpp"
#include "BackendLoop.hpp"
#include <atomic>
#include <thread>
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <cstdio>
#include <windows.h>

#include <cstdio>
struct SensitivityState {
    void updateDynamic(double m, bool b){
        std::fprintf(stderr, "\rmult=%.3f y=%d   ", m, b?1:0);
    }
};
SensitivityState* g_sensState = new SensitivityState();

void getLatestMouseDelta(double& dx, double& dy){
    static double px=0.0, py=0.0;
    POINT p{}; GetCursorPos(&p);
    dx = (double)p.x - px;
    dy = (double)p.y - py;
    px = (double)p.x;
    py = (double)p.y;
}

static void printHelp(){
    std::cout <<
        "commands:\n"
        "  set <key> <value>\n"
        "  show\n"
        "  save [path]\n"
        "  load [path]\n"
        "  on / off\n"
        "  fov <w> <h>\n"
        "  quit\n";
}

int main(){
    Config cfg;
    cfg.loadIni("config.ini");

    std::atomic<bool> shutdown{false};
    std::atomic<double> fps{0.0};
    std::atomic<int> targets{0};
    std::atomic<bool> locked{false};

    std::thread backend(BackendLoop, std::ref(cfg), std::ref(shutdown),
        std::ref(fps), std::ref(targets), std::ref(locked));
    backend.detach();

    std::thread telemetry([&]{
        while(!shutdown.load()){
            std::printf("\r[fps=%6.1f] [targets=%2d] [locked=%s]      ",
                fps.load(), targets.load(), locked.load() ? "Y" : "N");
            std::fflush(stdout);
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
    });
    telemetry.detach();

    printHelp();

    std::string line;
    while(std::getline(std::cin, line)){
        std::istringstream ss(line);
        std::string cmd; ss >> cmd;
        if(cmd.empty()) continue;

        if(cmd == "quit" || cmd == "exit"){
            shutdown.store(true);
            break;
        } else if(cmd == "help"){
            printHelp();
        } else if(cmd == "show"){
            cfg.dump();
        } else if(cmd == "save"){
            std::string p = "config.ini"; ss >> p;
            cfg.saveIni(p);
            std::cout << "saved " << p << "\n";
        } else if(cmd == "load"){
            std::string p = "config.ini"; ss >> p;
            cfg.loadIni(p);
            std::cout << "loaded " << p << "\n";
        } else if(cmd == "on"){
            cfg.enable_aim_tracking = true;
            std::cout << "tracking ON\n";
        } else if(cmd == "off"){
            cfg.enable_aim_tracking = false;
            std::cout << "tracking OFF\n";
        } else if(cmd == "fov"){
            int w=0,h=0; ss >> w >> h;
            if(w>0 && h>0){ cfg.max_x=w; cfg.max_y=h; std::cout << "fov " << w << "x" << h << "\n"; }
        } else if(cmd == "set"){
            std::string k, v; ss >> k >> v;
            if(k.empty()) continue;
            int iv=0; double dv=0.0;
            try{ iv=std::stoi(v); }catch(...){}
            try{ dv=std::stod(v); }catch(...){}
            bool bv = (v=="1"||v=="true"||v=="on");
            if(k=="max_x") cfg.max_x=iv;
            else if(k=="max_y") cfg.max_y=iv;
            else if(k=="offset_x") cfg.offset_x=iv;
            else if(k=="offset_y") cfg.offset_y=iv;
            else if(k=="enable_aim_tracking") cfg.enable_aim_tracking=bv;
            else if(k=="y_axis_freedom") cfg.y_axis_freedom=bv;
            else if(k=="magnetic_aim_assist") cfg.magnetic_aim_assist=bv;
            else if(k=="head_aim_sensitivity") cfg.head_aim_sensitivity=dv;
            else if(k=="magnetic_acceleration") cfg.magnetic_acceleration=dv;
            else if(k=="magnetic_correction") cfg.magnetic_correction=dv;
            else if(k=="magnetic_lock_radius") cfg.magnetic_lock_radius=dv;
            else if(k=="progressive_slowdown_radius") cfg.progressive_slowdown_radius=dv;
            else if(k=="magnetic_acceleration_radius") cfg.magnetic_acceleration_radius=dv;
            else if(k=="magnetic_friction_radius") cfg.magnetic_friction_radius=dv;
            else if(k=="outline_yellow_r") cfg.outline_yellow_r=iv;
            else if(k=="outline_yellow_g") cfg.outline_yellow_g=iv;
            else if(k=="outline_yellow_b") cfg.outline_yellow_b=iv;
            else if(k=="outline_cyan_r") cfg.outline_cyan_r=iv;
            else if(k=="outline_cyan_g") cfg.outline_cyan_g=iv;
            else if(k=="outline_cyan_b") cfg.outline_cyan_b=iv;
            else if(k=="outline_tolerance") cfg.outline_tolerance=iv;
            else if(k=="cluster_distance") cfg.cluster_distance=iv;
            else if(k=="min_cluster_pixels") cfg.min_cluster_pixels=iv;
            else if(k=="min_cluster_width") cfg.min_cluster_width=iv;
            else if(k=="min_cluster_height") cfg.min_cluster_height=iv;
            else std::cout << "unknown key\n";
        } else {
            std::cout << "unknown command\n";
        }
    }

    shutdown.store(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::cout << "\nshutdown\n";
    return 0;
}

