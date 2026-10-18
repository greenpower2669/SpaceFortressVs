#include "../src/solo_rock_material_variation.hpp"
#include <cassert>
#include <cmath>
int main(){
  using namespace sfsolo;
  for(float x:{-.02f,0.f,1.f,2.f,8.f,16.f,39.f}){
    auto a=soloRockMaterialAt(x-.0001f,3.7f),b=soloRockMaterialAt(x+.0001f,3.7f);
    assert(std::abs(a.sourceX-b.sourceX)<.003f);
    assert(std::abs(a.sourceY-b.sourceY)<.003f);
    assert(std::abs(a.shade-b.shade)<.003f);
    assert(std::abs(a.altWeight-b.altWeight)<.003f);
  }
  float minShade=2,maxShade=0,minGlow=2,maxGlow=0;
  int different=0;
  for(int y=0;y<72;++y)for(int x=0;x<65;++x){
    auto a=soloRockMaterialAt(x*.41f,y*.37f);
    assert(std::isfinite(a.sourceX)&&std::isfinite(a.sourceY));
    assert(a.altWeight>=0&&a.altWeight<=.36f);
    assert(a.blueWeight>=0&&a.blueWeight<=1);
    minShade=std::min(minShade,a.shade);maxShade=std::max(maxShade,a.shade);
    minGlow=std::min(minGlow,a.blueWeight);maxGlow=std::max(maxGlow,a.blueWeight);
    auto b=soloRockMaterialAt(x*.41f+2.f,y*.37f);
    if(std::abs(a.sourceX+2.f-b.sourceX)>.03f)++different;
  }
  assert(different>1000);
  assert(maxShade-minShade>.12f);
  assert(minGlow==0 && maxGlow>.5f);
}
