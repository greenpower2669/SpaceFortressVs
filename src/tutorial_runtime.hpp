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
    // Tutorial timing represents real continuous hold time. Cap only pathological
    // frame gaps at the full 2 s threshold; a legitimate long frame must not
    // turn a real 2 s hold into eight artificial 250 ms frames.
    dt=std::clamp(dt,0.0f,2.0f);
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
    static const char *labels[]={"TOUT FAIRE","DEPLACEMENT","TIR","HUD","ENERGIE","PV + BOUCLIER","POUSSIERES","CINETIQUE","SURCHARGE","PRET","RELACHEMENT","DANGER"};
    return labels[std::clamp(index,0,11)];
}

static void sfTutorialDraw(SDL_Renderer *renderer)
{
    if(!renderer || !sfTutorialState.active) return;
    int width=0,height=0;SDL_GetRendererOutputSize(renderer,&width,&height);
    if(width<=0 || height<=0) return;
    sfUiBackground(renderer,width,height);
    const int base=std::max(1,std::min(width/285,height/190));
    const int title=std::max(2,std::min(width/130,height/92));
    sfHelpDrawButton(renderer,sfTutorialExitRect(width,height),"X");

    if(sfTutorialState.selecting) {
        sfUiCenteredText(renderer,width,int(height*.055f),"TUTORIEL IN-GAME",title,235,250,255);
        sfUiCenteredText(renderer,width,int(height*.115f),"CHOISISSEZ UN MODULE OU TOUT FAIRE",base,165,215,240);
        for(int i=0;i<12;++i) sfHelpDrawButton(renderer,sfTutorialModuleRect(i,width,height),sfTutorialModuleLabel(i),i==0);
        return;
    }

    sfUiCenteredText(renderer,width,int(height*.055f),sfTutorialStepTitle(sfTutorialState.step),title,235,250,255);
    sfUiCenteredText(renderer,width,int(height*.145f),sfTutorialStepLine(sfTutorialState.step),base,190,225,245);
    const SDL_Rect target=sfTutorialTargetRect(width,height);
    sfUiPanel(renderer,target,3,10,24,65,135,170);
    const int cx=target.x+target.w/2,cy=target.y+target.h/2;
    const int unit=std::max(8,std::min(target.w,target.h)/7);

    switch(sfTutorialState.step) {
        case SfTutorialStep::Move: {
            const int x=target.x+int(sfTutorialState.demoX*target.w),y=target.y+int((sfTutorialState.demoY-.5f)*target.h);
            sfUiCircle(renderer,std::clamp(x,target.x+unit,target.x+target.w-unit),std::clamp(y,target.y+unit,target.y+target.h-unit),unit,105,220,255);
            sfHelpDrawArrow(renderer,target.x+unit,cy,target.x+target.w-unit,cy,{120,235,190,255});break;
        }
        case SfTutorialStep::Fire: sfUiCircle(renderer,cx,cy,unit,255,170,85);break;
        case SfTutorialStep::Hud: case SfTutorialStep::Energy: case SfTutorialStep::HullShield:
            sfHelpDrawDiagram(renderer,sfTutorialState.step==SfTutorialStep::Hud ? 2 : sfTutorialState.step==SfTutorialStep::Energy ? 3 : 8,target,nullptr,nullptr);break;
        case SfTutorialStep::Dust: sfHelpDrawDiagram(renderer,4,target,nullptr,nullptr);break;
        case SfTutorialStep::Kinetic: sfHelpDrawDiagram(renderer,5,target,nullptr,nullptr);break;
        case SfTutorialStep::SurgeHold: case SfTutorialStep::SurgeReady: case SfTutorialStep::SurgeRelease: {
            sfHelpDrawDiagram(renderer,6,target,nullptr,nullptr);
            const float progress=sfTutorialState.step==SfTutorialStep::SurgeHold ? sfTutorialState.hold/2.0f : 1.0f;
            SDL_Rect bar{target.x+unit,target.y+target.h-unit,target.w-2*unit,std::max(6,unit/3)};
            sfUiPanel(renderer,bar,10,18,28,70,100,120);SDL_Rect fill=bar;fill.w=int(bar.w*progress);
            SDL_SetRenderDrawColor(renderer,125,235,255,255);SDL_RenderFillRect(renderer,&fill);break;
        }
        case SfTutorialStep::Danger: sfHelpDrawDiagram(renderer,7,target,nullptr,nullptr);break;
        case SfTutorialStep::Complete:
            sfUiCenteredText(renderer,width,cy,"VOUS CHOISISSEZ QUOI APPRENDRE",base+1,130,235,190);break;
    }
    if(sfTutorialState.step!=SfTutorialStep::Move && sfTutorialState.step!=SfTutorialStep::Fire &&
       sfTutorialState.step!=SfTutorialStep::SurgeHold && sfTutorialState.step!=SfTutorialStep::SurgeReady &&
       sfTutorialState.step!=SfTutorialStep::SurgeRelease && sfTutorialState.step!=SfTutorialStep::Complete)
        sfUiCenteredText(renderer,width,int(height*.78f),"TOUCHEZ LA ZONE POUR CONTINUER",base,170,220,245);
    sfUiCenteredText(renderer,width,int(height*.91f),sfTutorialState.all ? "PARCOURS COMPLET" : "MODULE LIBRE",base,145,200,230);
}
