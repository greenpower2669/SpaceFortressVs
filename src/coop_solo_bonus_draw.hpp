#pragma once
// One SOLO renderer for arena and world. Art stays separate from collisions;
// the environment sheet contributes explicit sprites, never collision tiles.
#include <cstdio>

struct SfCoopSoloBonusRenderer {
    SDL_Renderer *renderer;
    SDL_Texture *atlas=nullptr,*animationAtlas=nullptr;
    std::array<SDL_Texture*,4> icons{};
    explicit SfCoopSoloBonusRenderer(SDL_Renderer *r):renderer(r){reload();}
    void reload(){
        release();SDL_Renderer *r=renderer;
        atlas=IMG_LoadTexture(r,"resources/assets/pict/solo/EnvironnementsModeSolo.png");
        animationAtlas=IMG_LoadTexture(r,"resources/assets/pict/solo/SoloBonusAnimations.png");
        const char *paths[]={"remaster/heart_blue.png","remaster/energy_ice.png",
                             "remaster/shield_blue.png","missilebb.png"};
        for(unsigned i=0;i<icons.size();++i)
            icons[i]=IMG_LoadTexture(r,(std::string("resources/assets/pict/")+paths[i]).c_str());
    }
    SfCoopSoloBonusRenderer(const SfCoopSoloBonusRenderer &)=delete;
    SfCoopSoloBonusRenderer &operator=(const SfCoopSoloBonusRenderer &)=delete;
    void release(){
        if(atlas)SDL_DestroyTexture(atlas);atlas=nullptr;
        if(animationAtlas)SDL_DestroyTexture(animationAtlas);animationAtlas=nullptr;
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
    static SDL_Rect animatedCrop(sfsolo::bonus::Kind k,int frame,bool impact=false){
        using sfsolo::bonus::Kind;
        frame=std::clamp(frame,0,7);
        static const SDL_Rect rock[8]={
            {13,590,182,247},{184,590,148,247},{334,603,147,230},{490,590,128,236},
            {607,611,141,209},{740,608,133,235},{880,632,120,186},{1007,603,103,229}};
        static const SDL_Rect ice[8]={
            {12,857,150,247},{161,903,145,193},{299,919,148,176},{452,898,150,205},
            {601,903,158,204},{762,887,102,215},{866,887,123,214},{989,887,123,214}};
        static const SDL_Rect lava[8]={
            {13,326,155,224},{175,340,145,220},{325,340,126,220},{454,340,159,216},
            {611,340,126,221},{740,340,136,221},{873,312,128,242},{1006,340,107,219}};
        static const SDL_Rect smoke[8]={
            {21,1131,171,233},{178,1155,155,194},{336,1120,142,254},{446,1176,167,173},
            {573,1208,201,156},{764,1144,203,221},{972,1121,136,263},{972,1121,136,263}};
        if(k==Kind::Prison)return rock[frame];
        if(k==Kind::Ice)return ice[frame];
        if(k==Kind::Lava)return lava[frame];
        if(k==Kind::Meteor)return impact?smoke[frame]:rock[(frame+2)&7];
        return {};
    }
    static int productionAnimationRow(sfsolo::bonus::Kind k){
        using sfsolo::bonus::Kind;
        switch(k){
        case Kind::Prison:return 0;case Kind::Ice:return 1;
        case Kind::Lava:return 2;case Kind::Meteor:return 3;
        default:return -1;
        }
    }
    static SDL_Rect productionAnimationCrop(sfsolo::bonus::Kind k,int frame){
        const int row=productionAnimationRow(k);
        if(row<0)return {};
        frame=std::clamp(frame,0,7);
        return {frame*192,row*192,192,192};
    }
    void animatedSprite(sfsolo::bonus::Kind k,int frame,float x,float y,float side,
                        float rotation=0,Uint8 alpha=255,bool impact=false){
        if(animationAtlas && productionAnimationRow(k)>=0){
            const SDL_Rect source=productionAnimationCrop(k,frame);
            SDL_FRect rect{x-side*.5f,y-side*.5f,side,side};
            SDL_SetTextureAlphaMod(animationAtlas,alpha);
            SDL_RenderCopyExF(renderer,animationAtlas,&source,&rect,rotation,nullptr,SDL_FLIP_NONE);
            SDL_SetTextureAlphaMod(animationAtlas,255);
            return;
        }
        // Safety fallback for old packages: keep the authored source motifs.
        if(!atlas)return;
        const SDL_Rect source=animatedCrop(k,frame,impact);
        if(source.w<=0||source.h<=0)return;
        const float scale=side/std::max(1,std::max(source.w,source.h));
        SDL_FRect rect{x-source.w*scale*.5f,y-source.h*scale*.5f,
                       source.w*scale,source.h*scale};
        SDL_SetTextureAlphaMod(atlas,alpha);
        SDL_RenderCopyExF(renderer,atlas,&source,&rect,rotation,nullptr,SDL_FLIP_NONE);
        SDL_SetTextureAlphaMod(atlas,255);
    }
    void icon(sfsolo::bonus::Kind k,float x,float y,float side,float rotation=0,Uint8 alpha=255){
        SDL_Texture *texture=nullptr;SDL_Rect source{};bool crop=false;
        const auto n=unsigned(k);
        if(n<icons.size())texture=icons[n];
        else if(sfsolo::bonus::isOrb(k)){
            // Collectibles use one clear representative cell; combat plays all
            // eight production frames.
            crop=true;
            const int representative=k==sfsolo::bonus::Kind::Prison?6:
                                     k==sfsolo::bonus::Kind::Ice?6:
                                     k==sfsolo::bonus::Kind::Lava?5:2;
            if(animationAtlas){
                texture=animationAtlas;source=productionAnimationCrop(k,representative);
            }else{
                texture=atlas;source=animatedCrop(k,representative,false);
            }
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
        // BONUS-013: reward visuals are born at the boss and visibly travel
        // to their already-validated landing cells. The real pickup remains
        // hidden and non-collectable until this short ejection completes.
        for(const auto &e:sfCoop.soloBonusEjections){
            const double duration=std::max(.001,e.until-e.started);
            const float t=float(std::clamp((s.time-e.started)/duration,0.0,1.0));
            const float smooth=t*t*(3.0f-2.0f*t);
            const float dx=e.target.x-e.origin.x,dy=e.target.y-e.origin.y;
            const float length=std::max(1.0f,std::hypot(dx,dy));
            const float direction=(e.pickupId&1u)?1.0f:-1.0f;
            // WORLD landing cells have a tested wall-free boss segment; keep
            // that exact line. Arena may use a small decorative side arc.
            const float arc=world?0.0f:
                std::sin(t*float(PI))*std::min(sfArenaW*.035f,length*.10f)*direction;
            const float x=e.origin.x+dx*smooth-dy/length*arc;
            const float y=e.origin.y+dy*smooth+dx/length*arc;
            const auto p=project(x,y);
            const float r=std::max(10.0f,sfArenaW*.023f)*1.6f;
            if(!visible(p,r*2))continue;
            const Uint8 alpha=Uint8(205+35*std::sin(t*float(PI)));
            ring(p.x,p.y,r*(.72f+.25f*t),color(e.kind,alpha),t*5.0f,8);
            icon(e.kind,p.x,p.y,r*1.55f,t*150.0f,alpha);
        }
        for(const auto &pickup:s.pickups){
            const bool flying=std::any_of(sfCoop.soloBonusEjections.begin(),sfCoop.soloBonusEjections.end(),
                [&](const SfSoloBonusEjection &e){return e.pickupId==pickup.id && e.until>s.time;});
            if(flying)continue;
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
            if(!visible(p,shot.radius*6))continue;
            const float speed=std::max(1.0f,std::hypot(shot.velocity.vx,shot.velocity.vy));
            for(int i=1;i<=3;++i){
                const float trail=shot.radius*i*1.6f;
                ring(p.x-shot.velocity.vx/speed*trail,p.y-shot.velocity.vy/speed*trail,
                     shot.radius*(1-.18f*i),color(shot.bonusKind,Uint8(150-i*35)),0,8);
            }
            // Flight is deliberately limited to the four approach frames.
            // Frames 4..7 are impact/closure frames and must never flash early.
            const int frame=int(std::floor(shot.age*11.0f))&3;
            if(shot.bonusKind==Kind::Meteor){
                const float tail=shot.radius*2.3f;
                animatedSprite(Kind::Lava,frame,
                    p.x-shot.velocity.vx/speed*tail,p.y-shot.velocity.vy/speed*tail,
                    shot.radius*4.0f,shot.age*180,145,false);
                animatedSprite(Kind::Meteor,frame,p.x,p.y,shot.radius*4.8f,shot.age*150,255,false);
            }else{
                animatedSprite(shot.bonusKind,frame,p.x,p.y,shot.radius*4.4f,shot.age*150,245,false);
            }
            ring(p.x,p.y,shot.radius*1.5f,color(shot.bonusKind,150),shot.age,12);
        }
        for(const auto &fx:sfCoop.soloBonusImpacts){
            const auto p=project(fx.position.x,fx.position.y);
            const float life=float(std::clamp((fx.until-s.time)/.5,0.0,1.0));
            const float progress=1.0f-life;
            const float r=sfArenaW*.07f*(progress+.2f);
            if(!visible(p,r*2))continue;
            if(isOrb(fx.kind)){
                const int frame=std::clamp(int(progress*8.0f),0,7);
                const float side=std::max(sfArenaW*.095f,sfCoopBossRadius()*2.15f);
                animatedSprite(fx.kind,frame,p.x,p.y,side,progress*55,Uint8(235*life+20),true);
                // The production atlas already contains the authored lava/smoke
                // impact. Do not cover it with the old generic explosion sprite.
            }else ring(p.x,p.y,r,color(fx.kind,Uint8(220*life)),0,24);
        }
        if(s.stealth() && Spritej2->pv>0){
            const auto p=project(Spritej2->x,Spritej2->y);
            ring(p.x,p.y,sfCoopShipRadius()*1.3f,color(Kind::Stealth,95),float(s.time),6);
        }
        if(sfCoop.bossPresent){
            const auto p=project(sfCoop.position.x,sfCoop.position.y);
            const float bossR=sfCoopBossRadius();
            for(Kind k:{Kind::Prison,Kind::Ice,Kind::Lava})if(s.remaining(k)>0 && visible(p,bossR*2.3f)){
                // Hold the closing production frames around the actual boss:
                // rock cage, crystal lock or lava envelope.
                const int frame=5+(int(s.time*4.0+unsigned(k))%3);
                const float side=bossR*(k==Kind::Prison?3.55f:k==Kind::Ice?3.35f:3.15f);
                animatedSprite(k,frame,p.x,p.y,side,float(s.time)*(k==Kind::Prison?2.0f:8.0f),
                               k==Kind::Prison?235:215,true);
                ring(p.x,p.y,bossR*(k==Kind::Prison?1.08f:k==Kind::Ice?1.18f:1.30f),
                     color(k,105),float(s.time)*.20f,k==Kind::Prison?8:24);
            }
        }
        // Fixed screen HUD; timers never inherit the camera transform.
        const int hudScale=std::clamp(width/220,2,5);
        const int margin=std::max(8,width/30),line=std::max(hudScale*8+8,height/45);
        if(sfCoop.bossPresent){
            const int bossY=int(height*.105f),barWidth=width-2*margin;
            const float ratio=std::clamp(sfCoop.health/sfCoopProfile().health,0.0f,1.0f);
            sfCoopText(renderer,margin,bossY,std::string(sfCoop.soloDiagnostic?"BOSS TEST : ":"BOSS FINAL : ")+std::to_string(int(std::ceil(sfCoop.health)))+" PV",
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
