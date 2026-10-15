#pragma once
// This overlay is never drawn or hit-tested by the production SOLO session.
struct SfCoopSoloDiagnosticUi {
    SDL_Rect rect(int index,int width,int height) const {
        const int col=index%3,row=index/3;
        const int gap=std::max(4,int(width*.008f));
        const int w=(width-gap*4)/3,h=std::max(36,int(height*.055f));
        return {gap+col*(w+gap),int(height*.145f)+row*(h+gap),w,h};
    }
    void draw(SDL_Renderer *renderer,int width,int height,bool worldActive,bool bonusTestOpen) const {
        sfCoopButton(renderer,{int(width*.02f),8,int(width*.30f),int(height*.08f)},
                     bonusTestOpen?"TEST BONUS ON":"TEST BONUS OFF");
        sfCoopButton(renderer,{int(width*.355f),8,int(width*.29f),int(height*.08f)},
                     worldActive?"MONDE ON":"MONDE TEST");
        sfCoopButton(renderer,{int(width*.68f),8,int(width*.30f),int(height*.08f)},
                     worldActive?(sfCoop.bossPresent?"BOSS OFF":"BOSS TEST"):
                         sfCoop.bossPresent?"BOSS OFF":"BOSS TEST");
        if(bonusTestOpen){
            using sfsolo::bonus::Kind;
            const char *labels[]={"PV","ENERGIE","FURTIF","MISSILES","PRISON","GLACE","LAVE","METEORE","TOUS X8"};
            for(int test=0;test<9;++test)sfCoopButton(renderer,rect(test,width,height),labels[test]);
            if(!sfCoop.bossPresent)
                sfCoopCentered(renderer,width,int(height*.335f),"BOSS TEST REQUIS",2,{255,180,120,235});
        }
    }
};
