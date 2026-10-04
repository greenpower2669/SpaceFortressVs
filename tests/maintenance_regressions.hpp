#pragma once
#include <dirent.h>

static void testDuelStyleRoundTrip()
{
    sfCampaignRestoreDuelShips();
    setupTactics();
    for (auto *ship : {Spritej1,Spritej2}) {
        ship->w=ship->sw=200;ship->h=ship->sh=180;
        ship->animated=true;ship->frames=10;ship->frame=3;ship->speed=-1;
    }
    for (int repeat=0;repeat<3;++repeat) {
        sfActiveMode=SF_COOP_LOCAL;sfCampaignSave.pending=false;
        sfCampaignStart();
        sfActiveMode=SF_DUEL_LOCAL;sfCampaignRestoreDuelShips();
        for (auto *ship : {Spritej1,Spritej2}) {
            assert(ship->frames==10 && ship->frame==3 && ship->speed==-1);
            assert(ship->w==200 && ship->h==180 && ship->sw==200 && ship->sh==180);
            assert(ship->animated);
        }
    }
    std::puts("PASS: repeated cooperative round trips restore the complete historical animation state");
}

static void testShipBreathing()
{
    auto *surface=SDL_CreateRGBSurfaceWithFormat(0,780,1680,32,SDL_PIXELFORMAT_RGBA32);assert(surface);
    auto *renderer=SDL_CreateSoftwareRenderer(surface);assert(renderer);
    for (int fps : {30,60,120}) for (float scale : {1.0f,2.0f,4.8f}) {
        sfCampaignRestoreDuelShips();setupTactics();
        sfActiveMode=sfSelectedMode=SF_DUEL_LOCAL;setia=false;k0=scale;
        for (auto *s : {Spritej1,Spritej2}) {
            s->sw=s->w=200;s->sh=s->h=160;s->frames=10;s->frame=0;s->speed=1;s->animated=true;
        }
        sfFixResetMatchState();setia=false;
        const auto x=Spritej1->x,y=Spritej1->y;float low=200,high=200;
        for (int i=0;i<=fps*3;++i) {
            // Exercise the historical auxiliary call as well as the real
            // frame hook: it must not accumulate visual growth between frames.
            Spritej1->update();Spritej2->update();
            assert(Spritej1->w>=197.99f && Spritej1->w<=202.01f);
            sfSceneSeconds=float(i)/fps-.016f;sfTacticsLastTick=0;
            sfTacticsBeginFrame(renderer);
            low=std::min(low,Spritej1->w);high=std::max(high,Spritej1->w);
            assert(Spritej1->x==x && Spritej1->y==y);
            assert(Spritej1->sw==200 && Spritej1->sh==160);
            assert(Spritej1->w>=197.99f && Spritej1->w<=202.01f);
            assert(std::abs(Spritej1->w/200-Spritej1->h/160)<.0001f);
        }
        assert(low<199 && high>201);
        assert(std::abs(Spritej1->w-200)<.0001f && std::abs(Spritej1->h-160)<.0001f);
        sfFixResetMatchState();assert(Spritej1->w==200 && Spritej1->h==160);
    }
    k0=1;SDL_DestroyRenderer(renderer);SDL_FreeSurface(surface);
    std::puts("PASS: subtle centered breathing returns to nominal size across 30/60/120Hz and legacy clock scales");
}

static void testCoopHumanAim()
{
    for (bool ai : {false,true}) for (int fps : {30,60,120}) {
        setupCampaign(0,ai);sfCoop.position=tupl(700,840);
        sfCoop.motion.velocity.set(110,0);sfCoop.shots.clear();
        Spritej1->nrj=Spritej2->nrj=10;
        if (!ai) sfCoopFire(0);sfCoopFire(1);
        sfCoopMovePlayers(1.0f/fps);
        assert(sfCoop.shots.size()==2);
        for (const auto &shot : sfCoop.shots) {
            assert(shot.kind==0);
            if (ai && shot.owner==0) assert(shot.velocity.vx>100);
            else {
                assert(std::abs(shot.velocity.vx)<30);
                assert(shot.owner==0 ? shot.velocity.vy>0 : shot.velocity.vy<0);
            }
        }
        const auto shots=sfCoop.shots;
        sfCoop.position=tupl(40,840);sfCoop.motion.velocity.set(-250,0);
        sfCoopProjectiles(.01f);
        assert(sfCoop.shots.size()==shots.size());
        for (size_t i=0;i<shots.size();++i) {
            assert(sfCoop.shots[i].velocity.vx==shots[i].velocity.vx);
            assert(sfCoop.shots[i].velocity.vy==shots[i].velocity.vy);
        }
    }
    sfActiveMode=sfSelectedMode=SF_DUEL_LOCAL;sfCampaignRestoreDuelShips();
    std::puts("PASS: human cooperative shots keep bounded launch dispersion; Orion anticipates; ordinary shots never steer in flight");
}

static std::string sfTestReadBytes(const std::string &path)
{
    std::ifstream file(path,std::ios::binary);
    return std::string((std::istreambuf_iterator<char>(file)),{});
}

static void testUnknownSaveWithBackup(const std::string &directory)
{
    const std::string path=directory+"/protected.dat";
    const std::string unknown="SPACEFORTRESS_CAMPAIGN 9\nfuture data\n";
    SfCampaignSave known;known.cleared=4;known.selected=4;
    const auto backup=sfEncodeCampaign(known);
    for (bool changedAfterLoad : {false,true}) {
        {std::ofstream out(path);out<<(changedAfterLoad ? backup : unknown);}
        {std::ofstream out(path+".bak");out<<backup;}
        sfCampaignStoragePath=path;sfCampaignLoaded=false;sfCampaignStorageBlocked=false;
        sfCampaignSave=SfCampaignSave{};sfLoadCampaign();
        if (changedAfterLoad) {std::ofstream out(path);out<<unknown;}
        assert(!sfSaveCampaign(known));
        assert(sfTestReadBytes(path)==unknown);
        assert(sfTestReadBytes(path+".bak")==backup);
    }
    sfCampaignStoragePath=directory+"/campaign-v1.dat";
    sfCampaignLoaded=false;sfCampaignStorageBlocked=false;sfLoadCampaign();
    std::puts("PASS: an unknown primary is never replaced by its old backup, including changes after load");
}

static void testSaveRecoveryPreservation(const std::string &directory)
{
    const auto path=directory+"/recovery.dat";
    SfCampaignSave known;known.cleared=3;known.selected=3;known.names={{"Fab","Yann","Éclipse"}};
    const auto good=sfEncodeCampaign(known);
    const std::string damaged="SPACEFORTRESS_CAMPAIGN 1\ninterrupted bytes\n";
    {std::ofstream out(path);out<<damaged;}
    {std::ofstream out(path+".bak");out<<good;}
    sfCampaignStoragePath=path;sfCampaignLoaded=false;sfCampaignStorageBlocked=false;
    sfLoadCampaign();assert(sfCampaignSave.names[2]=="Éclipse");
    assert(sfSaveCampaign(sfCampaignSave));
    bool preserved=false;DIR *files=opendir(directory.c_str());assert(files);
    while (auto *file=readdir(files)) {
        if (std::string(file->d_name).find("recovery.dat.preserved-")==0)
            preserved|=sfTestReadBytes(directory+"/"+file->d_name)==damaged;
    }
    closedir(files);assert(preserved);
    assert(sfTestReadBytes(path)==good && sfTestReadBytes(path+".bak")==good);
    auto invalid=known;invalid.cleared=201;
    assert(!sfSaveCampaign(invalid));
    assert(sfTestReadBytes(path)==good && sfTestReadBytes(path+".bak")==good);
    // A newer backup is just as valuable as a newer primary.
    const std::string future="SPACEFORTRESS_CAMPAIGN 9\nfuture backup\n";
    {std::ofstream out(path+".bak");out<<future;}
    assert(!sfSaveCampaign(known));
    assert(sfTestReadBytes(path)==good && sfTestReadBytes(path+".bak")==future);
    sfCampaignStoragePath=directory+"/campaign-v1.dat";
    sfCampaignLoaded=false;sfCampaignStorageBlocked=false;sfLoadCampaign();
    std::puts("PASS: backup recovery preserves corrupt bytes; invalid candidates and unknown backups never overwrite data");
}
