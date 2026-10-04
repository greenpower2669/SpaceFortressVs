#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <string>
#include <vector>

enum class SfHelpFormat { Quick=0, Detailed=1, Animated=2 };
enum class SfHelpView { Hub=0, Page=1 };

struct SfHelpState {
    SfHelpFormat format=SfHelpFormat::Animated;
    SfHelpView view=SfHelpView::Hub;
    int page=0;
    int returnScreen=SF_UI_HOME;
    bool open=false;
    bool fromLiveGame=false;
    bool closeRequested=false;
    bool tutorialRequested=false;
};
inline SfHelpState sfHelpState;

struct SfHelpPage {
    const char *title;
    const char *line1;
    const char *line2;
    int diagram;
};

inline constexpr std::array<SfHelpPage,8> SF_HELP_QUICK_PAGES{{
    {"OBJECTIF","SURVIVEZ ET DETRUISEZ","L ADVERSAIRE OU LE BOSS",0},
    {"CONTROLES","GLISSEZ POUR BOUGER","TAPOTEZ POUR TIRER",1},
    {"HUD","PV = COQUE","ENERGIE = RESERVE DE COMBAT",2},
    {"ENERGIE","TIRER CHAUFFE LA RESERVE","POUSSIERE BLANCHE RECHARGE",3},
    {"POUSSIERES","BLANCHE = ENERGIE PUIS SOIN","ROUGE = VISUELLE",4},
    {"CINETIQUE","LES ASTEROIDES POUSSENT","DES VAGUES DE PROTECTION",5},
    {"SURCHARGE","GARDEZ LE 2E DOIGT 2 S","RELACHEZ POUR PURGER",6},
    {"DANGER","LE NOM REGLE LES DEGATS","LES COEFFICIENTS RESTENT CACHES",7}
}};

inline constexpr std::array<SfHelpPage,11> SF_HELP_DETAILED_PAGES{{
    {"MODES ET OBJECTIF","DUEL LOCAL IA OU COOP","CAMPAGNE = 200 COMBATS",0},
    {"CONTROLES","GLISSEZ LE VAISSEAU","UN AUTRE DOIGT DECLENCHE LE TIR",1},
    {"HUD","LES BARRES PV MONTRENT LA COQUE","ENERGIE MONTRE LA RESERVE",2},
    {"ENERGIE","PLEIN = RESERVE DISPONIBLE","EPUISE = TIRS MOINS CONFORTABLES",3},
    {"PV ET BOUCLIERS","LE BOUCLIER ABSORBE UNE PART","LE RESTE ATTEINT LA COQUE",8},
    {"POUSSIERES","BLANCHE RECHARGE PUIS SOIGNE","ROUGE NE DONNE AUCUN BONUS",4},
    {"BOUCLIER CINETIQUE","IMPACT = MASSE ET VITESSE RELATIVE","LES VAGUES PARTENT DU CENTRE",5},
    {"VAGUES","ELLES GRANDISSENT VERS L EXTERIEUR","LA POUSSIERE BLANCHE RESTE LIBRE",9},
    {"SURCHARGE 2 DOIGTS","MAINTENEZ 2 SECONDES","PRET = VULNERABLE JUSQU AU RELACHE",6},
    {"CAMPAGNE 200","50 BOSS FOIS 4 DIFFICULTES","PROGRESSION ET HALL OF FAME",10},
    {"DANGER ET ASTUCES","LE NOM CHANGE LES DEGATS HOSTILES","LE CINETIQUE RESTE INDEPENDANT",7}
}};

static int sfHelpPageCount(SfHelpFormat format)
{
    return format==SfHelpFormat::Quick ? int(SF_HELP_QUICK_PAGES.size()) : int(SF_HELP_DETAILED_PAGES.size());
}

static const SfHelpPage &sfHelpCurrentPage()
{
    if(sfHelpState.format==SfHelpFormat::Quick) {
        sfHelpState.page=std::clamp(sfHelpState.page,0,int(SF_HELP_QUICK_PAGES.size())-1);
        return SF_HELP_QUICK_PAGES[size_t(sfHelpState.page)];
    }
    sfHelpState.page=std::clamp(sfHelpState.page,0,int(SF_HELP_DETAILED_PAGES.size())-1);
    return SF_HELP_DETAILED_PAGES[size_t(sfHelpState.page)];
}

static void sfHelpOpen(int returnScreen,bool fromLiveGame)
{
    sfHelpState.open=true;
    sfHelpState.view=SfHelpView::Hub;
    sfHelpState.page=0;
    sfHelpState.returnScreen=returnScreen;
    sfHelpState.fromLiveGame=fromLiveGame;
    sfHelpState.closeRequested=false;
    sfHelpState.tutorialRequested=false;
}

static void sfHelpSelectFormat(SfHelpFormat format)
{
    sfHelpState.format=format;
    sfHelpState.page=0;
    sfHelpState.view=SfHelpView::Page;
    sfHelpState.closeRequested=false;
}

static void sfHelpNext()
{
    sfHelpState.page=std::min(sfHelpState.page+1,sfHelpPageCount(sfHelpState.format)-1);
}

static void sfHelpPrevious()
{
    sfHelpState.page=std::max(0,sfHelpState.page-1);
}

static void sfHelpShowHub()
{
    sfHelpState.view=SfHelpView::Hub;
    sfHelpState.page=0;
}

static void sfHelpCloseRequest()
{
    sfHelpState.closeRequested=true;
}

static void sfHelpBack()
{
    if(sfHelpState.view==SfHelpView::Page) sfHelpShowHub();
    else sfHelpCloseRequest();
}

static SDL_Rect sfHelpGameButtonRect(int width,int height)
{
    const int side=std::max(42,std::min(72,std::min(width,height)/9));
    const int margin=std::max(6,std::min(width,height)/45);
    return {std::max(0,width-margin-side),margin,side,side};
}

static SDL_Rect sfHelpHubButtonRect(int index,int width,int height)
{
    const bool portrait=height>=width;
    if(portrait) {
        const int x=int(width*.10f),w=int(width*.80f),h=std::max(48,int(height*.105f));
        const int y=int(height*.25f)+index*int(height*.135f);
        return {x,y,w,h};
    }
    const int gap=std::max(8,int(width*.025f));
    const int w=(width-gap*3)/2,h=std::max(48,int(height*.20f));
    const int x=gap+(index%2)*(w+gap),y=int(height*.28f)+(index/2)*int(height*.27f);
    return {x,y,w,h};
}

static void sfHelpDrawButton(SDL_Renderer *renderer,SDL_Rect rect,const char *label,bool selected=false)
{
    sfUiPanel(renderer,rect,selected ? 10 : 6,selected ? 48 : 22,selected ? 58 : 42,
              selected ? 110 : 70,selected ? 235 : 195,selected ? 225 : 240);
    const int scale=std::max(1,std::min({std::max(1,rect.h/12),std::max(1,rect.w/std::max(1,int(std::strlen(label))*7)),4}));
    sfUiText(renderer,rect.x+(rect.w-sfUiTextWidth(label,scale))/2,
             rect.y+(rect.h-7*scale)/2,label,scale,235,248,255);
}

static void sfHelpDrawArrow(SDL_Renderer *renderer,int x1,int y1,int x2,int y2,SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);
    SDL_RenderDrawLine(renderer,x1,y1,x2,y2);
    const float angle=std::atan2(float(y2-y1),float(x2-x1));
    const int size=10;
    SDL_RenderDrawLine(renderer,x2,y2,x2-int(std::cos(angle-.55f)*size),y2-int(std::sin(angle-.55f)*size));
    SDL_RenderDrawLine(renderer,x2,y2,x2-int(std::cos(angle+.55f)*size),y2-int(std::sin(angle+.55f)*size));
}

static void sfHelpDrawDiagram(SDL_Renderer *renderer,int kind,SDL_Rect area,SDL_Texture *shipTexture,SDL_Texture *rockTexture)
{
    if(!renderer || area.w<=0 || area.h<=0) return;
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    const float seconds=float(SDL_GetTicks64()%120000)*.001f;
    const int cx=area.x+area.w/2,cy=area.y+area.h/2;
    const int unit=std::max(8,std::min(area.w,area.h)/7);

    auto drawShip=[&](int x,int y,int size) {
        if(shipTexture) {
            SDL_Rect dst{x-size/2,y-size/2,size,size};
            SDL_RenderCopy(renderer,shipTexture,nullptr,&dst);
        } else {
            sfUiCircle(renderer,x,y,size/2,95,210,255);
            sfUiCircle(renderer,x,y,std::max(2,size/5),255,170,85);
        }
    };
    auto drawRock=[&](int x,int y,int size) {
        if(rockTexture) {
            SDL_Rect dst{x-size/2,y-size/2,size,size};
            SDL_RenderCopyEx(renderer,rockTexture,nullptr,&dst,seconds*35,nullptr,SDL_FLIP_NONE);
        } else {
            sfUiCircle(renderer,x,y,size/2,175,185,200);
            sfUiCircle(renderer,x+size/8,y-size/10,std::max(2,size/7),115,125,145);
        }
    };

    switch(kind) {
        case 0: {
            drawShip(cx-unit*2,cy,unit*2);drawShip(cx+unit*2,cy,unit*2);
            sfHelpDrawArrow(renderer,cx-unit,cy,cx+unit,cy,{255,130,120,255});
            break;
        }
        case 1: {
            drawShip(cx,cy,unit*2);
            const int dx=int(std::sin(seconds*1.5f)*unit*2.2f),dy=int(std::cos(seconds*1.1f)*unit*.8f);
            sfHelpDrawArrow(renderer,cx,cy,cx+dx,cy+dy,{120,235,190,255});
            break;
        }
        case 2: {
            SDL_Rect pv{area.x+unit/2,cy-unit,area.w-unit,unit/2};
            SDL_Rect energy{area.x+unit/2,cy+unit/2,area.w-unit,unit/2};
            sfUiPanel(renderer,pv,35,16,24,235,95,90);sfUiPanel(renderer,energy,8,35,54,90,210,245);
            break;
        }
        case 3: {
            drawShip(cx-unit,cy,unit*2);
            for(int i=0;i<7;++i) {
                const int x=cx+int(unit*.7f+i*unit*.32f),y=cy+int(std::sin(seconds*2+i)*unit*.45f);
                sfUiCircle(renderer,x,y,std::max(2,unit/8),235,245,255);
            }
            sfHelpDrawArrow(renderer,cx+unit*2,cy,cx+unit/2,cy,{235,245,255,255});
            break;
        }
        case 4: {
            for(int i=0;i<5;++i) sfUiCircle(renderer,cx-unit*2+i*unit/3,cy-unit/2,std::max(2,unit/8),240,250,255);
            for(int i=0;i<5;++i) sfUiCircle(renderer,cx+unit/2+i*unit/3,cy+unit/2,std::max(2,unit/8),255,95,80);
            break;
        }
        case 5: case 9: {
            drawShip(cx,cy,unit*2);drawRock(cx+unit*2,cy-unit/2,unit*2);
            const int radius=int(unit*(1.2f+std::fmod(seconds,1.5f)*1.6f));
            sfUiCircle(renderer,cx,cy,radius,115,220,255);
            break;
        }
        case 6: {
            drawShip(cx,cy,unit*2);
            const float phase=std::fmod(seconds,3.0f);
            const float charge=std::min(1.0f,phase/2.0f);
            sfUiCircle(renderer,cx,cy,int(unit*(1.1f+charge*1.3f)),Uint8(120+120*charge),Uint8(210-70*charge),255);
            SDL_Rect bar{cx-unit*2,cy+unit*2,unit*4,std::max(5,unit/4)};
            sfUiPanel(renderer,bar,14,18,28,80,110,130);
            SDL_Rect fill=bar;fill.w=int(bar.w*charge);SDL_SetRenderDrawColor(renderer,125,235,255,255);SDL_RenderFillRect(renderer,&fill);
            break;
        }
        case 7: {
            static const char *names[]={"MOU DU GENOU","ROCK N ROLL","APOCALYPSE"};
            for(int i=0;i<3;++i) sfUiText(renderer,area.x+unit/2,area.y+unit/2+i*unit,names[i],std::max(1,unit/14),235,235-i*35,200-i*45);
            break;
        }
        case 8: {
            drawShip(cx,cy,unit*2);
            sfUiCircle(renderer,cx,cy,int(unit*1.45f),95,215,255);
            sfUiCircle(renderer,cx,cy,int(unit*2.05f),235,195,95);
            break;
        }
        case 10: {
            for(int i=0;i<4;++i) {
                const int x=area.x+area.w*(i+1)/5;
                sfUiCircle(renderer,x,cy,unit/2,Uint8(110+i*35),Uint8(220-i*20),Uint8(245-i*35));
                if(i<3) sfHelpDrawArrow(renderer,x+unit/2,cy,area.x+area.w*(i+2)/5-unit/2,cy,{180,220,245,255});
            }
            break;
        }
    }
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
}

static void sfHelpDrawHub(SDL_Renderer *renderer,int width,int height)
{
    sfUiBackground(renderer,width,height);
    const int title=std::max(2,std::min(width/120,height/85));
    const int base=std::max(1,std::min(width/260,height/170));
    sfUiCenteredText(renderer,width,int(height*.06f),"CENTRE D AIDE",title,235,250,255);
    sfUiCenteredText(renderer,width,int(height*.15f),"CHOISISSEZ VOTRE FORMAT",base,150,210,240);
    const char *labels[]={"RAPIDE","DETAILLE","ANIME","TUTORIEL"};
    for(int i=0;i<4;++i) {
        const bool selected=i<3 && int(sfHelpState.format)==i;
        sfHelpDrawButton(renderer,sfHelpHubButtonRect(i,width,height),labels[i],selected);
    }
    sfUiCenteredText(renderer,width,int(height*.87f),"ANIME = DETAIL MAXIMAL",base,175,220,245);
    sfUiCenteredText(renderer,width,int(height*.93f),"RETOUR",base+1,225,245,240);
}

static void sfHelpDrawPage(SDL_Renderer *renderer,int width,int height,SDL_Texture *shipTexture,SDL_Texture *rockTexture)
{
    sfUiBackground(renderer,width,height);
    const auto &page=sfHelpCurrentPage();
    const int title=std::max(2,std::min(width/135,height/95));
    const int base=std::max(1,std::min(width/285,height/190));
    sfUiCenteredText(renderer,width,int(height*.045f),page.title,title,235,250,255);
    sfUiCenteredText(renderer,width,int(height*.15f),page.line1,base,220,238,250);
    sfUiCenteredText(renderer,width,int(height*.205f),page.line2,base,190,220,240);
    SDL_Rect diagram{int(width*.08f),int(height*.29f),int(width*.84f),int(height*.43f)};
    sfUiPanel(renderer,diagram,3,10,24,45,90,120);
    sfHelpDrawDiagram(renderer,page.diagram,diagram,shipTexture,rockTexture);
    const std::string counter=std::to_string(sfHelpState.page+1)+" / "+std::to_string(sfHelpPageCount(sfHelpState.format));
    sfUiCenteredText(renderer,width,int(height*.75f),counter.c_str(),base,135,205,235);
    sfHelpDrawButton(renderer,{int(width*.04f),int(height*.82f),int(width*.27f),int(height*.09f)},"PRECEDENT");
    sfHelpDrawButton(renderer,{int(width*.365f),int(height*.82f),int(width*.27f),int(height*.09f)},"MENU");
    sfHelpDrawButton(renderer,{int(width*.69f),int(height*.82f),int(width*.27f),int(height*.09f)},"SUIVANT");
    const char *format=sfHelpState.format==SfHelpFormat::Quick ? "RAPIDE" : sfHelpState.format==SfHelpFormat::Detailed ? "DETAILLE" : "ANIME";
    sfUiCenteredText(renderer,width,int(height*.94f),format,base,180,220,245);
}

static void sfHelpDraw(SDL_Renderer *renderer,SDL_Texture *shipTexture,SDL_Texture *rockTexture)
{
    if(!renderer) return;
    int width=0,height=0;SDL_GetRendererOutputSize(renderer,&width,&height);
    if(width<=0 || height<=0) return;
    if(sfHelpState.view==SfHelpView::Hub) sfHelpDrawHub(renderer,width,height);
    else sfHelpDrawPage(renderer,width,height,shipTexture,rockTexture);
}

struct SfHelpTextures {
    SDL_Renderer *renderer=nullptr;
    SDL_Texture *ship=nullptr;
    SDL_Texture *rock=nullptr;
};
inline std::vector<SfHelpTextures> sfHelpTextures;

static SfHelpTextures &sfHelpTexturesFor(SDL_Renderer *renderer)
{
    for(auto &entry:sfHelpTextures) if(entry.renderer==renderer) return entry;
    SfHelpTextures entry{};entry.renderer=renderer;
    entry.ship=IMG_LoadTexture(renderer,"resources/assets/pict/remaster/player_blue.png");
    entry.rock=IMG_LoadTexture(renderer,"resources/assets/pict/aa1.png");
    if(entry.ship) SDL_SetTextureBlendMode(entry.ship,SDL_BLENDMODE_BLEND);
    if(entry.rock) SDL_SetTextureBlendMode(entry.rock,SDL_BLENDMODE_BLEND);
    sfHelpTextures.push_back(entry);return sfHelpTextures.back();
}

static void sfHelpDrawActive(SDL_Renderer *renderer)
{
    auto &textures=sfHelpTexturesFor(renderer);
    sfHelpDraw(renderer,textures.ship,textures.rock);
}

static void sfHelpForgetRenderer(SDL_Renderer *renderer)
{
    sfHelpTextures.erase(std::remove_if(sfHelpTextures.begin(),sfHelpTextures.end(),
        [renderer](const auto &entry){return entry.renderer==renderer;}),sfHelpTextures.end());
}

static void sfHelpDrawGameButton(SDL_Renderer *renderer)
{
    if(!renderer) return;
    int width=0,height=0;SDL_GetRendererOutputSize(renderer,&width,&height);
    const SDL_Rect rect=sfHelpGameButtonRect(width,height);
    sfUiPanel(renderer,rect,6,22,42,90,210,245);
    const int scale=std::max(2,std::min(5,rect.h/11));
    sfUiText(renderer,rect.x+(rect.w-sfUiTextWidth("?",scale))/2,
             rect.y+(rect.h-7*scale)/2,"?",scale,240,250,255);
}

static bool sfHelpPointIn(SDL_Rect rect,float normalizedX,float normalizedY,int width,int height)
{
    const int x=int(normalizedX*width),y=int(normalizedY*height);
    return x>=rect.x && x<rect.x+rect.w && y>=rect.y && y<rect.y+rect.h;
}

static void sfHelpHandleTap(float x,float y,int width,int height)
{
    if(sfHelpState.view==SfHelpView::Hub) {
        for(int i=0;i<4;++i) if(sfHelpPointIn(sfHelpHubButtonRect(i,width,height),x,y,width,height)) {
            if(i<3) sfHelpSelectFormat(SfHelpFormat(i));
            else sfHelpState.tutorialRequested=true;
            return;
        }
        if(y>=.88f) sfHelpCloseRequest();
        return;
    }
    if(y>=.80f && y<=.93f) {
        if(x<.33f) sfHelpPrevious();
        else if(x<.67f) sfHelpShowHub();
        else sfHelpNext();
    }
}
