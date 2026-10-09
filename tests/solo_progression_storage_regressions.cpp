#include "../src/solo_progression_storage.hpp"
#include <cassert>
#include <fstream>
#include <string>
#include <cstdio>
int main(){
 using namespace sfsolo;
 const std::string path="solo-progression-test.save";
 std::remove(path.c_str());
 std::remove((path+".tmp").c_str());
 Progression source,restored;
 assert(!loadProgression(path,restored));
 assert(source.complete(1,1,true));
 assert(source.complete(1,2,true));
 assert(saveProgression(path,source));
 assert(loadProgression(path,restored));
 assert(restored.isCompleted(1,1));
 assert(restored.isCompleted(1,2));
 assert(!restored.isCompleted(1,3));
 // A corrupt save must not destroy the last known-good progression.
 {std::ofstream corrupt(path,std::ios::trunc);corrupt<<"SFSOLO1:BAD";}
 assert(!loadProgression(path,restored));
 assert(restored.isCompleted(1,2));
 assert(saveProgression(path,source));
 assert(loadProgression(path,restored));
 assert(!saveProgression("",source));
 std::remove(path.c_str());
 std::remove((path+".tmp").c_str());
}
