#pragma once
#include "solo_session.hpp"
namespace sfsolo {
// Integration seam: SOLO owns the map; historical SpaceFortress owns gameplay.
struct CanonicalInput { float x=0,y=0,dt=0; bool fire=false,charge=false; };
struct CanonicalSnapshot { float x=0,y=0,vx=0,vy=0,health=100,heat=0; };
struct CanonicalHooks {
 void *context=nullptr;
 void (*update)(void *,const CanonicalInput &)=nullptr;
 CanonicalSnapshot (*snapshot)(void *)=nullptr;
 void (*collision)(void *,float)=nullptr;
};
struct CanonicalAdapter {
 CanonicalHooks hooks;
 bool available() const {return hooks.context && hooks.update && hooks.snapshot && hooks.collision;}
 void step(Session &s,const CanonicalInput &input) const {
  if(!available() || (s.phase!=Phase::Flying && s.phase!=Phase::BossFight))return;
  CanonicalInput bounded=input;
  bounded.dt=std::clamp(input.dt,0.0f,.05f);
  bounded.x=std::clamp(input.x,-1.0f,1.0f);
  bounded.y=std::clamp(input.y,-1.0f,1.0f);
  s.iceSeconds=std::max(0.0f,s.iceSeconds-bounded.dt);
  if(s.iceActive()){bounded.x*=.25f;bounded.y*=.25f;}
  hooks.update(hooks.context,bounded);
  const auto state=hooks.snapshot(hooks.context);
  s.pilot.x=std::clamp(state.x,s.shipRadius,float(s.map.width)-s.shipRadius);
  s.pilot.y=std::clamp(state.y,s.shipRadius,float(s.map.height)-s.shipRadius);
  s.pilot.vx=state.vx;s.pilot.vy=state.vy;s.pilot.health=state.health;
  s.seconds+=bounded.dt;
  s.collisionCooldown=std::max(0.0f,s.collisionCooldown-bounded.dt);
  const Tile tile=s.map.at(int(s.pilot.x),int(s.pilot.y));
  if(s.collisionCooldown<=0 && (tile==Tile::Rock || tile==Tile::Asteroid || tile==Tile::IceAsteroid || tile==Tile::Lava || tile==Tile::Beam)){
   hooks.collision(hooks.context,tile==Tile::Rock?8.0f:(tile==Tile::Asteroid||tile==Tile::IceAsteroid)?5.0f:tile==Tile::Lava?12.0f:9.0f);
   if(tile==Tile::IceAsteroid)s.iceSeconds=Session::ICE_MALUS_SECONDS;
   s.collisionCooldown=.45f;
  }
  if(tile==Tile::Boss && !s.bossDefeated)s.phase=Phase::BossFight;
  if(tile==Tile::Finish){s.reachedFinish=true;if(s.bossDefeated)s.phase=Phase::Won;}
  if(s.pilot.health<=0)s.phase=Phase::Lost;
  s.cameraY=std::clamp(s.pilot.y+std::clamp(s.pilot.vy*.6f,-3.0f,3.0f),0.0f,float(s.map.height));
 }
};
}
