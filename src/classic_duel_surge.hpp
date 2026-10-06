#pragma once
#include <array>

// Classic local duel keeps the historical first-finger movement controls.
// A second finger on a player's half owns only that player's shared kinetic
// surge state. This bridge runs before the historical event switch in the
// generated Android copy, so src/main.cpp stays byte-for-byte untouched.
inline std::array<int,2> sfClassicDuelKineticFinger{{-1,-1}};

static void sfClassicDuelSurgeClearStale()
{
    for(int owner=0;owner<2;++owner)
        if(sfClassicDuelKineticFinger[owner]>=0 && !sfKineticSurges[owner].held)
            sfClassicDuelKineticFinger[owner]=-1;
}

static void sfClassicDuelStartSurge(int owner,int fingerId)
{
    if(owner==0) {
        sfKineticSurgePress(0);
        sfKineticAudioStartCharge(0);
    } else {
        sfKineticSurgePress(1);
        sfKineticAudioStartCharge(1);
    }
    sfClassicDuelKineticFinger[owner]=fingerId;
}

static void sfClassicDuelReleaseSurge(int owner)
{
    bool purge=false;
    if(owner==0) purge=sfKineticSurgeRelease(0);
    else purge=sfKineticSurgeRelease(1);
    sfClassicDuelKineticFinger[owner]=-1;

    if(purge) {
        if(owner==0) {
            sfKineticAudioRelease(0);
            sfKineticPurgeAsteroids(0);
        } else {
            sfKineticAudioRelease(1);
            sfKineticPurgeAsteroids(1);
        }
    } else {
        if(owner==0) sfKineticAudioCancel(0);
        else sfKineticAudioCancel(1);
        // A short second-finger tap remains the historical ordinary shot.
        sfFireMain(owner,nullptr);
    }
}

static void sfClassicDuelCancelOwnedSurges()
{
    for(int owner=0;owner<2;++owner) {
        if(sfClassicDuelKineticFinger[owner]<0) continue;
        sfKineticSurgeCancel(owner);
        sfKineticAudioCancel(owner);
        sfClassicDuelKineticFinger[owner]=-1;
    }
}

static bool sfClassicDuelSurgeHandleEvent(const SDL_Event &event,int fingerId,float y)
{
    sfClassicDuelSurgeClearStale();
    if(sfActiveMode!=SF_DUEL_LOCAL) {
        sfClassicDuelCancelOwnedSurges();
        return false;
    }

    if(event.type==SDL_FINGERMOTION) {
        return fingerId==sfClassicDuelKineticFinger[0] ||
               fingerId==sfClassicDuelKineticFinger[1];
    }
    if(event.type==SDL_FINGERUP) {
        if(fingerId==sfClassicDuelKineticFinger[0]) {
            sfClassicDuelReleaseSurge(0);
            return true;
        }
        if(fingerId==sfClassicDuelKineticFinger[1]) {
            sfClassicDuelReleaseSurge(1);
            return true;
        }
        return false;
    }
    if(event.type!=SDL_FINGERDOWN) return false;

    const int owner=y<HEIGHT*.5f ? 0 : 1;
    if(sfClassicDuelKineticFinger[owner]>=0) return true; // ignore a third finger.
    const sprite *ship=owner==0 ? Spritej1 : Spritej2;
    if(!ship || ship->pv<=0) return false;
    // id==100 means there is no primary movement finger yet: let the historical
    // event path claim this as the first finger rather than turning it into a surge.
    if(ship->id==100 || ship->id==fingerId) return false;
    sfClassicDuelStartSurge(owner,fingerId);
    return true;
}
