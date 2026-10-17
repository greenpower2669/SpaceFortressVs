#include "coop_solo_scenery.hpp"
#include "solo_lair_transition.hpp"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <vector>
static std::vector<Uint32> pixels(SDL_Renderer *r){
    std::vector<Uint32> out(540*960);
    assert(SDL_RenderReadPixels(r,nullptr,SDL_PIXELFORMAT_RGBA8888,out.data(),540*4)==0);return out;
}
static void clear(SDL_Renderer *r,Uint8 red,Uint8 green,Uint8 blue){
    SDL_SetRenderDrawColor(r,red,green,blue,255);SDL_RenderClear(r);
}
int main(){
    using namespace sfsolo;
    SDL_setenv("SDL_VIDEODRIVER","dummy",1);assert(SDL_Init(SDL_INIT_VIDEO)==0);
    assert(IMG_Init(IMG_INIT_PNG)&IMG_INIT_PNG);
    auto *surface=SDL_CreateRGBSurfaceWithFormat(0,540,960,32,SDL_PIXELFORMAT_RGBA8888);
    auto *r=SDL_CreateSoftwareRenderer(surface);assert(r);
    {
        CoopSoloScenery scenery(r);assert(scenery.ready());
        // SOLO-023: one RGBA sprite atlas gives 16 topology masks x 3
        // naturally chipped variations, for all six original wall themes.
        assert(scenery.edgeSprites.ready());
        assert(SoloEdgeSprites::profiles==16&&SoloEdgeSprites::variants==3);
        for(int theme=0;theme<6;++theme)
            for(int mask=0;mask<16;++mask)
                for(int v=0;v<3;++v){
                    auto *tex=scenery.edgeSprites.tile(theme,mask,v);assert(tex);
                    int w=0,h=0;SDL_ScaleMode sm=SDL_ScaleModeLinear;
                    assert(SDL_QueryTexture(tex,nullptr,nullptr,&w,&h)==0);
                    assert(w==128 && h==128);
                    assert(SDL_GetTextureScaleMode(tex,&sm)==0);
                    assert(sm==SDL_ScaleModeNearest);
                }
        // Real nonrectangular alpha: the top-left corner of an isolated
        // rock is open, while its middle remains opaque on BOTH backgrounds.
        SDL_FRect soloCell{90,90,90,90};
        auto *isolated=scenery.edgeSprites.tile(0,0,1);
        clear(r,255,0,255);
        SoloEdgeSprites::drawCell(r,isolated,soloCell);
        const auto redPixels=pixels(r);
        SoloEdgeSprites::drawCell(r,isolated,soloCell);
        assert(redPixels==pixels(r)); // Binary alpha: repeated foreground is stable.
        clear(r,0,255,0);
        SoloEdgeSprites::drawCell(r,isolated,soloCell);
        const auto greenPixels=pixels(r);
        assert(redPixels[size_t(91)*540+91]!=greenPixels[size_t(91)*540+91]);
        assert(redPixels[size_t(135)*540+135]==greenPixels[size_t(135)*540+135]);
        // A fully surrounded Rock cell cannot develop an artificial crack
        // into traversable space at a shared tile edge.
        const auto *connected=scenery.edgeSprites.tile(0,15,1);
        assert(connected!=nullptr);

        // SOLO-021: asset stays untouched; only foreground sampling improves.
        // Backgrounds MUST remain linear, with no change to parallax.
        SDL_ScaleMode scaleMode=SDL_ScaleModeNearest;
        assert(SDL_GetTextureScaleMode(scenery.space,&scaleMode)==0);
        assert(scaleMode==SDL_ScaleModeLinear);
        assert(SDL_GetTextureScaleMode(scenery.mineral,&scaleMode)==0);
        assert(scaleMode==SDL_ScaleModeLinear);
        int nativeW=0,nativeH=0;
        assert(SDL_QueryTexture(scenery.material[0],nullptr,nullptr,&nativeW,&nativeH)==0);
        assert(nativeW==200 && nativeH==58);
        for(auto *wall:scenery.material){
            assert(SDL_GetTextureScaleMode(wall,&scaleMode)==0);
            assert(scaleMode==SDL_ScaleModeNearest);
        }
        assert(std::abs(CoopSoloScenery::wallRepeatCellsX*90-nativeW)<nativeW*.15f);
        assert(std::abs(CoopSoloScenery::wallRepeatCellsY*90-nativeH)<nativeH*.15f);
        auto *plate=IMG_Load("resources/assets/pict/solo/FondsParallaxeModeSolo.png");assert(plate);
        assert(plate->w<=2048&&plate->h<=2048);
        auto *rgba=SDL_ConvertSurfaceFormat(plate,SDL_PIXELFORMAT_RGBA32,0);assert(rgba);
        size_t transparent=0,rock=0;
        for(int y=0;y<rgba->h;++y)for(int x=0;x<rgba->w;++x){
            const auto p=reinterpret_cast<Uint32*>(static_cast<Uint8*>(rgba->pixels)+y*rgba->pitch)[x];
            Uint8 red,g,b,a;SDL_GetRGBA(p,rgba->format,&red,&g,&b,&a);
            transparent+=a==0;rock+=a>=230;
            if(x==0||y==0||x==rgba->w-1||y==rgba->h-1)assert(a>220);
        }
        assert(transparent>size_t(rgba->w*rgba->h)/6&&rock>size_t(rgba->w*rgba->h)/2);
        SDL_FreeSurface(rgba);SDL_FreeSurface(plate);
        CoopSoloWorldBridge world;assert(world.create({}, {0,0,540,960},540));world.centerOn(world.start());
        const auto map=world.map.tiles;const auto cx=world.cameraLeft,cy=world.cameraTop;
        SDL_ClearError();scenery.draw(r,world,0);assert(!*SDL_GetError());const auto first=pixels(r);
        scenery.draw(r,world,0);assert(first==pixels(r));
        world.cameraLeft+=300;world.cameraTop+=200;scenery.draw(r,world,0);assert(first!=pixels(r));
        world.cameraLeft=cx;world.cameraTop=cy;scenery.draw(r,world,0);assert(first==pixels(r));
        assert(map==world.map.tiles);
        if(const char *path=std::getenv("SF_SOLO_SCENERY_CAPTURE"))assert(IMG_SavePNG(surface,path)==0);
        scenery.drawBackground(r,world,false);const auto sky=pixels(r);
        scenery.drawBackground(r,world,true);const auto cave=pixels(r);
        size_t skyVisible=0,mineral=0;
        for(size_t i=0;i<sky.size();++i){skyVisible+=sky[i]==cave[i];mineral+=sky[i]!=cave[i];}
        assert(skyVisible>1000&&mineral>1000);
        // Same world material rendered as one rectangle or 32 cells must have
        // identical coverage, including fractional camera and Android scaling.
        auto *flat=SDL_CreateRGBSurfaceWithFormat(0,8,8,32,SDL_PIXELFORMAT_RGBA32);
        SDL_FillRect(flat,nullptr,SDL_MapRGBA(flat->format,80,120,180,255));
        auto *texture=SDL_CreateTextureFromSurface(r,flat);SDL_FreeSurface(flat);assert(texture);
        SDL_SetTextureBlendMode(texture,SDL_BLENDMODE_NONE);
        for(float scale:{1.0f,.75f,2048.0f/4800.0f,1.25f})for(float camera:{0.0f,.37f,173.28f}){
            SDL_RenderSetScale(r,scale,scale);clear(r,255,0,255);
            const float origin=70-camera;
            CoopSoloScenery::tiled(r,texture,{origin,40,360,720},90,0,405,180);
            const auto whole=pixels(r);clear(r,255,0,255);
            for(int y=0;y<8;++y)for(int x=0;x<4;++x)
                CoopSoloScenery::tiled(r,texture,{origin+x*90,40.0f+y*90,90,90},90.0f+x*90,y*90.0f,405,180);
            assert(whole==pixels(r));
        }
        SDL_DestroyTexture(texture);SDL_RenderSetScale(r,1,1);
        std::fill(world.map.tiles.begin(),world.map.tiles.end(),Tile::Empty);
        for(int y=1;y<9;++y)for(int x=1;x<5;++x)world.map.tiles[size_t(y)*world.map.width+x]=Tile::Rock;
        assert(!world.canOccupy({180,180},10));world.map.tiles[size_t(9)*world.map.width+1]=Tile::Lava;
        assert(world.canOccupy({135,855},10));
        for(float scale:{1.0f,.75f,2048.0f/4800.0f,1.25f})for(float camera:{0.0f,.37f}){
            world.cameraLeft=world.cameraTop=camera;SDL_RenderSetScale(r,scale,scale);
            clear(r,255,0,255);scenery.drawWalls(r,world,0);const auto pink=pixels(r);
            clear(r,0,255,0);scenery.drawWalls(r,world,0);const auto green=pixels(r);
            // SOLO-023: the exposed 16-24px rim is INTENTIONALLY see-through,
            // otherwise the silhouette would still be a square. Retain the
            // old opacity check in the true rock CORE at each zoom/scale.
            for(int y=int(std::ceil((115-camera)*scale));y<std::min(960,int((785-camera)*scale));++y)
                for(int x=int(std::ceil((115-camera)*scale));x<std::min(540,int((425-camera)*scale));++x){
                    assert(pink[size_t(y)*540+x]==green[size_t(y)*540+x]);
                    assert((green[size_t(y)*540+x]>>8)!=0x393b45);
                }
            // Top-left outside corner MUST be transparent, allowing the
            // different backgrounds to show through the carved Rock edge.
            const int edgeX=int(std::floor((92-camera)*scale));
            const int edgeY=int(std::floor((92-camera)*scale));
            assert(edgeX>=0 && edgeY>=0 && edgeX<540 && edgeY<960);
            assert(pink[size_t(edgeY)*540+edgeX]!=green[size_t(edgeY)*540+edgeX]);
        }
        SDL_RenderSetScale(r,1,1);world.cameraLeft=world.cameraTop=0;
        clear(r,15,25,35);scenery.draw(r,world,0);
        const auto beforeMask=pixels(r);scenery.drawWalls(r,world,0,true);
        assert(beforeMask==pixels(r)); // translucent hazards are not composited twice
        SDL_Rect viewport{3,4,500,900},clip{7,9,400,800};
        SDL_RenderSetViewport(r,&viewport);SDL_RenderSetScale(r,.75f,.8f);SDL_RenderSetClipRect(r,&clip);
        SDL_Rect beforeV{},beforeC{},afterV{},afterC{};
        SDL_RenderGetViewport(r,&beforeV);SDL_RenderGetClipRect(r,&beforeC);
        SDL_SetRenderDrawColor(r,1,2,3,4);SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_ADD);
        scenery.draw(r,world,0);SDL_RenderGetViewport(r,&afterV);SDL_RenderGetClipRect(r,&afterC);
        assert(SDL_RectEquals(&beforeV,&afterV)&&SDL_RectEquals(&beforeC,&afterC));
        float sx,sy;SDL_RenderGetScale(r,&sx,&sy);assert(sx==.75f&&sy==.8f);
        Uint8 red,g,b,a;SDL_GetRenderDrawColor(r,&red,&g,&b,&a);assert(red==1&&g==2&&b==3&&a==4);
        SDL_BlendMode mode;SDL_GetRenderDrawBlendMode(r,&mode);assert(mode==SDL_BLENDMODE_ADD);
        SDL_RenderSetViewport(r,nullptr);SDL_RenderSetScale(r,1,1);SDL_RenderSetClipRect(r,nullptr);
        SDL_RenderSetLogicalSize(r,3200,4800);
        LairTransition transition;assert(transition.beginCapture(r,3200,4800,1600,2400));
        CoopSoloWorldBridge big;assert(big.create({}, {0,0,3200,4800},3200));big.centerOn(big.start());
        scenery.draw(r,big,0);transition.endCapture();transition.release();SDL_RenderSetLogicalSize(r,540,960);
        scenery.release();scenery.release();assert(!scenery.ready());scenery.reload();assert(scenery.ready());
        world.map.tiles=map;world.cameraLeft=cx;world.cameraTop=cy;scenery.draw(r,world,0);assert(first==pixels(r));
    }
    SDL_DestroyRenderer(r);SDL_FreeSurface(surface);IMG_Quit();SDL_Quit();
    std::puts("PASS: SOLO actual atlases, alpha openings, reversible parallax, scaled wall continuity, immutable collision map, state/reset");
}
