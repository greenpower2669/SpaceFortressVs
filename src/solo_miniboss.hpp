#pragma once
// Lightweight SOLO actors, not a second campaign boss or pilot simulation.
// Included in the historical TU: positions retain Fab's tupl convention.
struct SfSoloMiniboss {
    tupl position,anchor,lastSeen,searchTarget;
    std::vector<tupl> route;
    unsigned waypoint=0;
    int role=0,pack=-1;
    float memory=0,repath=0,searchTime=0,patrolClock=0;
    bool searching=false;
    float health=180,radius=24,cooldown=1.1f,warning=0,aim=0,hit=0;
    int appearance=0;
    bool alerted=false;
};
