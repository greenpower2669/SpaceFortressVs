#pragma once
// Pure SOLO campaign navigation. No changes to historical SDL2 menus.
#include "solo_progression.hpp"
#include "solo_map_generator.hpp"

namespace sfsolo {
struct StageSelection {
    unsigned world=1,stage=1,difficulty=1;
};
inline bool selectable(const Progression &progress,const StageSelection &s){
    return s.difficulty>=1&&s.difficulty<=9&&progress.unlocked(s.world,s.stage);
}
inline bool selectStage(const Progression &progress,StageSelection &current,
                        unsigned world,unsigned stage,unsigned difficulty){
    StageSelection candidate{world,stage,difficulty};
    if(!selectable(progress,candidate))return false;
    current=candidate;return true;
}
inline bool advanceStage(const Progression &progress,StageSelection &current){
    StageSelection next=current;
    if(next.stage<Progression::stagesPerWorld)++next.stage;
    else {++next.world;next.stage=1;}
    if(!selectable(progress,next))return false;
    current=next;return true;
}
inline Map mapForSelection(const Progression &progress,const StageSelection &selection){
    if(!selectable(progress,selection))return {};
    MapOptions options;
    options.world=selection.world;
    options.stage=selection.stage;
    options.difficulty=selection.difficulty;
    return generate(options);
}
} // namespace sfsolo
