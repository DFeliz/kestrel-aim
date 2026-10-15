#pragma once
#include <cstdint>
#include <vector>
#include "GameCapture.hpp"
#include "Config.hpp"

struct BoundingBox {
    int x0=0, y0=0, x1=0, y1=0;
    int count=0;
    int headX=0, headY=0;
    int width()  const { return x1-x0+1; }
    int height() const { return y1-y0+1; }
    int cx()     const { return (x0+x1)/2; }
    int cy()     const { return (y0+y1)/2; }
};

struct TargetResult {
    bool found=false;
    int headX=0, headY=0;
    int clusterCount=0;
    std::vector<BoundingBox> clusters;
};

class ColorDetection {
public:
    static TargetResult detect(const FrameView& frame, const Config& cfg);

private:
    static inline bool withinTol(int v, int t, int tol){
        int d = v - t; if(d<0) d=-d;
        return d <= tol;
    }
    static inline bool isOutlinePixel(uint8_t R, uint8_t G, uint8_t B, const Config& cfg){
        int tol = cfg.outline_tolerance.load();
        bool yellow =
            withinTol(R, cfg.outline_yellow_r.load(), tol) &&
            withinTol(G, cfg.outline_yellow_g.load(), tol) &&
            withinTol(B, cfg.outline_yellow_b.load(), tol);
        bool cyan =
            withinTol(R, cfg.outline_cyan_r.load(), tol) &&
            withinTol(G, cfg.outline_cyan_g.load(), tol) &&
            withinTol(B, cfg.outline_cyan_b.load(), tol);
        return yellow || cyan;
    }
};
