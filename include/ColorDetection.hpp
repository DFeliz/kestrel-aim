#pragma once
#include <cstdint>
#include <vector>
#include "GameCapture.hpp"

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
    static TargetResult detect(const FrameView& frame,
                               int clusterDistance = 35,
                               int minPixels = 5,
                               int minWidth = 3,
                               int minHeight = 3);
private:
    static inline bool isTargetPixel(uint8_t R, uint8_t G, uint8_t B){
        if(G >= 120){
            int rb = (int)R - (int)B; if(rb < 0) rb = -rb;
            if(rb > 8) return false;
            if(((int)R - (int)G) < 50) return false;
            if(((int)B - (int)G) < 50) return false;
            if(R < 105) return false;
            if(B < 105) return false;
            return true;
        } else {
            int rb = (int)R - (int)B; if(rb < 0) rb = -rb;
            if(rb > 13) return false;
            if(((int)R - (int)G) < 60) return false;
            if(((int)B - (int)G) < 60) return false;
            if(R < 105) return false;
            if(B < 105) return false;
            return true;
        }
    }
};
