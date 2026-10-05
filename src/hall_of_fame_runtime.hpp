#pragma once

#include "hall_of_fame.hpp"
#include "boss_danger.hpp"
#include "hall_sync_runtime.hpp"

static void sfFameDrawStars(SDL_Renderer *renderer,int right,int centerY,int danger,int scale)
{
    if (danger<1 || danger>9) return;
    const int count=danger;
    scale=std::max(2,scale);
    const int radius=3*scale,step=9*scale;
    const int first=right-(count-1)*step;
    SDL_SetRenderDrawColor(renderer,255,220,105,255);
    for (int i=0;i<count;++i) {
        const int cx=first+i*step;
        for (int thickness=0;thickness<2;++thickness) {
            const int o=thickness;
            SDL_RenderDrawLine(renderer,cx-radius,centerY+o,cx+radius,centerY+o);
            SDL_RenderDrawLine(renderer,cx+o,centerY-radius,cx+o,centerY+radius);
            SDL_RenderDrawLine(renderer,cx-radius+1,centerY-radius+1+o,cx+radius-1,centerY+radius-1+o);
            SDL_RenderDrawLine(renderer,cx-radius+1,centerY+radius-1+o,cx+radius-1,centerY-radius+1+o);
        }
    }
}

static std::string sfHallDisplayTitle(const SfHallDisplayEntry &entry,int fallbackRank)
{
    if(entry.serverRank>0) return "#"+std::to_string(entry.serverRank)+" "+entry.playerName;
    if(entry.local && entry.pending) return "LOCAL / EN ATTENTE  "+entry.playerName;
    if(entry.local) return "LOCAL  "+entry.playerName;
    return "#"+std::to_string(fallbackRank)+" "+entry.playerName;
}

// Visible Hall of Fame replacement. It renders a read-only snapshot made from
// the durable global cache plus the phone's local scores. Network orchestration
// is owned by hall_sync_ui_bridge.hpp; drawing never starts HTTP/JNI work.
static void sfCampaignDrawHall(SDL_Renderer *renderer)
{
    sfLoadCampaign();int width,height;SDL_GetRendererOutputSize(renderer,&width,&height);
    if (width<=0 || height<=0) return;
    sfDrawCampaignSpace(renderer,199,SDL_GetTicks64()*.001f,width,height);
    sfCoopCentered(renderer,width,int(height*.035f),"HALL OF FAME",std::max(3,width/130),{255,220,125,255});
    sfCoopCentered(renderer,width,int(height*.105f),"GLOBAL + CE TELEPHONE",std::max(2,width/250));

    const auto entries=sfHallCurrentDisplaySnapshot(sfCampaignSave);
    const auto status=sfHallReadStatusSnapshot();
    const int perPage=height>=width ? 5 : 2,pages=std::max(1,int(entries.size()+perPage-1)/perPage);
    sfFamePage=std::clamp(sfFamePage,0,pages-1);

    if (entries.empty()) {
        sfCoopCentered(renderer,width,int(height*.38f),"VOTRE LEGENDE COMMENCE ICI",std::max(2,width/230));
        sfCoopCentered(renderer,width,int(height*.46f),"BATTEZ UN BOSS EN COOP",std::max(2,width/260));
    }

    int globalCount=0,localOnlyCount=0;
    for(const auto &entry:entries) {
        if(entry.local) ++localOnlyCount; else ++globalCount;
    }

    for (int row=0;row<perPage;++row) {
        const int index=sfFamePage*perPage+row;if (index>=int(entries.size())) break;
        const auto &entry=entries[index];const int y=int(height*(.19f+row*(.57f/perPage)));
        SDL_Rect panel{int(width*.07f),y,int(width*.86f),int(height*.52f/perPage)};
        sfUiPanel(renderer,panel,7,16,34,90,125,150);
        const int left=panel.x+12,w=panel.w-24,scale=std::max(2,std::min(width/240,panel.h/30));
        const int danger=entry.danger;
        const std::string dangerName=(danger>=1 && danger<=9) ? SF_BOSS_DANGER_NAMES[danger-1] : "DANGER INCONNU";

        sfCoopText(renderer,left,y+8,sfHallDisplayTitle(entry,index+1),w,scale+1,{255,215,125,255});
        sfCoopText(renderer,left,y+10+9*(scale+1),entry.pilots[0]+" + "+entry.pilots[1],int(w*.55f),scale);
        sfFameDrawStars(renderer,panel.x+panel.w-18,y+13+12*(scale+1),danger,scale);
        sfCoopText(renderer,left,y+14+18*(scale+1),
            "BOSS "+std::to_string(entry.boss)+"  "+dangerName+"  "+sfFameTime(entry.seconds)+"  "+std::to_string(entry.points)+" PTS",
            w,scale,{120,220,220,255});
    }

    const std::string syncLine=std::string(sfHallStatusLabel(status))+"  GLOBAL "+std::to_string(globalCount)+" + LOCAL "+std::to_string(localOnlyCount);
    if (!sfCampaignStorageError.empty()) sfCoopCentered(renderer,width,int(height*.79f),sfCampaignStorageError,2,{255,130,110,255});
    else if(!status.storageError.empty()) sfCoopCentered(renderer,width,int(height*.79f),status.storageError,2,{255,130,110,255});
    else sfCoopCentered(renderer,width,int(height*.79f),syncLine,2,
        status.error==SfHallSyncError::Auth || status.error==SfHallSyncError::Protocol ? SDL_Color{255,150,120,255} : SDL_Color{165,220,235,255});

    sfCoopButton(renderer,{int(width*.07f),int(height*.84f),int(width*.22f),int(height*.055f)},"PRECEDENT");
    sfCoopCentered(renderer,width,int(height*.855f),std::to_string(sfFamePage+1)+" / "+std::to_string(pages),2);
    sfCoopButton(renderer,{int(width*.71f),int(height*.84f),int(width*.22f),int(height*.055f)},"SUIVANT");
    sfCoopButton(renderer,{int(width*.25f),int(height*.925f),int(width*.5f),int(height*.06f)},"ACCUEIL");
}
