#pragma once

#include "hall_of_fame.hpp"

static void sfFameDrawStars(SDL_Renderer *renderer,int right,int centerY,int danger,int scale)
{
    const int count=std::clamp(danger,1,4);
    scale=std::max(1,scale);
    const int radius=3*scale,step=9*scale;
    const int first=right-(count-1)*step;
    SDL_SetRenderDrawColor(renderer,255,220,105,255);
    for (int i=0;i<count;++i) {
        const int cx=first+i*step;
        for (int thickness=0;thickness<scale;++thickness) {
            const int o=thickness-scale/2;
            SDL_RenderDrawLine(renderer,cx-radius,centerY+o,cx+radius,centerY+o);
            SDL_RenderDrawLine(renderer,cx+o,centerY-radius,cx+o,centerY+radius);
            SDL_RenderDrawLine(renderer,cx-radius+1,centerY-radius+1+o,cx+radius-1,centerY+radius-1+o);
            SDL_RenderDrawLine(renderer,cx-radius+1,centerY+radius-1+o,cx+radius-1,centerY-radius+1+o);
        }
    }
}

// Visible Hall of Fame replacement. The historical renderer remains compiled as
// sfCampaignDrawHallLegacy so this change is isolated from campaign gameplay.
static void sfCampaignDrawHall(SDL_Renderer *renderer)
{
    sfLoadCampaign();int width,height;SDL_GetRendererOutputSize(renderer,&width,&height);
    if (width<=0 || height<=0) return;
    sfDrawCampaignSpace(renderer,199,SDL_GetTicks64()*.001f,width,height);
    sfCoopCentered(renderer,width,int(height*.035f),"HALL OF FAME",std::max(3,width/130),{255,220,125,255});
    sfCoopCentered(renderer,width,int(height*.105f),"CLASSEMENT PAR POINTS",std::max(2,width/250));

    std::vector<SfFameEntry> entries=sfCampaignSave.fame;
    std::stable_sort(entries.begin(),entries.end(),[](const auto &a,const auto &b){
        return sfFameRanksBefore(a.boss,a.seconds,b.boss,b.seconds);
    });

    const int perPage=height>=width ? 5 : 2,pages=std::max(1,int(entries.size()+perPage-1)/perPage);
    sfFamePage=std::clamp(sfFamePage,0,pages-1);
    if (entries.empty()) {
        sfCoopCentered(renderer,width,int(height*.38f),"VOTRE LEGENDE COMMENCE ICI",std::max(2,width/230));
        sfCoopCentered(renderer,width,int(height*.46f),"BATTEZ UN BOSS EN COOP",std::max(2,width/260));
    }

    for (int row=0;row<perPage;++row) {
        const int index=sfFamePage*perPage+row;if (index>=int(entries.size())) break;
        const auto &entry=entries[index];const int y=int(height*(.19f+row*(.57f/perPage)));
        SDL_Rect panel{int(width*.07f),y,int(width*.86f),int(height*.52f/perPage)};
        sfUiPanel(renderer,panel,7,16,34,90,125,150);
        const int left=panel.x+12,w=panel.w-24,scale=std::max(2,std::min(width/240,panel.h/30));
        const int danger=sfFameDanger(entry.boss),points=sfFamePoints(entry.boss,entry.seconds);
        const char *difficulty=sfDifficultyNames[std::clamp(danger-1,0,3)];

        sfCoopText(renderer,left,y+8,std::to_string(index+1)+". "+entry.names[2],w,scale+1,{255,215,125,255});
        sfCoopText(renderer,left,y+10+9*(scale+1),entry.names[0]+" + "+entry.names[1],w-int(45*scale),scale);
        sfFameDrawStars(renderer,panel.x+panel.w-16-int(3*scale),y+13+12*(scale+1),danger,std::max(1,scale/2));
        sfCoopText(renderer,left,y+14+18*(scale+1),
            "BOSS "+std::to_string(entry.boss)+"  "+difficulty+"  "+sfFameTime(entry.seconds)+"  "+std::to_string(points)+" PTS",
            w,scale,{120,220,220,255});
    }

    if (!sfCampaignStorageError.empty()) sfCoopCentered(renderer,width,int(height*.79f),sfCampaignStorageError,2,{255,130,110,255});
    else sfCoopCentered(renderer,width,int(height*.79f),"MEMOIRE DE CE TELEPHONE - "+std::to_string(sfCampaignSave.fame.size())+" VICTOIRES",2);
    sfCoopButton(renderer,{int(width*.07f),int(height*.84f),int(width*.22f),int(height*.055f)},"PRECEDENT");
    sfCoopCentered(renderer,width,int(height*.855f),std::to_string(sfFamePage+1)+" / "+std::to_string(pages),2);
    sfCoopButton(renderer,{int(width*.71f),int(height*.84f),int(width*.22f),int(height*.055f)},"SUIVANT");
    sfCoopButton(renderer,{int(width*.25f),int(height*.925f),int(width*.5f),int(height*.06f)},"ACCUEIL");
}
