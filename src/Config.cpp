#include "Config.hpp"
#include <fstream>
#include <iostream>
#include <cctype>
#include <unordered_map>

namespace {
    std::unordered_map<std::string,std::string> parseIni(const std::string& path){
        std::unordered_map<std::string,std::string> kv;
        std::ifstream f(path);
        if(!f) return kv;
        std::string line;
        while(std::getline(f,line)){
            if(line.empty() || line[0]==';' || line[0]=='#') continue;
            auto eq = line.find('=');
            if(eq==std::string::npos) continue;
            std::string k = line.substr(0,eq);
            std::string v = line.substr(eq+1);
            auto trim=[](std::string& s){
                while(!s.empty() && std::isspace((unsigned char)s.front())) s.erase(s.begin());
                while(!s.empty() && std::isspace((unsigned char)s.back())) s.pop_back();
            };
            trim(k); trim(v);
            kv[k]=v;
        }
        return kv;
    }
    int    toI(const std::string& s,int d){try{return std::stoi(s);}catch(...){return d;}}
    double toD(const std::string& s,double d){try{return std::stod(s);}catch(...){return d;}}
    bool   toB(const std::string& s,bool d){
        if(s=="1"||s=="true"||s=="TRUE"||s=="on") return true;
        if(s=="0"||s=="false"||s=="FALSE"||s=="off") return false;
        return d;
    }
}

void Config::loadIni(const std::string& path){
    auto kv = parseIni(path);
    auto get=[&](const char* k)->const std::string*{
        auto it=kv.find(k); return it==kv.end()?nullptr:&it->second;
    };
    if(auto v=get("max_x")) max_x=toI(*v,max_x.load());
    if(auto v=get("max_y")) max_y=toI(*v,max_y.load());
    if(auto v=get("offset_x")) offset_x=toI(*v,offset_x.load());
    if(auto v=get("offset_y")) offset_y=toI(*v,offset_y.load());
    if(auto v=get("enable_aim_tracking")) enable_aim_tracking=toB(*v,enable_aim_tracking.load());
    if(auto v=get("y_axis_freedom")) y_axis_freedom=toB(*v,y_axis_freedom.load());
    if(auto v=get("magnetic_aim_assist")) magnetic_aim_assist=toB(*v,magnetic_aim_assist.load());
    if(auto v=get("head_aim_sensitivity")) head_aim_sensitivity=toD(*v,head_aim_sensitivity.load());
    if(auto v=get("magnetic_acceleration")) magnetic_acceleration=toD(*v,magnetic_acceleration.load());
    if(auto v=get("magnetic_correction")) magnetic_correction=toD(*v,magnetic_correction.load());
    if(auto v=get("magnetic_lock_radius")) magnetic_lock_radius=toD(*v,magnetic_lock_radius.load());
    if(auto v=get("progressive_slowdown_radius")) progressive_slowdown_radius=toD(*v,progressive_slowdown_radius.load());
    if(auto v=get("magnetic_acceleration_radius")) magnetic_acceleration_radius=toD(*v,magnetic_acceleration_radius.load());
    if(auto v=get("magnetic_friction_radius")) magnetic_friction_radius=toD(*v,magnetic_friction_radius.load());
    if(auto v=get("capture_method")) capture_method=toI(*v,capture_method.load());
    if(auto v=get("outline_yellow_r")) outline_yellow_r=toI(*v,outline_yellow_r.load());
    if(auto v=get("outline_yellow_g")) outline_yellow_g=toI(*v,outline_yellow_g.load());
    if(auto v=get("outline_yellow_b")) outline_yellow_b=toI(*v,outline_yellow_b.load());
    if(auto v=get("outline_cyan_r")) outline_cyan_r=toI(*v,outline_cyan_r.load());
    if(auto v=get("outline_cyan_g")) outline_cyan_g=toI(*v,outline_cyan_g.load());
    if(auto v=get("outline_cyan_b")) outline_cyan_b=toI(*v,outline_cyan_b.load());
    if(auto v=get("outline_tolerance")) outline_tolerance=toI(*v,outline_tolerance.load());
    if(auto v=get("cluster_distance")) cluster_distance=toI(*v,cluster_distance.load());
    if(auto v=get("min_cluster_pixels")) min_cluster_pixels=toI(*v,min_cluster_pixels.load());
    if(auto v=get("min_cluster_width")) min_cluster_width=toI(*v,min_cluster_width.load());
    if(auto v=get("min_cluster_height")) min_cluster_height=toI(*v,min_cluster_height.load());
}

void Config::saveIni(const std::string& path) const {
    std::ofstream f(path);
    if(!f) return;
    f << "max_x=" << max_x.load() << "\n";
    f << "max_y=" << max_y.load() << "\n";
    f << "offset_x=" << offset_x.load() << "\n";
    f << "offset_y=" << offset_y.load() << "\n";
    f << "enable_aim_tracking=" << (enable_aim_tracking.load()?1:0) << "\n";
    f << "y_axis_freedom=" << (y_axis_freedom.load()?1:0) << "\n";
    f << "magnetic_aim_assist=" << (magnetic_aim_assist.load()?1:0) << "\n";
    f << "head_aim_sensitivity=" << head_aim_sensitivity.load() << "\n";
    f << "magnetic_acceleration=" << magnetic_acceleration.load() << "\n";
    f << "magnetic_correction=" << magnetic_correction.load() << "\n";
    f << "magnetic_lock_radius=" << magnetic_lock_radius.load() << "\n";
    f << "progressive_slowdown_radius=" << progressive_slowdown_radius.load() << "\n";
    f << "magnetic_acceleration_radius=" << magnetic_acceleration_radius.load() << "\n";
    f << "magnetic_friction_radius=" << magnetic_friction_radius.load() << "\n";
    f << "capture_method=" << capture_method.load() << "\n";
    f << "outline_yellow_r=" << outline_yellow_r.load() << "\n";
    f << "outline_yellow_g=" << outline_yellow_g.load() << "\n";
    f << "outline_yellow_b=" << outline_yellow_b.load() << "\n";
    f << "outline_cyan_r=" << outline_cyan_r.load() << "\n";
    f << "outline_cyan_g=" << outline_cyan_g.load() << "\n";
    f << "outline_cyan_b=" << outline_cyan_b.load() << "\n";
    f << "outline_tolerance=" << outline_tolerance.load() << "\n";
    f << "cluster_distance=" << cluster_distance.load() << "\n";
    f << "min_cluster_pixels=" << min_cluster_pixels.load() << "\n";
    f << "min_cluster_width=" << min_cluster_width.load() << "\n";
    f << "min_cluster_height=" << min_cluster_height.load() << "\n";
}

void Config::dump() const {
    std::cout << "[cfg] fov=" << max_x.load() << "x" << max_y.load()
              << " off=(" << offset_x.load() << "," << offset_y.load() << ")\n"
              << "      track=" << enable_aim_tracking.load()
              << " yfree=" << y_axis_freedom.load()
              << " mag=" << magnetic_aim_assist.load() << "\n"
              << "      sens=" << head_aim_sensitivity.load()
              << " accel=" << magnetic_acceleration.load()
              << " corr=" << magnetic_correction.load() << "\n"
              << "      lock=" << magnetic_lock_radius.load()
              << " slow=" << progressive_slowdown_radius.load()
              << " accR=" << magnetic_acceleration_radius.load()
              << " fricR=" << magnetic_friction_radius.load() << "\n"
              << "      outlineY=(" << outline_yellow_r.load() << ","
              << outline_yellow_g.load() << "," << outline_yellow_b.load() << ")"
              << " outlineC=(" << outline_cyan_r.load() << ","
              << outline_cyan_g.load() << "," << outline_cyan_b.load() << ")"
              << " tol=" << outline_tolerance.load() << "\n";
}
