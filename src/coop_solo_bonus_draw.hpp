#pragma once
// One SOLO renderer for arena and world. Art stays separate from collisions;
// the environment sheet contributes explicit sprites, never collision tiles.
#include <cstdio>

struct SfCoopSoloBonusRenderer {
    SDL_Renderer *renderer;
    SDL_Texture *atlas=nullptr;
    std::array<SDL_Texture*,4> icons{};
    explicit SfCoopSoloBonusRenderer(SDL_Renderer *r):renderer(r){
        atlas=IMG_LoadTexture(r,"resources/assets/pict/solo/EnvironnementsModeSolo.png");
        const char *paths[]={"remaster/heart_blue.png","remaster/energy_ice.png",
                             "remaster/shield_blue.png","missilebb.png"};
        for(unsigned i=0;i<icons.size();++i)
            icons[i]=IMG_LoadTexture(r,(std::string("resources/assets/pict/")+paths[i]).c_str());
    }
    SfCoopSoloBonusRenderer(const SfCoopSoloBonusRenderer &)=delete;
    SfCoopSoloBonusRenderer &operator=(const SfCoopSoloBonusRenderer &)=delete;
    void release(){
        if(atlas)SDL_DestroyTexture(atlas);atlas=nullptr;
        for(auto &t:icons){if(t)SDL_DestroyTexture(t);t=nullptr;}
    }
    ~SfCoopSoloBonusRenderer(){release();}
    static SDL_Color color(sfsolo::bonus::Kind k,Uint8 alpha=255){
        using sfsolo::bonus::Kind;
        switch(k){
        case Kind::Health:return {115,255,155,alpha};case Kind::Energy:return {118,195,255,alpha};
        case Kind::Stealth:return {202,144,255,alpha};case Kind::Missiles:return {255,225,105,alpha};
        case Kind::Prison:return {110,125,255,alpha};case Kind::Ice:return {165,245,255,alpha};
        case Kind::Lava:return {255,104,40,alpha};case Kind::Meteor:return {255,167,108,alpha};
        default:return {255,255,255,alpha};
        }
    }
    void ring(float x,float y,float radius,SDL_Color c,float rotation=0,int sides=32){
        SDL_FPoint points[33];sides=std::clamp(sides,3,32);
        for(int i=0;i<=sides;++i){const float a=rotation+i*2*float(PI)/sides;
            points[i]={x+std::cos(a)*radius,y+std::sin(a)*radius};}
        SDL_SetRenderDrawColor(renderer,c.r,c.g,c.b,c.a);
        SDL_RenderDrawLinesF(renderer,points,sides+1);
    }
    void icon(sfsolo::bonus::Kind k,float x,float y,float side,float rotation=0,Uint8 alpha=255){
        SDL_Texture *texture=nullptr;SDL_Rect source{};bool crop=false;
        const auto n=unsigned(k);
        if(n<icons.size())texture=icons[n];
        else if(sfsolo::bonus::isOrb(k)){
            texture=atlas;crop=true;
            // Hand inspected, non-grid atlas. Crops isolate each motif.
            constexpr SDL_Rect crops[]={{172,66,125,232},{294,906,149,198},
                                        {453,347,167,214},{13,589,183,251}};
            source=crops[n-unsigned(sfsolo::bonus::Kind::Prison)];
        }
        if(!texture){sfCoopDisc(renderer,x,y,side*.35f,color(k,alpha));return;}
        int w=source.w,h=source.h;
        if(!crop)SDL_QueryTexture(texture,nullptr,nullptr,&w,&h);
        const float scale=side/std::max(1,std::max(w,h));
        SDL_FRect rect{x-w*scale*.5f,y-h*scale*.5f,w*scale,h*scale};
        SDL_SetTextureAlphaMod(texture,alpha);
        SDL_RenderCopyExF(renderer,texture,crop?&source:nullptr,&rect,rotation,nullptr,SDL_FLIP_NONE);
        SDL_SetTextureAlphaMod(texture,255);
    }
    void draw(const sfsolo::CoopSoloWorldBridge *world,int width,int height){
        if(!sfCoopSoloBonusesEnabled())return;
        using namespace sfsolo::bonus;
        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
        const auto project=[&](float x,float y){return world?world->toScreen({x,y}):sfsolo::CoopWorldPoint{x,y};};
        const auto visible=[&](sfsolo::CoopWorldPoint p,float r){return p.x+r>=0&&p.y+r>=0&&p.x-r<width&&p.y-r<height;};
        const auto &s=sfCoop.soloBonus;
        for(const auto &pickup:s.pickups){
            const auto p=project(pickup.x,pickup.y);const float r=pickup.radius*1.6f;
            if(!visible(p,r*2))continue;
            const bool expiring=pickup.expires-s.time<5;
            const Uint8 alpha=expiring?Uint8(150+90*std::sin(s.time*9)):Uint8(240);
            sfCoopDisc(renderer,p.x,p.y,r*1.2f,{8,15,31,180});
            ring(p.x,p.y,r*(1+.08f*std::sin(float(s.time)*3+pickup.id)),color(pickup.kind,alpha),float(s.time)*.3f,8);
            icon(pickup.kind,p.x,p.y,r*1.65f,0,alpha);
            const int labelWidth=int(std::string(name(pickup.kind)).size())*12;
            sfCoopText(renderer,int(p.x-labelWidth*.5f),int(p.y+r+5),name(pickup.kind),labelWidth,2,color(pickup.kind));
        }
        for(const auto &shot:sfCoop.shots)if(isOrb(shot.bonusKind)){
            const auto p=project(shot.position.x,shot.position.y);
            if(!visible(p,shot.radius*5))continue;
            const float speed=std::max(1.0f,std::hypot(shot.velocity.vx,shot.velocity.vy));
            for(int i=1;i<=3;++i){
                const float trail=shot.radius*i*1.6f;
                ring(p.x-shot.velocity.vx/speed*trail,p.y-shot.velocity.vy/speed*trail,
                     shot.radius*(1-.18f*i),color(shot.bonusKind,Uint8(150-i*35)),0,8);
            }
            icon(shot.bonusKind,p.x,p.y,shot.radius*3.4f,shot.age*120);
            ring(p.x,p.y,shot.radius*1.5f,color(shot.bonusKind,160),shot.age,12);
        }
        for(const auto &fx:sfCoop.soloBonusImpacts){
            const auto p=project(fx.position.x,fx.position.y);
            const float life=float(std::clamp((fx.until-s.time)/.5,0.0,1.0));
            const float r=sfArenaW*.07f*(1-life+.2f);
            if(!visible(p,r))continue;
            ring(p.x,p.y,r,color(fx.kind,Uint8(220*life)),0,24);
            if(fx.kind==Kind::Meteor || fx.kind==Kind::Lava){
                auto *explosion=sfCoopTextures(renderer).explosion;
                if(explosion){
                    SDL_FRect rect{p.x-r,p.y-r,r*2,r*2};
                    SDL_SetTextureAlphaMod(explosion,Uint8(210*life));
                    SDL_RenderCopyF(renderer,explosion,nullptr,&rect);SDL_SetTextureAlphaMod(explosion,255);
                }
            }
        }
        if(s.stealth() && Spritej2->pv>0){
            const auto p=project(Spritej2->x,Spritej2->y);
            ring(p.x,p.y,sfCoopShipRadius()*1.3f,color(Kind::Stealth,95),float(s.time),6);
        }
        if(sfCoop.bossPresent){
            const auto p=project(sfCoop.position.x,sfCoop.position.y);
            for(Kind k:{Kind::Prison,Kind::Ice,Kind::Lava})if(s.remaining(k)>0 && visible(p,sfCoopBossRadius()*1.8f))
                ring(p.x,p.y,sfCoopBossRadius()*(k==Kind::Prison?1.1f:k==Kind::Ice?1.25f:1.4f),
                     color(k,190),float(s.time)*(k==Kind::Prison?0:.4f),k==Kind::Prison?6:24);
        }
        // Fixed screen HUD; timers never inherit the camera transform.
        const int hudScale=std::clamp(width/220,2,5);
        const int margin=std::max(8,width/30),line=std::max(hudScale*8+8,height/45);
        if(sfCoop.bossPresent){
            const int bossY=int(height*.105f),barWidth=width-2*margin;
            const float ratio=std::clamp(sfCoop.health/sfCoopProfile().health,0.0f,1.0f);
            sfCoopText(renderer,margin,bossY,"BOSS TEST : "+std::to_string(int(std::ceil(sfCoop.health)))+" PV",
                       barWidth,hudScale,{224,207,255,235});
            sfDrawBossRatioBar(renderer,{margin,bossY+hudScale*7+8,barWidth,9},ratio,sfHealthColor(ratio),false);
            sfDrawBossRatioBar(renderer,{margin,bossY+hudScale*7+4,barWidth,3},sfCoop.bossEnergyReserve,sfBossEnergyColor(),false);
            sfDrawBossRatioBar(renderer,{margin,bossY+hudScale*7+18,barWidth,3},sfCoop.bossKineticReserve,sfBossKineticColor(),false);
        }
        int y=height-102-line*3;
        const auto label=[&](Kind k,double seconds){
            char value[64];std::snprintf(value,sizeof(value),"%s %.1f S",name(k),seconds);
            sfCoopText(renderer,margin,y,value,width-margin*2,hudScale,color(k));y+=line;
        };
        if(s.stealth())label(Kind::Stealth,s.remaining(Kind::Stealth));
        if(s.missiles())label(Kind::Missiles,s.remaining(Kind::Missiles));
        if(s.charges){
            const std::string value=std::string(name(s.orb))+" : "+std::to_string(s.charges)+" SALVES X3";
            sfCoopText(renderer,margin,y,value,width-margin*2,hudScale,color(s.orb));
        }
        if(sfCoop.soloCollectUntil>s.time)
            sfCoopText(renderer,margin,int(height*.15f),std::string("BONUS ")+name(sfCoop.soloLastCollected),width-margin*2,hudScale,color(sfCoop.soloLastCollected));
        int statusY=int(height*.20f);
        if(sfCoop.bossPresent)for(Kind k:{Kind::Prison,Kind::Ice,Kind::Lava})if(s.remaining(k)>0){
            char value[80];std::snprintf(value,sizeof(value),"BOSS %s %.1f S%s",name(k),s.remaining(k),world && k!=Kind::Lava?" - TEST FIXE":"");
            sfCoopText(renderer,margin,statusY,value,width-margin*2,hudScale,color(k));statusY+=line;
        }
    }
};
