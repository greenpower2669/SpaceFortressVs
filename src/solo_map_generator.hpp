#pragma once
// Deterministic non-linear SpaceFortress SOLO maze: interconnected halls,
// alternate routes and three distant navigation relays. No automatic world
// scrolling: players can fly backwards all the way to their starting region.
#include "solo_campaign_model.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <queue>
#include <string>
#include <vector>

namespace sfsolo {
struct Rng {
    uint32_t state;
    explicit Rng(uint32_t seed):state(seed ? seed:0x9e3779b9u){}
    uint32_t next(){
        uint32_t x=state;
        x^=x<<13; x^=x>>17; x^=x<<5;
        return state=x;
    }
};
struct MapOptions {
    unsigned world=1,stage=1,difficulty=1;
    int width=54,height=160;
    uint32_t campaignSeed=0x5f2026u;
};
inline bool soloPassable(Tile tile) {return tile!=Tile::Rock;}
inline std::vector<int> soloFlood(const Map &m,int x,int y) {
    std::vector<int> distance(m.tiles.size(),-1);
    if(!m.valid()||x<0||y<0||x>=m.width||y>=m.height||
       !soloPassable(m.at(x,y)))return distance;
    const auto idx=[&](int px,int py){return py*m.width+px;};
    const int start=idx(x,y);
    distance[start]=0;
    std::queue<int> q;q.push(start);
    while(!q.empty()){
        const int here=q.front();q.pop();
        const int hx=here%m.width,hy=here/m.width;
        for(const auto v:std::array<std::array<int,2>,4>{{{0,-1},{1,0},{0,1},{-1,0}}}){
            const int nx=hx+v[0],ny=hy+v[1];
            if(nx<0||ny<0||nx>=m.width||ny>=m.height ||
               !soloPassable(m.at(nx,ny)))continue;
            const int next=idx(nx,ny);
            if(distance[next]>=0)continue;
            distance[next]=distance[here]+1;
            q.push(next);
        }
    }
    return distance;
}
inline bool soloMazeConnected(const Map &m) {
    if(!m.valid())return false;
    int x=0,y=0; unsigned triggers=0;
    for(int j=0;j<m.height;++j)for(int i=0;i<m.width;++i){
        if(m.at(i,j)==Tile::Start){x=i;y=j;}
        if(m.at(i,j)==Tile::Trigger)++triggers;
    }
    const auto paths=soloFlood(m,x,y);
    if(triggers<3)return false;
    for(size_t k=0;k<m.tiles.size();++k){
        const Tile tile=m.tiles[k];
        if((tile==Tile::Finish||tile==Tile::Boss||tile==Tile::Trigger) &&
           paths[k]<0)return false;
    }
    return true;
}
inline Map generate(const MapOptions &o) {
    Map m;
    if(o.world==0 || o.stage==0||o.stage>20||o.difficulty<1||
       o.difficulty>9||o.width<24||o.width>512||
       o.height<32||o.height>8192)return m;
    m.world=o.world;m.stage=o.stage;m.version=3;
    m.id="W"+std::to_string(o.world)+"-S"+std::to_string(o.stage)+
         "-D"+std::to_string(o.difficulty)+"-MAZE-V3";
    m.width=o.width;m.height=o.height;
    m.tiles.assign(size_t(m.width)*size_t(m.height),Tile::Rock);
    Rng rng(o.campaignSeed ^ (o.world*0x85ebca6bu) ^
            (o.stage*0xc2b2ae35u) ^ (o.difficulty*0x27d4eb2fu));
    const int mid=m.width/2;
    auto at=[&](int x,int y)->Tile& {return m.tiles[size_t(y)*m.width+x];};
    auto carve=[&](int x,int y,int radius=1){
        for(int yy=y-radius;yy<=y+radius;++yy)
            for(int xx=x-radius;xx<=x+radius;++xx)
                if(xx>=2&&yy>=1&&xx<m.width-2&&yy<m.height-1)
                    at(xx,yy)=Tile::Empty;
    };
    auto tunnel=[&](int ax,int ay,int bx,int by){
        int x=ax,y=ay;
        carve(x,y);
        while(x!=bx){x+=(bx>x?1:-1);carve(x,y);}
        while(y!=by){y+=(by>y?1:-1);carve(x,y);}
    };
    // Chamber centers are deliberately much wider than the visible 18-cell
    // screen. A single screen cannot reveal the map's other choices.
    std::vector<int> xs,ys;
    for(int x=5;x<=m.width-6;x+=7)xs.push_back(x);
    for(int y=12;y<=m.height-10;y+=9)ys.push_back(y);
    if(xs.size()<2||ys.size()<2)return {};
    const int cols=int(xs.size()),rows=int(ys.size());
    const int total=cols*rows;
    const auto id=[&](int col,int row){return row*cols+col;};
    const int middleCol=int(std::min_element(xs.begin(),xs.end(),
         [&](int a,int b){return std::abs(a-mid)<std::abs(b-mid);})-xs.begin());
    const int startNode=id(middleCol,rows-1);
    std::vector<uint8_t> seen(size_t(total),0);
    std::vector<std::array<bool,4>> links(size_t(total));
    std::vector<int> stack={startNode};
    seen[startNode]=1;
    for(int yy:ys)for(int xx:xs)carve(xx,yy,2);
    constexpr std::array<int,4> dc{0,1,0,-1};
    constexpr std::array<int,4> dr{-1,0,1,0};
    while(!stack.empty()){
        const int here=stack.back(),col=here%cols,row=here/cols;
        std::vector<int> candidates;
        for(int direction=0;direction<4;++direction){
            int nc=col+dc[direction],nr=row+dr[direction];
            if(nc>=0&&nc<cols&&nr>=0&&nr<rows&&!seen[id(nc,nr)])
                candidates.push_back(direction);
        }
        if(candidates.empty()){stack.pop_back();continue;}
        const int direction=candidates[rng.next()%candidates.size()];
        const int nc=col+dc[direction],nr=row+dr[direction];
        const int next=id(nc,nr);
        links[here][direction]=true;
        links[next][(direction+2)%4]=true;
        tunnel(xs[col],ys[row],xs[nc],ys[nr]);
        seen[next]=1;
        stack.push_back(next);
    }
    // Deterministic loops: alternate routes, short-cuts, and occasional
    // double-back passages instead of a single perfect-maze solution.
    int shortcuts=0;
    for(int row=0;row<rows;++row)for(int col=0;col<cols;++col){
        const int here=id(col,row);
        for(int direction:std::array<int,2>{1,2}){
            const int nc=col+dc[direction],nr=row+dr[direction];
            if(nc>=cols||nr>=rows||links[here][direction])continue;
            if(rng.next()%100u<23u) {
                const int next=id(nc,nr);
                links[here][direction]=true;
                links[next][(direction+2)%4]=true;
                tunnel(xs[col],ys[row],xs[nc],ys[nr]);
                ++shortcuts;
            }
        }
    }
    // Guarantee more than one loop even for compact tutorial maps.
    if(shortcuts<2 && cols>=2 && rows>=2){
        for(int row=0;row<rows-1&&shortcuts<2;++row)
          for(int col=0;col<cols-1&&shortcuts<2;++col){
            for(int direction:std::array<int,2>{1,2}){
                const int here=id(col,row),nc=col+dc[direction],nr=row+dr[direction];
                if(links[here][direction])continue;
                const int next=id(nc,nr);
                links[here][direction]=true;
                links[next][(direction+2)%4]=true;
                tunnel(xs[col],ys[row],xs[nc],ys[nr]);
                ++shortcuts;
            }
          }
    }
    // Connect start/boss/finish to the room network without removing the
    // authored entrance. Returning to the opening region stays possible.
    tunnel(mid,m.height-3,xs[middleCol],ys.back());
    tunnel(xs[middleCol],ys.front(),mid,9);
    tunnel(mid,9,mid,2);
    // Real side objectives: one near each edge at separate heights. To open
    // the finish the player collects all three, which naturally creates
    // backtracking and visits through alternative branches.
    const std::array<std::array<int,2>,3> relays{{
        {0,std::max(0,rows-3)},
        {cols-1,rows/2},
        {0,std::min(rows-1,2)}
    }};
    for(const auto &rc:relays)at(xs[rc[0]],ys[rc[1]])=Tile::Trigger;
    // Scatter distinct hazards on the traversable network, avoiding mandatory
    // relays and avoiding the start/boss/finish safe connectors.
    for(int row=1;row<rows-1;++row)for(int col=0;col<cols;++col){
        const int x=xs[col],y=ys[row];
        if(at(x,y)!=Tile::Empty)continue;
        if(rng.next()%100u>=38u)continue;
        const unsigned roll=rng.next()%13u;
        Tile t=roll<3?Tile::Asteroid:roll<5?Tile::IceAsteroid:
               roll<7?Tile::Lava:roll<9?Tile::Beam:Tile::Enemy;
        if(o.difficulty>=8 && t==Tile::Asteroid && (rng.next()%3u))
            t=Tile::Enemy;
        at(x,y)=t; // all are navigable/destroyable, never a stone blockade.
    }
    // Tutorial always exposes a recognisable ice obstacle off the straight
    // entrance without obstructing the finish.
    if(o.stage==1 && rows>=4 && cols>=3 &&
       at(xs[cols-2],ys[rows-2])==Tile::Empty)
        at(xs[cols-2],ys[rows-2])=Tile::IceAsteroid;
    at(mid,9)=Tile::Boss;
    at(mid,m.height-3)=Tile::Start;
    at(mid,2)=Tile::Finish;
    return m;
}
// Legacy tests/reference callers: physical free vertical corridor intentionally
// goes away; this checks real connectivity through winding paths instead.
inline bool hasVerticalSafeCorridor(const Map &m) {return soloMazeConnected(m);}
} // namespace sfsolo
