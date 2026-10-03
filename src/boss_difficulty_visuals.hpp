#pragma once

struct SfBossAppearance {SDL_Color aura;int tentacles;SDL_Color bodyTint;};
static SfBossAppearance sfBossAppearance(int encounter)
{
    const int tentacles[]={0,4,8,20};
    const auto rgb=sfProgressRgb(encounter);
    const float correction=.08f+.06f*sfBossTravelProgress(encounter);
    const auto channel=[&](std::uint8_t target){return Uint8(255*(1-correction)+target*correction+.5f);};
    return {{rgb.r,rgb.g,rgb.b,255},tentacles[sfDifficultyIndex(encounter)],
            {channel(rgb.r),channel(rgb.g),channel(rgb.b),255}};
}
static std::array<SDL_FPoint,15> sfTentaclePath(int boss,int index,int count,float time)
{
    std::array<SDL_FPoint,15> points{};
    const float angle=2*float(PI)*index/count+boss*.17f;
    const float length=.45f+((index*37+boss*19)%67)/100.0f;
    for(int j=0;j<15;++j) {
        const float u=j/14.0f;
        const float radial=.62f+length*u;
        const float wave=(.04f+.13f*u)*std::sin(time*(1.5f+(boss%10)*.06f)-u*7+index*1.71f);
        points[j]={std::cos(angle)*radial-std::sin(angle)*wave,
                   std::sin(angle)*radial+std::cos(angle)*wave};
    }
    return points;
}
// Shared by combat and the selector. Layer order: aura, serpents, original
// mesh, subtle additive reflections masked by the original atlas alpha.
static void sfDrawEncounterBoss(SDL_Renderer *renderer,SDL_Texture *atlas,int encounter,
                                 tupl centre,float radius,float time,float hit=0,float dying=0)
{
    const auto look=sfBossAppearance(encounter);const int boss=sfBossIndex(encounter);
    const float opacity=1-std::clamp(dying,0.0f,1.0f);
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    if (look.tentacles) {
        std::array<SDL_Vertex,192> halo{};std::array<int,768> indices{};
        for(int i=0;i<64;++i) {
            const float a=2*float(PI)*i/64;
            const float pulse=1+.025f*std::sin(time*1.4f+boss);
            const float radii[]={.50f,.94f,1.55f};
            for(int edge=0;edge<3;++edge) {
                const float r=radius*radii[edge]*pulse;
                auto color=look.aura;color.a=Uint8((edge==1 ? 48 : 0)*opacity);
                halo[i*3+edge]={{centre.x+std::cos(a)*r,centre.y+std::sin(a)*r},color,{0,0}};
            }
            for(int strip=0;strip<2;++strip) {
                const int a0=i*3+strip,b0=((i+1)%64)*3+strip;
                const int tri[]={a0,b0,a0+1,a0+1,b0,b0+1};
                for(int j=0;j<6;++j) indices[i*12+strip*6+j]=tri[j];
            }
        }
        SDL_RenderGeometry(renderer,nullptr,halo.data(),192,indices.data(),768);
        for(int i=0;i<look.tentacles;++i) {
            const auto path=sfTentaclePath(boss,i,look.tentacles,time);
            std::array<SDL_Vertex,30> mesh{};std::array<int,84> triangles{};
            for(int j=0;j<15;++j) {
                const auto p=path[j],a=path[std::max(0,j-1)],b=path[std::min(14,j+1)];
                const float length=std::max(.0001f,vlong(b.x-a.x,b.y-a.y));
                const float width=(.042f+(boss%5)*.004f)*(1-j/16.0f)*radius;
                const float nx=-(b.y-a.y)/length*width,ny=(b.x-a.x)/length*width;
                SDL_Color shade{Uint8(look.aura.r*.62f+25),Uint8(look.aura.g*.62f+25),Uint8(look.aura.b*.62f+25),Uint8(195*opacity)};
                mesh[j*2]={{centre.x+p.x*radius+nx,centre.y+p.y*radius+ny},shade,{0,0}};
                shade.r=Uint8(shade.r*.62f);shade.g=Uint8(shade.g*.62f);shade.b=Uint8(shade.b*.62f);
                mesh[j*2+1]={{centre.x+p.x*radius-nx,centre.y+p.y*radius-ny},shade,{0,0}};
                if(j<14) {const int k=j*2;const int tri[]={k,k+1,k+2,k+1,k+3,k+2};for(int n=0;n<6;++n) triangles[j*6+n]=tri[n];}
            }
            SDL_RenderGeometry(renderer,nullptr,mesh.data(),30,triangles.data(),84);
        }
    }
    sfDrawBoss(renderer,atlas,boss,centre,radius,time,hit,dying,look.bodyTint);
    if (atlas && look.tentacles && opacity>0) {
        auto vertices=sfBossVertices(atlas,boss,centre,radius,time,hit,dying);
        std::array<int,384> indices{};int offset=0;
        for(int row=0;row<9;++row) for(int col=0;col<9;++col) {
            const bool eye=(row==3 && (col==3 || col==5));
            const bool rim=((row==2 || row==6) && (col==2 || col==6)) ||
                           ((boss%3)==0 && row==4 && (col==1 || col==7));
            auto c=look.aura;c.a=Uint8((eye ? 38 : rim ? 27 : 0)*opacity);
            vertices[row*9+col].color=c;
            if(row<8 && col<8) {const int a=row*9+col;for(int j:{a,a+1,a+9,a+1,a+10,a+9}) indices[offset++]=j;}
        }
        SDL_SetTextureBlendMode(atlas,SDL_BLENDMODE_ADD);
        SDL_RenderGeometry(renderer,atlas,vertices.data(),81,indices.data(),384);
        SDL_SetTextureBlendMode(atlas,SDL_BLENDMODE_BLEND);
    }
}
