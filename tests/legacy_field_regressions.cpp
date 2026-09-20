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
#include "legacy-mining-snippet.hpp"

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

static void mining()
{
    for (int height : {360,709,780,1536,1680}) {
        HEIGHT=height;
        const float step=sfTestMiningStep();
        assert(step>0 && std::abs(step*1000-height)<.001f);
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

int main(int argc,char **argv)
{
    if (argc==1 || std::string(argv[1])=="visibility") visibility();
    if (argc==1 || std::string(argv[1])=="mining") mining();
    if (argc==1 || std::string(argv[1])=="trajectories") trajectories();
}
