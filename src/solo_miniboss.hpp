#pragma once
// Lightweight SOLO actors, not a second campaign boss or pilot simulation.
// Included in the historical TU: positions retain Fab's tupl convention.
struct SfSoloMiniboss {
    tupl position,anchor;
    float health=180,radius=24,cooldown=1.1f,warning=0,aim=0,hit=0;
    int appearance=0;
    bool alerted=false;
};
