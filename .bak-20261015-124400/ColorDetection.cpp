#include "ColorDetection.hpp"
#include <queue>
#include <algorithm>
#include <cmath>
#include <cstdlib>

TargetResult ColorDetection::detect(const FrameView& frame,
                                    int clusterDistance, int minPixels,
                                    int minWidth, int minHeight){
    TargetResult out{};
    if(!frame.valid || !frame.data) return out;
    const int W = frame.width, H = frame.height, stride = frame.stride;
    std::vector<uint8_t> mask((size_t)W*(size_t)H, 0);
    for(int y = 0; y < H; ++y){
        const uint8_t* row = frame.data + (size_t)y*(size_t)stride;
        for(int x = 0; x < W; ++x){
            uint8_t B = row[x*4+0], G = row[x*4+1], R = row[x*4+2];
            if(isTargetPixel(R,G,B)) mask[(size_t)y*(size_t)W + (size_t)x] = 1;
        }
    }
    std::vector<int> label((size_t)W*(size_t)H, -1);
    std::vector<BoundingBox> raw;
    const int dx8[8] = {1,-1,0,0,1,1,-1,-1};
    const int dy8[8] = {0,0,1,-1,1,-1,1,-1};
    for(int y = 0; y < H; ++y){
        for(int x = 0; x < W; ++x){
            size_t idx = (size_t)y*(size_t)W + (size_t)x;
            if(!mask[idx] || label[idx] != -1) continue;
            int id = (int)raw.size();
            BoundingBox bb; bb.x0=x; bb.y0=y; bb.x1=x; bb.y1=y; bb.count=0;
            bb.headX=x; bb.headY=y;
            std::queue<std::pair<int,int>> q; q.push({x,y}); label[idx]=id;
            while(!q.empty()){
                auto pr = q.front(); q.pop();
                int cx=pr.first, cy=pr.second;
                bb.count++;
                if(cx<bb.x0)bb.x0=cx; if(cx>bb.x1)bb.x1=cx;
                if(cy<bb.y0)bb.y0=cy; if(cy>bb.y1)bb.y1=cy;
                if(cy<bb.headY || (cy==bb.headY && std::abs(cx-W/2)<std::abs(bb.headX-W/2))){
                    bb.headY=cy; bb.headX=cx;
                }
                for(int k=0;k<8;++k){
                    int nx=cx+dx8[k], ny=cy+dy8[k];
                    if(nx<0||ny<0||nx>=W||ny>=H) continue;
                    size_t nidx=(size_t)ny*(size_t)W+(size_t)nx;
                    if(mask[nidx] && label[nidx]==-1){ label[nidx]=id; q.push({nx,ny}); }
                }
            }
            raw.push_back(bb);
        }
    }
    std::vector<BoundingBox> filtered;
    for(size_t i=0;i<raw.size();++i){
        auto& bb = raw[i];
        if(bb.count < minPixels) continue;
        if(bb.width() < minWidth) continue;
        if(bb.height() < minHeight) continue;
        filtered.push_back(bb);
    }
    bool merged = true;
    while(merged){
        merged = false;
        for(size_t i=0;i<filtered.size() && !merged;++i){
            for(size_t j=i+1;j<filtered.size() && !merged;++j){
                auto& a=filtered[i]; auto& b=filtered[j];
                int gapX = std::max(0, std::max(a.x0-b.x1, b.x0-a.x1));
                int gapY = std::max(0, std::max(a.y0-b.y1, b.y0-a.y1));
                if(gapX<=clusterDistance && gapY<=clusterDistance){
                    a.x0=std::min(a.x0,b.x0); a.y0=std::min(a.y0,b.y0);
                    a.x1=std::max(a.x1,b.x1); a.y1=std::max(a.y1,b.y1);
                    a.count+=b.count;
                    if(b.headY<a.headY || (b.headY==a.headY && std::abs(b.headX-W/2)<std::abs(a.headX-W/2))){
                        a.headY=b.headY; a.headX=b.headX;
                    }
                    filtered.erase(filtered.begin()+(ptrdiff_t)j);
                    merged = true;
                }
            }
        }
    }
    std::vector<BoundingBox> finalClusters;
    for(size_t i=0;i<filtered.size();++i){
        bool contained = false;
        for(size_t j=0;j<filtered.size();++j){
            if(i==j) continue;
            const auto& a=filtered[i]; const auto& b=filtered[j];
            if(a.x0>=b.x0 && a.x1<=b.x1 && a.y0>=b.y0 && a.y1<=b.y1 && a.count<b.count){
                contained=true; break;
            }
        }
        if(!contained) finalClusters.push_back(filtered[i]);
    }
    out.clusters = finalClusters;
    out.clusterCount = (int)finalClusters.size();
    if(!finalClusters.empty()){
        int cx=W/2, cy=H/2;
        size_t best=0; double bestD=1e18;
        for(size_t i=0;i<finalClusters.size();++i){
            double dx=(double)(finalClusters[i].cx()-cx);
            double dy=(double)(finalClusters[i].cy()-cy);
            double d=dx*dx+dy*dy;
            if(d<bestD){ bestD=d; best=i; }
        }
        out.found = true;
        out.headX = finalClusters[best].headX;
        out.headY = finalClusters[best].headY;
    }
    return out;
}
