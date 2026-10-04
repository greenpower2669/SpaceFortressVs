#pragma once

// Isolated interactive tutorial. This runtime owns every value it mutates: it
// demonstrates the real controls and timing without touching a live match,
// campaign progression, Hall of Fame, asteroid/projectile lists or ship state.

#include <SDL2/SDL.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <string>

extern float tw,th;

enum class SfTutorialStep {
    Move=0, Fire, Hud, Energy, HullShield, Dust, Kinetic,
    SurgeHold, SurgeReady, SurgeRelease, Danger, Complete
};

enum class SfTutorialModule {
    All=-1, Move=0, Fire, Hud, Energy, HullShield, Dust, Kinetic,
    SurgeHold, SurgeReady, SurgeRelease, Danger
};

struct SfTutorialState {
    bool active=false;
    bool selecting=false;
    bool all=true;
    SfTutorialStep step=SfTutorialStep::Move;
    SDL_FingerID primary=-1;
    SDL_FingerID secondary=-1;
    float startX=0,startY=0;
    float hold=0;
    float releaseTime=0;
    float demoX=.30f,demoY=.66f;
    float demoEnergy=0;
    float demoPv=1000;
    bool charged=false;
};
inline SfTutorialState sfTutorialState;
inline Uint64 sfTutorialLastTick=0;

static const char *sfTutorialStepTitle(SfTutorialStep step)
{
    static const char *titles[]={
        "DEPLACEMENT","TIR","HUD","ENERGIE","PV ET PROTECTION","POUSSIERES",
        "BOUCLIER CINETIQUE","SURCHARGE 2 DOIGTS","SURCHARGE PRETE",
        "RELACHEMENT","DANGER","TUTORIEL TERMINE"
    };
    return titles[std::clamp(int(step),0,int(SfTutorialStep::Complete))];
}

static const char *sfTutorialStepLine(SfTutorialStep step)
{
    switch(step) {
        case SfTutorialStep::Move: return "POSEZ UN DOIGT PUIS GLISSEZ";
        case SfTutorialStep::Fire: return "TOUCHEZ AVEC UN NOUVEAU DOIGT";
        case SfTutorialStep::Hud: return "TOUCHEZ LA ZONE HUD EN SURBRILLANCE";
        case SfTutorialStep::Energy: return "ENERGIE PLEINE VERS EPUISEE - TOUCHEZ";
        case SfTutorialStep::HullShield: return "PV ET PROTECTIONS - TOUCHEZ";
        case SfTutorialStep::Dust: return "BLANCHE RECHARGE PUIS SOIGNE - ROUGE VISUELLE";
        case SfTutorialStep::Kinetic: return "LES IMPACTS RAPIDES DECLENCHENT LES VAGUES";
        case SfTutorialStep::SurgeHold: return "MAINTENEZ LE DEUXIEME DOIGT 2 SECONDES";
        case SfTutorialStep::SurgeReady: return "PRET : CHAMP OFF JUSQU AU RELACHEMENT";
        case SfTutorialStep::SurgeRelease: return "RELACHE : PURGE DES ASTEROIDES DANS LA ZONE";
        case SfTutorialStep::Danger: return "LE NOM CHANGE LES DEGATS HOSTILES NON CINETIQUES";
        case SfTutorialStep::Complete: return "BRAVO - RETOURNEZ AU CENTRE D AIDE";
    }
    return "";
}

static SDL_Rect sfTutorialExitRect(int width,int height)
{
    const int side=std::max(44,std::min(70,std::min(width,height)/7));
    const int margin=std::max(6,std::min(width,height)/45);
    return {std::max(0,width-margin-side),margin,side,side};
}

static SDL_Rect sfTutorialTargetRect(int width,int height)
{
    return {int(width*.10f),int(height*.31f),std::max(44,int(width*.80f)),std::max(44,int(height*.39f))};
}

static SDL_Rect sfTutorialModuleRect(int index,int width,int height)
{
    index=std::clamp(index,0,11);
    const bool portrait=height>=width;
    const int columns=portrait ? 2 : 4;
    const int rows=portrait ? 6 : 3;
    const int gap=std::max(5,std::min(width,height)/70);
    const int top=int(height*.16f),bottom=int(height*.90f);
    const int usableW=width-gap*(columns+1);
    const int usableH=bottom-top-gap*(rows+1);
    const int w=std::max(44,usableW/columns),h=std::max(44,usableH/rows);
    const int col=index%columns,row=index/columns;
    return {gap+col*(w+gap),top+gap+row*(h+gap),w,h};
}

static bool sfTutorialPointIn(SDL_Rect r,float x,float y,int width,int height)
{
    const int px=int(x*width),py=int(y*height);
    return px>=r.x && px<r.x+r.w && py>=r.y && py<r.y+r.h;
}

static SfTutorialStep sfTutorialStepForModule(SfTutorialModule module)
{
    if(module==SfTutorialModule::All) return SfTutorialStep::Move;
    return SfTutorialStep(std::clamp(int(module),0,int(SfTutorialStep::Danger)));
}

static void sfTutorialStart(SfTutorialModule module)
{
    sfTutorialState={};
    sfTutorialState.active=true;
    sfTutorialState.selecting=false;
    sfTutorialState.all=module==SfTutorialModule::All;
    sfTutorialState.step=sfTutorialStepForModule(module);
    sfTutorialLastTick=SDL_GetTicks64();
}

static void sfTutorialOpenSelector()
{
    sfTutorialState={};
    sfTutorialState.active=true;
    sfTutorialState.selecting=true;
    sfTutorialLastTick=SDL_GetTicks64();
}

static void sfTutorialExit()
{
    sfTutorialState={};
    sfTutorialLastTick=0;
}

static void sfTutorialResetGesture()
{
    sfTutorialState.primary=-1;
    sfTutorialState.secondary=-1;
    sfTutorialState.hold=0;
    sfTutorialState.releaseTime=0;
    sfTutorialState.charged=false;
}

static void sfTutorialAdvance()
{
    if(!sfTutorialState.active || sfTutorialState.selecting || sfTutorialState.step==SfTutorialStep::Complete) return;
    if(!sfTutorialState.all) {
        sfTutorialState.step=SfTutorialStep::Complete;
        sfTutorialResetGesture();
        return;
    }
    const int next=std::min(int(SfTutorialStep::Complete),int(sfTutorialState.step)+1);
    sfTutorialState.step=SfTutorialStep(next);
    // Keep the held second finger specifically across Hold -> Ready.
    if(sfTutorialState.step!=SfTutorialStep::SurgeReady) sfTutorialResetGesture();
}

static void sfTutorialTick(float dt)
{
    if(!sfTutorialState.active || sfTutorialState.selecting) return;
    dt=std::clamp(dt,0.0f,.25f);
    if(sfTutorialState.step==SfTutorialStep::SurgeHold && sfTutorialState.secondary!=-1) {
        sfTutorialState.hold+=dt;
        if(sfTutorialState.hold>=2.0f) {
            sfTutorialState.hold=2.0f;
            sfTutorialState.charged=true;
            sfTutorialState.step=SfTutorialStep::SurgeReady;
        }
    } else if(sfTutorialState.step==SfTutorialStep::SurgeRelease) {
        sfTutorialState.releaseTime+=dt;
        if(sfTutorialState.releaseTime>=.35f) sfTutorialAdvance();
    }
}

static bool sfTutorialHandleEvent(SDL_Event *event)
{
    if(!event || !sfTutorialState.active) return false;
    const int width=std::max(1,int(tw)),height=std::max(1,int(th));

    if(event->type==SDL_KEYDOWN &&
       (event->key.keysym.sym==SDLK_ESCAPE || event->key.keysym.sym==SDLK_AC_BACK)) {
        sfTutorialExit();event->type=SDL_USEREVENT;return true;
    }
    if(event->type!=SDL_FINGERDOWN && event->type!=SDL_FINGERMOTION && event->type!=SDL_FINGERUP) return false;

    const float x=event->tfinger.x,y=event->tfinger.y;
    const SDL_FingerID id=event->tfinger.fingerId;
    if(event->type==SDL_FINGERDOWN && sfTutorialPointIn(sfTutorialExitRect(width,height),x,y,width,height)) {
        sfTutorialExit();event->type=SDL_USEREVENT;return true;
    }

    if(sfTutorialState.selecting) {
        if(event->type==SDL_FINGERDOWN) {
            for(int i=0;i<12;++i) if(sfTutorialPointIn(sfTutorialModuleRect(i,width,height),x,y,width,height)) {
                sfTutorialStart(i==0 ? SfTutorialModule::All : SfTutorialModule(i-1));
                event->type=SDL_USEREVENT;return true;
            }
        }
        event->type=SDL_USEREVENT;return true;
    }

    switch(sfTutorialState.step) {
        case SfTutorialStep::Move:
            if(event->type==SDL_FINGERDOWN && sfTutorialState.primary==-1) {
                sfTutorialState.primary=id;sfTutorialState.startX=x;sfTutorialState.startY=y;
                sfTutorialState.demoX=x;sfTutorialState.demoY=y;
            } else if(event->type==SDL_FINGERMOTION && id==sfTutorialState.primary) {
                sfTutorialState.demoX=x;sfTutorialState.demoY=y;
                if(std::hypot(x-sfTutorialState.startX,y-sfTutorialState.startY)>=.14f) sfTutorialAdvance();
            }
            break;
        case SfTutorialStep::Fire:
            if(event->type==SDL_FINGERDOWN) sfTutorialAdvance();
            break;
        case SfTutorialStep::Hud: case SfTutorialStep::Energy: case SfTutorialStep::HullShield:
        case SfTutorialStep::Dust: case SfTutorialStep::Kinetic: case SfTutorialStep::Danger:
            if(event->type==SDL_FINGERDOWN && sfTutorialPointIn(sfTutorialTargetRect(width,height),x,y,width,height)) sfTutorialAdvance();
            break;
        case SfTutorialStep::SurgeHold:
            if(event->type==SDL_FINGERDOWN && sfTutorialState.secondary==-1) {
                sfTutorialState.secondary=id;sfTutorialState.hold=0;
            } else if(event->type==SDL_FINGERUP && id==sfTutorialState.secondary) {
                sfTutorialState.secondary=-1;sfTutorialState.hold=0;
            }
            break;
        case SfTutorialStep::SurgeReady:
            if(event->type==SDL_FINGERUP && id==sfTutorialState.secondary) {
                sfTutorialState.secondary=-1;
                sfTutorialState.step=SfTutorialStep::SurgeRelease;
                sfTutorialState.releaseTime=0;
            }
            break;
        case SfTutorialStep::SurgeRelease: case SfTutorialStep::Complete:
            break;
    }
    event->type=SDL_USEREVENT;
    return true;
}

static const char *sfTutorialModuleLabel(int index)
{
    static const char *labels[]={"TOUT FAIRE","DEPLACER","TIR","HUD","ENERGIE","PV BOUCLIER",
        "POUSSIERES","CINETIQUE","TENIR 2 DOIGTS","PRET","RELACHER","DANGER"};
    return labels[std::clamp(index,0,11)];
}

static void sfTutorialDrawSelector(SDL_Renderer *renderer,int width,int height)
{
    sfUiBackground(renderer,width,height);
    const int title=std::max(2,std::min(width/125,height/90));
    sfUiCenteredText(renderer,width,int(height*.05f),"TUTORIEL",title,235,250,255);
    sfUiCenteredText(renderer,width,int(height*.115f),"TOUT FAIRE OU UN MODULE",std::max(1,title/2),160,215,245);
    for(int i=0;i<12;++i) sfHelpDrawButton(renderer,sfTutorialModuleRect(i,width,height),sfTutorialModuleLabel(i),i==0);
    const SDL_Rect exit=sfTutorialExitRect(width,height);
    sfHelpDrawButton(renderer,exit,"X");
}

static void sfTutorialDrawDemo(SDL_Renderer *renderer,SDL_Rect area)
{
    const int cx=area.x+area.w/2,cy=area.y+area.h/2;
    const int u=std::max(8,std::min(area.w,area.h)/7);
    const auto step=sfTutorialState.step;
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    if(step==SfTutorialStep::Move || step==SfTutorialStep::Fire) {
        const int sx=area.x+int(area.w*sfTutorialState.demoX),sy=area.y+int(area.h*.55f);
        sfUiCircle(renderer,std::clamp(sx,area.x+u,area.x+area.w-u),sy,u,95,210,255);
        if(step==SfTutorialStep::Fire) sfHelpDrawArrow(renderer,cx-u*2,cy,cx+u*2,cy,{255,145,100,255});
    } else if(step==SfTutorialStep::Hud || step==SfTutorialStep::Energy || step==SfTutorialStep::HullShield) {
        SDL_Rect pv{area.x+u,cy-u,area.w-u*2,std::max(8,u/2)};
        SDL_Rect energy{area.x+u,cy+u/2,area.w-u*2,std::max(8,u/2)};
        sfUiPanel(renderer,pv,38,14,25,235,90,80);sfUiPanel(renderer,energy,8,36,56,85,210,245);
    } else if(step==SfTutorialStep::Dust) {
        for(int i=0;i<6;++i) sfUiCircle(renderer,cx-u*2+i*u/2,cy-u/2,std::max(3,u/8),235,248,255);
        for(int i=0;i<6;++i) sfUiCircle(renderer,cx-u*2+i*u/2,cy+u/2,std::max(3,u/8),255,95,80);
    } else if(step==SfTutorialStep::Kinetic) {
        sfUiCircle(renderer,cx,cy,u,100,215,255);sfUiCircle(renderer,cx,cy,int(u*2.0f),115,225,255);
        sfUiCircle(renderer,cx+u*3,cy,u,170,180,195);
    } else if(step==SfTutorialStep::SurgeHold || step==SfTutorialStep::SurgeReady || step==SfTutorialStep::SurgeRelease) {
        const float charge=step==SfTutorialStep::SurgeHold ? std::clamp(sfTutorialState.hold/2.0f,0.0f,1.0f) : 1.0f;
        sfUiCircle(renderer,cx,cy,u,95,210,255);
        sfUiCircle(renderer,cx,cy,int(u*(1.2f+1.2f*charge)),Uint8(120+120*charge),Uint8(215-65*charge),255);
        if(step==SfTutorialStep::SurgeRelease) sfUiCircle(renderer,cx,cy,int(u*(2.4f+sfTutorialState.releaseTime*4*u)),245,225,130);
        SDL_Rect bar{cx-u*2,area.y+area.h-u,u*4,std::max(7,u/3)};
        sfUiPanel(renderer,bar,15,20,30,70,95,120);SDL_Rect fill=bar;fill.w=int(bar.w*charge);
        SDL_SetRenderDrawColor(renderer,125,235,255,255);SDL_RenderFillRect(renderer,&fill);
    } else if(step==SfTutorialStep::Danger) {
        const char *names[]={"MOU DU GENOU","ROCK N ROLL","APOCALYPSE"};
        for(int i=0;i<3;++i) sfUiText(renderer,area.x+u,area.y+u+i*u,names[i],std::max(1,u/14),235,230-i*25,190-i*30);
    } else {
        sfUiCircle(renderer,cx,cy,u*2,100,235,180);sfUiText(renderer,cx-u,cy-u/2,"OK",std::max(2,u/8),240,255,245);
    }
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
}

static void sfTutorialDraw(SDL_Renderer *renderer)
{
    if(!renderer || !sfTutorialState.active) return;
    int width=0,height=0;SDL_GetRendererOutputSize(renderer,&width,&height);
    if(width<=0 || height<=0) return;
    if(sfTutorialState.selecting) {sfTutorialDrawSelector(renderer,width,height);return;}
    sfUiBackground(renderer,width,height);
    const int title=std::max(2,std::min(width/125,height/90));
    const int base=std::max(1,std::min(width/280,height/185));
    sfUiCenteredText(renderer,width,int(height*.045f),sfTutorialStepTitle(sfTutorialState.step),title,235,250,255);
    sfUiCenteredText(renderer,width,int(height*.14f),sfTutorialStepLine(sfTutorialState.step),base,185,225,245);
    const SDL_Rect target=sfTutorialTargetRect(width,height);
    sfUiPanel(renderer,target,3,11,24,55,125,155);
    sfTutorialDrawDemo(renderer,target);
    const int ordinal=std::min(11,int(sfTutorialState.step)+1);
    const std::string progress=sfTutorialState.all ? std::to_string(ordinal)+" / 11" : "MODULE";
    sfUiCenteredText(renderer,width,int(height*.76f),progress.c_str(),base,140,205,235);
    if(sfTutorialState.step==SfTutorialStep::Complete)
        sfUiCenteredText(renderer,width,int(height*.84f),"X POUR RETOURNER A L AIDE",base,190,235,215);
    const SDL_Rect exit=sfTutorialExitRect(width,height);sfHelpDrawButton(renderer,exit,"X");
}
