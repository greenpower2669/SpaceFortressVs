#pragma once
static SDL_Rect sfCampaignCard(int index,int width,int height)
{
    const int columns=height>=width ? 5 : 10,rows=50/columns;
    const int cw=int(width*.9f)/columns,ch=int(height*.66f)/rows;
    return {int(width*.05f)+(index%columns)*cw,int(height*.16f)+(index/columns)*ch,cw-4,ch-4};
}
static void sfCampaignDrawSelect(SDL_Renderer *r)
{
    sfLoadCampaign();int w,h;SDL_GetRendererOutputSize(r,&w,&h);if (w<=0 || h<=0) return;
    sfDrawCampaignSpace(r,sfCampaignSave.selected,SDL_GetTicks64()*.001f,w,h);
    sfCoopCentered(r,w,int(h*.035f),"CAMPAGNE COOPERATIVE",std::max(2,w/150));
    sfCoopCentered(r,w,int(h*.095f),std::to_string(sfCampaignSave.cleared)+" VICTOIRES SUR 50",std::max(2,w/240));
    auto &t=sfCoopTextures(r);
    for (int i=0;i<50;++i) {
        const auto rect=sfCampaignCard(i,w,h);const bool unlocked=i<=sfCampaignSave.cleared;
        sfUiPanel(r,rect,5,14,28,unlocked ? 80 : 35,unlocked ? 175 : 55,unlocked ? 185 : 70);
        if (t.bosses) {
            const auto source=sfBossAtlasRect(t.bosses,i);const int size=std::max(1,std::min(rect.w-6,rect.h-18));
            SDL_Rect dest{rect.x+(rect.w-size)/2,rect.y+3,size,size};
            SDL_SetTextureColorMod(t.bosses,unlocked ? 255 : 45,unlocked ? 255 : 45,unlocked ? 255 : 60);
            SDL_RenderCopy(r,t.bosses,&source,&dest);
        }
        sfCoopText(r,rect.x+5,rect.y+rect.h-15,std::to_string(i+1)+(i<sfCampaignSave.cleared ? " OK" : unlocked ? " GO" : " -"),rect.w-8,2);
    }
    if (t.bosses) SDL_SetTextureColorMod(t.bosses,255,255,255);
    sfCoopButton(r,{int(w*.08f),int(h*.86f),int(w*.84f),int(h*.065f)},sfCampaignSave.pending ? "INSCRIRE LA VICTOIRE EN ATTENTE" : "REPRENDRE LA CAMPAGNE");
    sfCoopButton(r,{int(w*.3f),int(h*.94f),int(w*.4f),int(h*.045f)},"ACCUEIL");
}
static void sfCampaignDrawHall(SDL_Renderer *r)
{
    sfLoadCampaign();int w,h;SDL_GetRendererOutputSize(r,&w,&h);if (w<=0 || h<=0) return;
    sfDrawCampaignSpace(r,49,SDL_GetTicks64()*.001f,w,h);
    sfCoopCentered(r,w,int(h*.035f),"HALL OF FAME",std::max(3,w/130),{255,220,125,255});
    sfCoopCentered(r,w,int(h*.105f),"LES EQUIPES IMMORTALISEES",std::max(2,w/250));
    auto entries=sfCampaignSave.fame;
    std::stable_sort(entries.begin(),entries.end(),[](const auto &a,const auto &b) {
        return a.boss!=b.boss ? a.boss>b.boss : a.score!=b.score ? a.score>b.score : a.seconds<b.seconds;
    });
    const int perPage=h>=w ? 5 : 2,pages=std::max(1,int(entries.size()+perPage-1)/perPage);
    sfFamePage=std::clamp(sfFamePage,0,pages-1);
    if (entries.empty()) {
        sfCoopCentered(r,w,int(h*.38f),"VOTRE LEGENDE COMMENCE ICI",std::max(2,w/230));
        sfCoopCentered(r,w,int(h*.46f),"BATTEZ UN BOSS EN COOP",std::max(2,w/260));
    }
    for (int row=0;row<perPage;++row) {
        const int index=sfFamePage*perPage+row;if (index>=int(entries.size())) break;
        const auto &e=entries[index];const int y=int(h*(.19f+row*(.57f/perPage)));
        SDL_Rect panel{int(w*.07f),y,int(w*.86f),int(h*.52f/perPage)};
        sfUiPanel(r,panel,7,16,34,90,125,150);
        const int left=panel.x+12,maxw=panel.w-24,scale=std::max(2,std::min(w/240,panel.h/30));
        sfCoopText(r,left,y+8,std::to_string(index+1)+". "+e.names[2],maxw,scale+1,{255,215,125,255});
        sfCoopText(r,left,y+10+9*(scale+1),e.names[0]+" + "+e.names[1],maxw,scale);
        sfCoopText(r,left,y+14+18*(scale+1),"BOSS "+std::to_string(e.boss)+" / 50  SCORE "+std::to_string(e.score)+"  "+std::to_string(e.seconds)+" S",maxw,scale,{120,220,220,255});
    }
    if (!sfCampaignStorageError.empty()) sfCoopCentered(r,w,int(h*.79f),sfCampaignStorageError,2,{255,130,110,255});
    else sfCoopCentered(r,w,int(h*.79f),"MEMOIRE DE CE TELEPHONE - "+std::to_string(entries.size())+" VICTOIRES",2);
    sfCoopButton(r,{int(w*.07f),int(h*.84f),int(w*.22f),int(h*.055f)},"PRECEDENT");
    sfCoopCentered(r,w,int(h*.855f),std::to_string(sfFamePage+1)+" / "+std::to_string(pages),2);
    sfCoopButton(r,{int(w*.71f),int(h*.84f),int(w*.22f),int(h*.055f)},"SUIVANT");
    sfCoopButton(r,{int(w*.25f),int(h*.925f),int(w*.5f),int(h*.06f)},"ACCUEIL");
}
static void sfCoopDrawOverlay(SDL_Renderer *r,int w,int h)
{
    if (sfCoop.phase==SfCoopPhase::Combat || sfCoop.phase==SfCoopPhase::Dying) return;
    SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);SDL_SetRenderDrawColor(r,2,6,18,225);
    SDL_Rect dim{0,0,w,h};SDL_RenderFillRect(r,&dim);
    if (sfCoop.phase==SfCoopPhase::Name) {
        sfCoopCentered(r,w,int(h*.04f),sfCoop.boss==49 ? "LES 50 BOSS SONT VAINCUS" : "VICTOIRE COOPERATIVE",std::max(3,w/190),{255,215,125,255});
        sfCoopCentered(r,w,int(h*.09f),"GRAVEZ VOTRE EQUIPE DANS L HISTOIRE",std::max(2,w/300));
        const char *labels[]={"PILOTE ORANGE","PILOTE BLEU","NOM DE L EQUIPE"};
        for (int i=0;i<3;++i) {
            const int y=int(h*(.15f+i*.115f));
            sfCoopText(r,int(w*.09f),y,labels[i],int(w*.8f),std::max(2,w/300));
            SDL_Rect field{int(w*.08f),y+int(h*.027f),int(w*.84f),int(h*.064f)};
            sfUiPanel(r,field,8,25,45,i==sfCoop.nameField ? 100 : 45,i==sfCoop.nameField ? 240 : 100,180);
            auto value=sfCoop.names[i];
            if (i==sfCoop.nameField && !sfCoop.composition.empty()) value+=" "+sfCoop.composition;
            sfCoopText(r,field.x+10,field.y+(field.h-21)/2,value.empty() ? "TOUCHER POUR SAISIR" : value,field.w-20,std::max(2,w/230));
        }
        sfCoopCentered(r,w,int(h*.505f),sfCoop.error.empty() ? "24 CARACTERES PAR NOM" : sfCoop.error,2,sfCoop.error.empty() ? SDL_Color{228,241,255,255} : SDL_Color{255,140,120,255});
        sfCoopButton(r,{int(w*.08f),int(h*.54f),int(w*.84f),int(h*.068f)},"IMMORTALISER LA VICTOIRE");
    } else if (sfCoop.phase==SfCoopPhase::Intro) {
        sfCoopCentered(r,w,int(h*.32f),"BOSS "+std::to_string(sfCoop.boss+1)+" / 50",std::max(3,w/150),{255,210,125,255});
        sfCoopCentered(r,w,int(h*.40f),sfCoopProfile().name,std::max(3,w/190));
        sfCoopCentered(r,w,int(h*.51f),sfBossHints[sfCoopProfile().family],std::max(2,w/320));
        sfCoopCentered(r,w,int(h*.60f),"GLISSEZ POUR PILOTER - TIR AUTOMATIQUE",std::max(2,w/340));
        sfCoopCentered(r,w,int(h*.67f),"MINERAIS = ENERGIE - PROXIMITE = SECOURS",std::max(2,w/350));
    } else {
        const bool saved=sfCoop.phase==SfCoopPhase::Saved,paused=sfCoop.phase==SfCoopPhase::Paused;
        sfCoopCentered(r,w,int(h*.31f),saved ? "VICTOIRE IMMORTALISEE" : paused ? "PAUSE COOPERATIVE" : "EQUIPE HORS COMBAT",std::max(3,w/190),{255,220,130,255});
        if (saved) sfCoopCentered(r,w,int(h*.42f),sfCampaignSave.names[2],std::max(3,w/200));
        sfCoopButton(r,{int(w*.13f),int(h*.53f),int(w*.74f),int(h*.09f)},saved ? (sfCoop.boss==49 ? "VOIR LE HALL OF FAME" : "BOSS SUIVANT") : paused ? "REPRENDRE" : "REESSAYER CE BOSS");
        sfCoopButton(r,{int(w*.23f),int(h*.68f),int(w*.54f),int(h*.08f)},"CHOISIR UNE MISSION");
        sfCoopButton(r,{int(w*.3f),int(h*.81f),int(w*.4f),int(h*.07f)},"ACCUEIL");
    }
}
static bool sfCampaignFrame(SDL_Renderer *r)
{
    if (!sfIsCoop() || sfUiScreen!=SF_UI_GAME) return false;
    const float dt=sfFrameDt;
    if (sfCoop.phase==SfCoopPhase::Combat) {
        const int steps=std::max(1,int(std::ceil(dt*90)));
        for (int i=0;i<steps;++i) sfCoopTick(dt/steps);
    } else if (sfCoop.phase==SfCoopPhase::Intro || sfCoop.phase==SfCoopPhase::Dying) {
        sfCoop.phaseTime+=dt;
        if (sfCoop.phase==SfCoopPhase::Intro && sfCoop.phaseTime>=3) {sfCoop.phase=SfCoopPhase::Combat;sfCoop.phaseTime=0;}
        else if (sfCoop.phase==SfCoopPhase::Dying && sfCoop.phaseTime>=1.6f) {sfCoop.phase=SfCoopPhase::Name;sfCoop.phaseTime=0;}
    }
    int w,h;SDL_GetRendererOutputSize(r,&w,&h);
    if (w>0 && h>0) {sfCoopDrawArena(r,w,h);sfCoopDrawOverlay(r,w,h);}
    return true;
}
static void sfCoopFocusName(int field)
{
    sfCoop.nameField=std::clamp(field,0,2);sfCoop.keyboard=true;sfCoop.composition.clear();
    SDL_Rect area{int(sfArenaW*.08f),int(sfArenaH*(.177f+field*.115f)),int(sfArenaW*.84f),int(sfArenaH*.064f)};
    SDL_SetTextInputRect(&area);SDL_StartTextInput();
}
static bool sfCoopSaveNames()
{
    for (int i=0;i<3;++i) if (sfCleanName(sfCoop.names[i]).empty()) {
        sfCoop.error="RENSEIGNEZ LES DEUX NOMS ET L EQUIPE";sfCoopFocusName(i);return false;
    }
    if (!sfRecordCampaignVictory(sfCoop.names)) {
        sfCoop.error=sfCampaignStorageError.empty() ? "ECHEC SAUVEGARDE - REESSAYER" : sfCampaignStorageError;return false;
    }
    SDL_StopTextInput();sfCoop.keyboard=false;sfCoop.phase=SfCoopPhase::Saved;sfCoop.error.clear();return true;
}
static void sfCoopRequestHome(int screen=SF_UI_HOME)
{
    sfCampaignSuspend();sfFixLaunchPending.store(false);sfFixRequestedScreen.store(screen);
}
static bool sfCampaignHandleEvent(SDL_Event *event)
{
    const int screen=sfFixRequestedScreen.load();const bool game=sfIsCoop() && screen==SF_UI_GAME;
    if (event->type==SDL_KEYDOWN && (event->key.keysym.sym==SDLK_ESCAPE || event->key.keysym.sym==SDLK_AC_BACK)) {
        if (game) {
            if (sfCoop.phase==SfCoopPhase::Name && sfCoop.keyboard) {SDL_StopTextInput();sfCoop.keyboard=false;}
            else if (sfCoop.phase==SfCoopPhase::Combat) sfCampaignSuspend();else sfCoopRequestHome();
            event->type=SDL_USEREVENT;return true;
        }
        if (screen==SF_UI_HALL || screen==SF_UI_CAMPAIGN) {sfCoopRequestHome();event->type=SDL_USEREVENT;return true;}
    }
    if (game && sfCoop.phase==SfCoopPhase::Name) {
        if (event->type==SDL_TEXTINPUT) {
            auto &name=sfCoop.names[sfCoop.nameField];const std::string combined=name+event->text.text;
            int characters=0;size_t end=0;
            for (;end<combined.size();++end) if ((static_cast<unsigned char>(combined[end])&0xc0)!=0x80 && ++characters>24) break;
            name=combined.substr(0,std::min(end,size_t(96)));sfCoop.composition.clear();sfCoop.error.clear();
        } else if (event->type==SDL_TEXTEDITING) sfCoop.composition=event->edit.text;
        else if (event->type==SDL_KEYDOWN) {
            auto &name=sfCoop.names[sfCoop.nameField];
            if (event->key.keysym.sym==SDLK_BACKSPACE && !name.empty()) {
                size_t i=name.size()-1;while (i>0 && (static_cast<unsigned char>(name[i])&0xc0)==0x80) --i;name.erase(i);
            } else if (event->key.keysym.sym==SDLK_TAB) sfCoopFocusName((sfCoop.nameField+1)%3);
            else if (event->key.keysym.sym==SDLK_RETURN) {if (sfCoop.nameField<2) sfCoopFocusName(sfCoop.nameField+1);else sfCoopSaveNames();}
        }
        if (event->type==SDL_TEXTINPUT || event->type==SDL_TEXTEDITING || event->type==SDL_KEYDOWN) {event->type=SDL_USEREVENT;return true;}
    }
    const bool touch=event->type==SDL_FINGERDOWN || event->type==SDL_FINGERMOTION || event->type==SDL_FINGERUP;
    if (!touch) return false;
    const float x=event->tfinger.x,y=event->tfinger.y;const auto finger=event->tfinger.fingerId;
    if (screen==SF_UI_HOME && event->type==SDL_FINGERDOWN) {
        if (y>=.71f && y<=.85f && x>=.5f) {sfLoadCampaign();sfFixRequestedScreen.store(SF_UI_HALL);}
        else if (y>=.545f && y<=.69f && sfSelectedMode>=SF_COOP_LOCAL) {sfLoadCampaign();sfFixRequestedScreen.store(SF_UI_CAMPAIGN);}
        else return false;
        sfFixConsumedFingers.insert(finger);event->type=SDL_USEREVENT;return true;
    }
    if (screen==SF_UI_HALL || screen==SF_UI_CAMPAIGN) {
        if (event->type==SDL_FINGERDOWN) {
            sfFixConsumedFingers.insert(finger);
            if (screen==SF_UI_HALL) {
                if (y>.915f) sfCoopRequestHome();
                else if (y>=.825f && y<=.91f) {if (x<.35f) sfFamePage=std::max(0,sfFamePage-1);else if (x>.65f) ++sfFamePage;}
            } else if (y>.935f) sfCoopRequestHome();
            else {
                int selected=-1;
                if (y>=.85f && y<=.93f) selected=sfCampaignSave.selected;
                else for (int i=0;i<50;++i) {
                    const auto rect=sfCampaignCard(i,int(sfArenaW),int(sfArenaH));
                    if (x*sfArenaW>=rect.x && x*sfArenaW<rect.x+rect.w && y*sfArenaH>=rect.y && y*sfArenaH<rect.y+rect.h) {selected=i;break;}
                }
                if (selected>=0 && (selected<=sfCampaignSave.cleared || sfCampaignSave.pending)) {
                    sfCampaignSave.selected=std::clamp(selected,0,49);sfFixLaunchPending.store(true);
                }
            }
        }
        event->type=SDL_USEREVENT;return true;
    }
    if (!game) return false;
    if (sfCoop.phase!=SfCoopPhase::Combat) {
        if (event->type==SDL_FINGERDOWN) {
            sfFixConsumedFingers.insert(finger);
            if (sfCoop.phase==SfCoopPhase::Name) {
                if (y>=.54f && y<=.63f && x>.07f && x<.93f) sfCoopSaveNames();
                else for (int i=0;i<3;++i) if (y>=.15f+i*.115f && y<.26f+i*.115f) sfCoopFocusName(i);
            } else if (sfCoop.phase==SfCoopPhase::Saved || sfCoop.phase==SfCoopPhase::Defeat || sfCoop.phase==SfCoopPhase::Paused) {
                if (y>=.53f && y<=.64f) {
                    if (sfCoop.phase==SfCoopPhase::Paused) sfCoop.phase=SfCoopPhase::Combat;
                    else if (sfCoop.phase==SfCoopPhase::Saved && sfCoop.boss==49) sfCoopRequestHome(SF_UI_HALL);
                    else {if (sfCoop.phase==SfCoopPhase::Defeat) sfCampaignSave.selected=sfCoop.boss;sfFixLaunchPending.store(true);}
                } else if (y>=.68f && y<=.78f) sfCoopRequestHome(SF_UI_CAMPAIGN);
                else if (y>=.80f) sfCoopRequestHome();
            }
        }
        event->type=SDL_USEREVENT;return true;
    }
    if (event->type==SDL_FINGERDOWN && x>.79f && y<.10f) {
        sfCampaignSuspend();sfFixConsumedFingers.insert(finger);event->type=SDL_USEREVENT;return true;
    }
    if (event->type==SDL_FINGERDOWN) {
        const int owner=y<.5f ? 0 : 1;
        if (!(owner==0 && sfActiveMode==SF_COOP_AI) && !sfCoop.controls[owner].down && sfCoopShip(owner)->pv>0) {
            sfCoop.controls[owner].down=true;sfCoop.controls[owner].finger=finger;
        }
    }
    for (auto &c : sfCoop.controls) if (c.finger==finger) {
        c.target=tupl(std::clamp(x,0.0f,1.0f)*sfArenaW,std::clamp(y,0.0f,1.0f)*sfArenaH);
        if (event->type==SDL_FINGERUP) {c.down=false;c.finger=-1;}
    }
    event->type=SDL_USEREVENT;return true;
}
