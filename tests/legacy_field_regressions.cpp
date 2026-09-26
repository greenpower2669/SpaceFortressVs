// Exercise the Android-generated legacy code, including Fab's actual spawner,
// bounce procedure and visibility predicate. No substitute field simulation.
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <cassert>
#define __ANDROID__ 1
#define main sfLegacyMain
#include "main_android_compat.cpp"
#undef main


static void clearField()
{
    for (auto *rock : sa1) delete rock;
    sa1.clear();incra1=0;
}

static void visibility()
{
    W=WIDTH=780;H=HEIGHT=1680;
    sprite rock;rock.y=H*.5f;
    for (int textureWidth : {1,32,1024,4096}) {
        w=textureWidth;
        for (float x : {-W*.1f,W*.1f,W*.9f,W*1.1f}) {
            rock.x=x;assert(inxy(&rock));
        }
        for (float x : {-W*.21f,W*1.21f}) {
            rock.x=x;assert(!inxy(&rock));
        }
    }
    std::puts("PASS: legacy visibility is symmetric and independent of the last loaded texture");
}

static void classicRendering()
{
    W=0;H=0;::setw(320);seth(640);WIDTH=int(W);HEIGHT=int(H);
    assert(W==320 && H==640);
    auto *surface=SDL_CreateRGBSurfaceWithFormat(0,320,640,32,SDL_PIXELFORMAT_RGBA32);
    auto *renderer=SDL_CreateSoftwareRenderer(surface);assert(surface && renderer);
    auto *texture=IMG_LoadTexture(renderer,IMG_PATHa1);assert(texture);
    sprite rock;rock.setv(160,320,90,90,0,0,1);rock.name="a1";rock.pv=1;rock.startup();
    assert(inxy(&rock));
    SDL_SetRenderDrawColor(renderer,0,0,0,255);SDL_RenderClear(renderer);
    SDL_Rect dest{int(rock.x-rock.w*.5f),int(rock.y-rock.h*.5f),int(rock.w),int(rock.h)};
    assert(SDL_RenderCopyEx(renderer,texture,nullptr,&dest,0,nullptr,SDL_FLIP_NONE)==0);
    std::vector<Uint32> pixels(320*640);assert(SDL_RenderReadPixels(renderer,nullptr,SDL_PIXELFORMAT_RGBA32,pixels.data(),320*4)==0);
    const Uint32 black=SDL_MapRGBA(surface->format,0,0,0,255);
    assert(std::count_if(pixels.begin(),pixels.end(),[black](Uint32 p){return p!=black;})>500);
    SDL_DestroyTexture(texture);SDL_DestroyRenderer(renderer);SDL_FreeSurface(surface);
    std::puts("PASS: generated classic path has valid W/H, admits an asteroid and renders real asteroid pixels");
}

static void mining()
{
    for (int height : {360,709,780,1536,1680}) {
        HEIGHT=height;
        sfArenaH=height;
        sprite rock,shot;rock.pv=shot.pv=1;rock.w=rock.h=100;
        sfMineAsteroid(&rock,&shot);const float step=100-rock.w;
        assert(step>0 && std::abs(step*1000-height)<.01f);
    }
    std::puts("PASS: the generated legacy mining statement stays nonzero in both orientations");
}

static void trajectories()
{
    for (auto size : {std::pair<int,int>{780,1680},{1680,780}}) {
        W=WIDTH=size.first;H=HEIGHT=size.second;
        for (int fps : {30,60,120}) {
            std::array<long,4> columns{};long total=0;
            k0=60.0f/fps;
            for (unsigned seed=1;seed<=20;++seed) {
                clearField();std::srand(seed);setastswall();
                assert(sa1.size()==4);
                for (auto *rock : sa1) assert(rock->vx==0 && rock->vy==0);
                setasts(2);assert(sa1.size()==12 && incra1==12);
                for (int frame=0;frame<fps*300;++frame) {
                    for (auto *rock : sa1) {
                        rock->ast();
                        assert(std::isfinite(rock->x) && std::isfinite(rock->y));
                        assert(rock->x>=-W*.51f && rock->x<=W*1.51f);
                        assert(rock->y>=-H*.51f && rock->y<=H*1.51f);
                        // The stationary opening wall is not the wandering field.
                        if (rock->incr>4 && rock->x>=0 && rock->x<W && rock->y>=0 && rock->y<H) {
                            ++columns[std::min(3,int(rock->x/W*4))];++total;
                        }
                    }
                }
            }
            assert(total>0);
            for (long count : columns) assert(count>total*.15 && count<total*.35);
            std::printf("PASS: %dx%d, %dHz, 20 seeds x 5min, visible X quarters %.1f/%.1f/%.1f/%.1f%% (kinematics only)\n",
                WIDTH,HEIGHT,fps,100.0*columns[0]/total,100.0*columns[1]/total,100.0*columns[2]/total,100.0*columns[3]/total);
        }
    }
    clearField();
}

static sprite *fieldRock(float x,float y,float size)
{
    auto *rock=new sprite;rock->setv(x,y,size,size,0,0,1);
    rock->h=rock->sh=size;rock->pv=1;rock->timer=0;rock->name="a1";
    sa1.push_back(rock);return rock;
}
static void interactions()
{
    W=WIDTH=sfArenaW=780;H=HEIGHT=sfArenaH=1680;k0=1;
    sfActiveMode=sfSelectedMode=SF_DUEL_LOCAL;sfUiScreen=SF_UI_GAME;setgui=false;loosej1->pv=loosej2->pv=0;
    sfTacticsReset();clearField();Spritej1->pv=Spritej2->pv=0;
    for(int i=0;i<8;++i) fieldRock(60+i*90,100,20);
    fieldRock(380,840,100);fieldRock(400,840,100);
    sfLegacyFieldFrame(1.0f/60,nullptr);
    assert(sa1.size()==24 && incra1==24 && particules.size()==29);
    assert(!explos.empty());
    int children=0;for(const auto *rock:sa1) if(rock->w==50) {++children;assert(rock->timer!=0);}
    assert(children==16); // Four historical bursts; newborns wait for the next step.
    sfTacticsReset();clearField();
    for(int i=0;i<8;++i) fieldRock(60+i*90,100,20);
    auto *rock=fieldRock(390,840,100);
    auto *shot=new sprite;shot->setxywh(390,840,8,8);shot->pv=1;entitiesj1.push_back(shot);
    sfLegacyFieldFrame(1.0f/60,nullptr);
    assert(shot->pv==0 && rock->w<100 && particules.size()==29 && !explos.empty());
    sfLegacyFieldFrame(1.0f/60,nullptr);assert(particules.size()==29);
    Spritej1->setxywh(particules.front()->x,particules.front()->y,100,100);
    Spritej2->setxywh(100,1500,100,100);Spritej2->pv=1000;
    Spritej1->pv=1000;Spritej1->nrj=30;sfCollectDust();assert(Spritej1->nrj<30);
    const float heat=Spritej1->nrj;sfCollectDust();assert(Spritej1->nrj==heat);
    // Actual ship impact through the same field owner in duel and coop.
    for(bool coop:{false,true}) {
        sfTacticsReset();clearField();
        sfActiveMode=sfSelectedMode=coop ? SF_COOP_LOCAL : SF_DUEL_LOCAL;
        sfCoop.phase=SfCoopPhase::Combat;sfCoop.invulnerable={};
        Spritej1->setxywh(390,840,100,100);Spritej1->nrj=40;Spritej1->pv=1000;Spritej2->pv=0;
        fieldRock(390,840,100);
        sfLegacyFieldFrame(1.0f/60,coop ? sfCoopHurt : nullptr);
        assert(Spritej1->pv<1000 && Spritej1->nrj>40 && !particulesr.empty() && !explos.empty());
    }
    sfTacticsReset();clearField();
    std::puts("PASS: shared real field fragments into 16 children, mines once, emits/collects ore and applies ship collisions in duel/coop");
}

static void fullField()
{
    for(auto size:{std::pair<int,int>{780,1680},{1680,780}}) for(bool coop:{false,true}) {
        double reference=0;size_t referenceCount=0;
        for(int fps:{30,60,120}) {
            sfTacticsReset();clearField();std::srand(701);
            W=WIDTH=sfArenaW=size.first;H=HEIGHT=sfArenaH=size.second;k0=60.0f/fps;
            sfActiveMode=sfSelectedMode=coop ? SF_COOP_LOCAL : SF_DUEL_LOCAL;
            Spritej1->pv=Spritej2->pv=0;
            std::array<long,4> columns{};long visible=0;
            for(int frame=0;frame<fps*40;++frame) {
                sfLegacyFieldFrame(1.0f/fps,coop ? sfCoopHurt : nullptr);
                assert(sa1.size()<=200 && particules.size()<=1000 && particulesr.size()<=1000);
                for(const auto *rock:sa1) {
                    assert(std::isfinite(rock->x) && std::isfinite(rock->y) && rock->w>0);
                    if(rock->x>=0 && rock->x<W && rock->y>=0 && rock->y<H) {
                        ++columns[std::min(3,int(rock->x/W*4))];++visible;
                    }
                }
            }
            double checksum=0;for(const auto *rock:sa1) checksum+=rock->x+rock->y+rock->w;
            if(fps==30) {reference=checksum;referenceCount=sa1.size();}
            else assert(sa1.size()==referenceCount && std::abs(checksum-reference)<.05);
            assert(visible>1000);for(long column:columns) assert(column>visible*.04);
            std::printf("PASS: actual %s field, %dx%d, %dHz x 40s, %zu surviving rocks; all four screen quarters occupied\n",
                coop ? "coop" : "duel",WIDTH,HEIGHT,fps,sa1.size());
        }
    }
    sfTacticsReset();clearField();
}

int main(int argc,char **argv)
{
    if (argc==1 || std::string(argv[1])=="visibility") visibility();
    if (argc==1 || std::string(argv[1])=="classic-render") classicRendering();
    if (argc==1 || std::string(argv[1])=="mining") mining();
    if (argc==1 || std::string(argv[1])=="trajectories") trajectories();
    if (argc==1 || std::string(argv[1])=="interactions") interactions();
    if (argc==1 || std::string(argv[1])=="full-field") fullField();
}
