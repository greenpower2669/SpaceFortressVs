#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <cassert>
#include <cstdio>
#include <vector>

#define __ANDROID__ 1
#include "t.hpp"
#include "th2.h"

bool setgui=true,setia=false,sdlstarted=true;
float k0=1,tw=780,th=1680;
int tirj1=0,tirj2=0,incra1=0;
std::list<sprite*> sa1;
std::list<sprite*> entitiesj1,entitiesj2,burnsj1,burnsj2;
std::list<parts*> particules,particulesr;
std::list<eexpl*> explos;
bool tirjz=false,tirj1z=false,tirj2z=false;
sprite *Spritej1=new sprite,*Spritej2=new sprite;
sprite *loosej1=new sprite,*loosej2=new sprite;
sprite *rouage1=new sprite,*rouage2=new sprite,*Suiveur=new sprite;
enti *iago=new enti,*iago1=new enti,*iacalc=new enti,*iatake=new enti;

static int nonBlack(SDL_Surface *surface)
{
    std::vector<Uint32> pixels(surface->w*surface->h);
    assert(SDL_RenderReadPixels(SDL_GetRenderer(surface),nullptr,SDL_PIXELFORMAT_RGBA32,
        pixels.data(),surface->w*4)==0);
    const Uint32 black=SDL_MapRGBA(surface->format,0,0,0,255);
    int count=0;for(auto pixel:pixels) if(pixel!=black) ++count;return count;
}

int main()
{
    assert(sfHelpState.format==SfHelpFormat::Animated);
    assert(sfHelpPageCount(SfHelpFormat::Quick)>=6);
    assert(sfHelpPageCount(SfHelpFormat::Detailed)>=9);
    assert(sfHelpPageCount(SfHelpFormat::Animated)==sfHelpPageCount(SfHelpFormat::Detailed));

    sfHelpOpen(SF_UI_HOME,false);
    assert(sfHelpState.open && sfHelpState.returnScreen==SF_UI_HOME && !sfHelpState.fromLiveGame);
    sfHelpSelectFormat(SfHelpFormat::Quick);
    assert(sfHelpState.format==SfHelpFormat::Quick && sfHelpState.page==0);
    sfHelpNext();assert(sfHelpState.page==1);
    sfHelpPrevious();assert(sfHelpState.page==0);
    sfHelpCloseRequest();assert(sfHelpState.closeRequested);
    // Preference survives close/re-open within the process.
    sfHelpState.open=false;sfHelpState.closeRequested=false;
    sfHelpOpen(SF_UI_HOME,false);assert(sfHelpState.format==SfHelpFormat::Quick);

    for(auto size:{SDL_Point{360,780},SDL_Point{780,360}}) {
        const auto button=sfHelpGameButtonRect(size.x,size.y);
        assert(button.w>=42 && button.h>=42);
        assert(button.x>=0 && button.y>=0 && button.x+button.w<=size.x && button.y+button.h<=size.y);
        auto *surface=SDL_CreateRGBSurfaceWithFormat(0,size.x,size.y,32,SDL_PIXELFORMAT_RGBA32);
        auto *renderer=SDL_CreateSoftwareRenderer(surface);assert(surface && renderer);
        SDL_SetRenderDrawColor(renderer,0,0,0,255);SDL_RenderClear(renderer);
        sfHelpSelectFormat(SfHelpFormat::Animated);sfHelpState.page=0;
        // Null optional textures are the required fallback path.
        sfHelpDraw(renderer,nullptr,nullptr);
        SDL_RenderPresent(renderer);
        std::vector<Uint32> pixels(size.x*size.y);
        assert(SDL_RenderReadPixels(renderer,nullptr,SDL_PIXELFORMAT_RGBA32,pixels.data(),size.x*4)==0);
        const Uint32 black=SDL_MapRGBA(surface->format,0,0,0,255);
        assert(std::count_if(pixels.begin(),pixels.end(),[black](Uint32 p){return p!=black;})>size.x*size.y/30);
        SDL_DestroyRenderer(renderer);SDL_FreeSurface(surface);
    }

    std::puts("PASS: help defaults animated, keeps player choice, navigates and renders with asset fallback");
    return 0;
}
