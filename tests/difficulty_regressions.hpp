#pragma once
#include <filesystem>

static void testDifficultyGameplay()
{
    for(int encounter=0;encounter<200;++encounter) {
        setupCampaign(encounter);sfCoop.attack=0;
        const auto start=sfCoop.position;
        for(int frame=0;frame<120;++frame) sfCoopTick(1.0f/60);
        assert(sfCoop.volley>0 && sfCoop.phase==SfCoopPhase::Combat);
        assert(std::hypot(start.x-sfCoop.position.x,start.y-sfCoop.position.y)>1);
        assert(sfCoop.boss==encounter%50 && sfCoopProfile().difficulty==encounter/50);
        assert(!sa1.empty());
        for(const auto &shot:sfCoop.shots) assert(shot.owner<0 && std::isfinite(shot.position.x));
        if(encounter>=50) {
            const auto &before=sfEncounterProfile(encounter-50),&after=sfCoopProfile();
            assert(after.health>before.health && after.interval<before.interval && after.shotSpeed>before.shotSpeed);
        }
    }
    std::puts("PASS: 200 actual combat openings with moving bosses, hostile attacks, live field and no human autofire");
}

static void testDifficultyRendering(const char *previewDirectory)
{
    const int counts[]={0,4,8,20};
    auto *surface=SDL_CreateRGBSurfaceWithFormat(0,400,400,32,SDL_PIXELFORMAT_RGBA32);
    auto *renderer=SDL_CreateSoftwareRenderer(surface);assert(renderer);
    auto *atlas=sfCoopTextures(renderer).bosses;assert(atlas);
    auto pixels=[&](int encounter,bool original) {
        SDL_SetRenderDrawColor(renderer,2,5,16,255);SDL_RenderClear(renderer);
        if(original) sfDrawBoss(renderer,atlas,sfBossIndex(encounter),tupl(200,200),95,1.25f);
        else sfDrawEncounterBoss(renderer,atlas,encounter,tupl(200,200),95,1.25f);
        std::vector<Uint32> data(400*400);
        assert(SDL_RenderReadPixels(renderer,nullptr,SDL_PIXELFORMAT_RGBA32,data.data(),400*4)==0);
        return data;
    };
    for(int encounter=0;encounter<200;++encounter) {
        const int boss=sfBossIndex(encounter),difficulty=sfDifficultyIndex(encounter);
        assert(sfBossAppearance(encounter).tentacles==counts[difficulty]);
        const auto rect=sfBossAtlasRect(atlas,encounter),originalRect=sfBossAtlasRect(atlas,boss);
        assert(rect.x==originalRect.x && rect.y==originalRect.y);
        const auto original=pixels(encounter,true),decorated=pixels(encounter,false);
        if(difficulty==0) assert(original==decorated); // No aura or reflection at level 1.
        else {
            size_t outside=0,unchanged=0,body=0;
            for(int y=0;y<400;++y) for(int x=0;x<400;++x) {
                const size_t p=y*400+x;
                if(std::hypot(x-200,y-200)>110 && original[p]!=decorated[p]) ++outside;
                if(std::hypot(x-200,y-200)<60) {++body;if(original[p]==decorated[p]) ++unchanged;}
            }
            assert(outside>100 && unchanged>body/3); // Visible signature, original central colors retained.
        }
        for(int i=0;i<counts[difficulty];++i) {
            const auto a=sfTentaclePath(boss,i,counts[difficulty],1.25f);
            const auto b=sfTentaclePath(boss,i,counts[difficulty],1.26f);
            const auto c=sfTentaclePath(boss,i,counts[difficulty],2.25f);
            assert(std::hypot(a.back().x-c.back().x,a.back().y-c.back().y)>.002f);
            for(size_t j=0;j<a.size();++j) {
                assert(std::isfinite(a[j].x) && std::hypot(a[j].x,a[j].y)<1.9f);
                assert(std::hypot(a[j].x-b[j].x,a[j].y-b[j].y)<.01f);
            }
        }
    }
    SDL_DestroyRenderer(renderer);SDL_FreeSurface(surface);
    if(previewDirectory) {
        surface=SDL_CreateRGBSurfaceWithFormat(0,1200,1000,32,SDL_PIXELFORMAT_RGBA32);
        renderer=SDL_CreateSoftwareRenderer(surface);assert(renderer);
        atlas=sfCoopTextures(renderer).bosses;
        for(int frame=0;frame<2;++frame) {
            SDL_SetRenderDrawColor(renderer,2,5,16,255);SDL_RenderClear(renderer);
            for(int d=0;d<4;++d) for(int row=0;row<3;++row) {
                const int boss=row==0 ? 0 : row==1 ? 17 : 39;
                sfDrawEncounterBoss(renderer,atlas,d*50+boss,tupl(150+d*300,170+row*320),84,1.25f+frame*.65f);
                sfCoopText(renderer,20+d*300,20+row*320,"N"+std::to_string(d+1)+" "+sfDifficultyNames[d],260,2);
                sfCoopText(renderer,20+d*300,300+row*320,"BOSS "+std::to_string(boss+1)+" - "+std::to_string(counts[d])+" TENTACULES",270,2);
            }
            SDL_RenderFlush(renderer);
            assert(IMG_SavePNG(surface,(std::string(previewDirectory)+"/difficulty-"+std::to_string(frame)+".png").c_str())==0);
        }
        SDL_DestroyRenderer(renderer);SDL_FreeSurface(surface);
        for(auto size:{std::pair<int,int>{709,1536},{1536,709}}) {
            surface=SDL_CreateRGBSurfaceWithFormat(0,size.first,size.second,32,SDL_PIXELFORMAT_RGBA32);
            renderer=SDL_CreateSoftwareRenderer(surface);assert(renderer);
            setupCampaign(199);sfArenaW=size.first;sfArenaH=size.second;sfCampaignStart();
            sfCoop.phase=SfCoopPhase::Combat;
            const std::string orientation=size.first<size.second ? "portrait" : "landscape";
            sfCampaignDrawSelect(renderer);SDL_RenderFlush(renderer);
            assert(IMG_SavePNG(surface,(std::string(previewDirectory)+"/ultimate-selector-"+orientation+".png").c_str())==0);
            for(int i=0;i<120;++i) sfCoopTick(1.0f/60);
            sfCoopDrawArena(renderer,size.first,size.second);SDL_RenderFlush(renderer);
            assert(IMG_SavePNG(surface,(std::string(previewDirectory)+"/ultimate-combat-"+orientation+".png").c_str())==0);
            SDL_DestroyRenderer(renderer);SDL_FreeSurface(surface);
        }
    }
    std::puts("PASS: 200 renders, original level-1 pixels, 0/4/8/20 smooth bounded tentacles, visible aura and localized reflections");
}

static void testDifficultySelection()
{
    for(auto size:{std::pair<int,int>{360,780},{780,360}}) {
        setupCampaign(199);sfArenaW=size.first;sfArenaH=size.second;
        sfCampaignSave.cleared=200;sfCampaignPage=4;
        sfFixRequestedScreen.store(SF_UI_CAMPAIGN);sfFixLaunchPending.store(false);sfFixConsumedFingers.clear();
        auto choose=sfCoopFinger(SDL_FINGERDOWN,1701,.855f,.15f);sfFixHandleEvent(&choose);
        assert(sfCampaignPage==19 && !sfFixLaunchPending.load());
        const auto card=sfCampaignCard(9,size.first,size.second);
        choose=sfCoopFinger(SDL_FINGERDOWN,1702,(card.x+card.w*.5f)/size.first,(card.y+card.h*.5f)/size.second);
        sfFixHandleEvent(&choose);assert(sfCampaignSave.selected==199 && sfFixLaunchPending.load());
        sfFixApplyUiRequests();assert(sfCoop.encounter==199 && sfCoop.boss==49 && sfCoopProfile().difficulty==3);
    }
    std::puts("PASS: portrait/landscape difficulty tabs and last card launch encounter 200 with boss image 50");
}

static void testDurableV1Migration(const std::string &directory)
{
    const auto oldPath=sfCampaignStoragePath;
    const std::string fixture="SPACEFORTRESS_CAMPAIGN 1\n50 49 1 1\n\"Fab\"\n\"Yann\"\n\"Éclipse\"\n42 50 9876 120 2 100 \"Fab\" \"Yann\" \"Éclipse\"\n41 49 1234 110 2 99 \"Fab\" \"Yann\" \"Éclipse\"\n";
    sfCampaignStoragePath=directory+"/migration.dat";
    {std::ofstream out(sfCampaignStoragePath);out<<fixture;}
    sfCampaignLoaded=false;sfLoadCampaign();sfCampaignStart();
    assert(sfCoop.phase==SfCoopPhase::Name && sfCoop.encounter==49 && sfCampaignSave.victory.id==42);
    assert(sfSaveCampaign(sfCampaignSave));
    bool preserved=false;
    for(const auto &entry:std::filesystem::directory_iterator(directory)) {
        if(entry.path().filename().string().find("migration.dat.v1-original.preserved-")!=0) continue;
        std::ifstream in(entry.path());std::string bytes((std::istreambuf_iterator<char>(in)),{});
        if(bytes==fixture) preserved=true;
    }
    assert(preserved);
    sfCampaignSave={};sfCampaignLoaded=false;sfLoadCampaign();sfCampaignStart();
    assert(sfCoop.phase==SfCoopPhase::Name && sfCampaignSave.cleared==50);
    assert(sfCampaignSave.victory.score==9876 && sfCampaignSave.names[2]=="Éclipse");
    sfCoop.names=sfCampaignSave.names;assert(sfCoopSaveNames());
    assert(sfCampaignSave.fame.size()==2 && sfCampaignSave.fame.back().id==42);
    assert(sfCampaignSave.fame.front().score==1234 && !sfCampaignSave.pending);
    sfCampaignStoragePath=oldPath;sfCampaignLoaded=false;sfLoadCampaign();
    std::puts("PASS: durable v1 archive, migration and restart preserve names/scores/progression and award pending victory exactly once");
}
