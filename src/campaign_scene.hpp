#pragma once
struct SfCampaignTextures {
    SDL_Renderer *renderer=nullptr;
    SDL_Texture *bosses=nullptr,*galaxy=nullptr,*orange=nullptr,*blue=nullptr,*rock=nullptr,*planet=nullptr;
    int planetIndex=-1;
};
inline std::vector<SfCampaignTextures> sfCampaignTextures;
static SfCampaignTextures &sfCoopTextures(SDL_Renderer *renderer)
{
    for (auto &t : sfCampaignTextures) if (t.renderer==renderer) return t;
    SfCampaignTextures t;t.renderer=renderer;
    t.bosses=IMG_LoadTexture(renderer,"resources/assets/pict/campaign/bosses.png");
    t.galaxy=IMG_LoadTexture(renderer,"resources/assets/pict/fond4hlz.png");
    t.orange=IMG_LoadTexture(renderer,"resources/assets/pict/remaster/player_orange.png");
    t.blue=IMG_LoadTexture(renderer,"resources/assets/pict/remaster/player_blue.png");
    t.rock=IMG_LoadTexture(renderer,"resources/assets/pict/aa1.png");
    sfCampaignTextures.push_back(t);return sfCampaignTextures.back();
}
static void sfCampaignForgetRenderer(SDL_Renderer *renderer)
{
    sfCampaignTextures.erase(std::remove_if(sfCampaignTextures.begin(),sfCampaignTextures.end(),
        [renderer](const auto &t){return t.renderer==renderer;}),sfCampaignTextures.end());
}
static SDL_Rect sfBossAtlasRect(SDL_Texture *texture,int index)
{
    constexpr int columns[]{0,177,343,503,672,846,1025,1202,1392,1578,1774};
    constexpr int rows[]{0,156,332,511,693,887};
    int w,h;SDL_QueryTexture(texture,nullptr,nullptr,&w,&h);
    const int col=index%10,row=index/10;
    const int x=(columns[col]+2)*w/1774,y=(rows[row]+2)*h/887;
    return {x,y,(columns[col+1]-2)*w/1774-x,(rows[row+1]-2)*h/887-y};
}
static void sfCoopDisc(SDL_Renderer *renderer,float x,float y,float radius,SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);
    for (int row=-int(radius);row<=int(radius);++row) {
        const int extent=int(std::sqrt(std::max(0.0f,radius*radius-row*row)));
        SDL_RenderDrawLine(renderer,int(x)-extent,int(y)+row,int(x)+extent,int(y)+row);
    }
}
static SDL_Color sfWorldColor(int seed,float light=1)
{
    const float hue=seed*2.399963f;
    return {Uint8(std::clamp((135+95*std::sin(hue))*light,0.0f,255.0f)),
            Uint8(std::clamp((140+85*std::sin(hue+2.1f))*light,0.0f,255.0f)),
            Uint8(std::clamp((160+75*std::sin(hue+4.2f))*light,0.0f,255.0f)),255};
}
static SDL_Texture *sfCampaignPlanet(SfCampaignTextures &textures,int boss)
{
    if (textures.planetIndex==boss && textures.planet) return textures.planet;
    if (textures.planet) SDL_DestroyTexture(textures.planet);
    textures.planet=nullptr;textures.planetIndex=boss;
    // Code-native procedural worlds: unique terrain, bands/craters, palette and
    // lighting seed for every encounter. Build once, never in each draw frame.
    constexpr int size=256;
    auto *surface=SDL_CreateRGBSurfaceWithFormat(0,size,size,32,SDL_PIXELFORMAT_RGBA32);
    if (!surface) return nullptr;
    SDL_LockSurface(surface);
    const auto base=sfWorldColor(boss+1),land=sfWorldColor(boss+15);
    for (int y=0;y<size;++y) for (int x=0;x<size;++x) {
        const float nx=(x-127.5f)/124,ny=(y-127.5f)/124,d=nx*nx+ny*ny;
        Uint32 pixel=0;
        if (d<1) {
            const float nz=std::sqrt(1-d),lon=std::atan2(nx,nz),lat=std::asin(ny);
            const float seed=boss*1.713f;
            const float noise=std::sin(lon*(4+boss%5)+seed+std::sin(lat*9))*.44f+
                std::sin(lat*(8+boss%7)+seed*.7f+std::sin(lon*8))*.32f+
                std::sin(lon*23+lat*17+seed)*.14f+std::sin(lon*47-lat*37)*.07f;
            float mix=.5f+.5f*noise,detail=1;
            switch (boss%5) {
                case 0: mix=noise>.08f ? .85f : .08f;detail=noise>.08f ? .8f+noise*.4f : .82f;break;
                case 1: mix=.5f+.45f*std::sin(lat*32+noise*4+seed);break;
                case 2: mix=std::abs(lat)>1.05f ? 1 : .4f+noise*.25f;detail=1.1f;break;
                case 3: mix=std::pow(std::max(0.0f,1-std::abs(noise)*8),4);detail=.48f+mix*.7f;break;
                default: mix=.4f+noise*.5f;detail=.82f;
                    for (int c=0;c<9;++c) {
                        const float cx=.78f*std::sin(c*2.4f+seed),cy=.76f*std::cos(c*1.7f-seed);
                        const float radius=.025f+.017f*(c%4);
                        const float distance=vlong(nx-cx,ny-cy);
                        if (distance<radius) detail*=.55f+.35f*distance/radius;
                        else if (distance<radius*1.18f) detail*=1.13f;
                    }
            }
            const float diffuse=std::max(.09f,-nx*.42f-ny*.30f+nz*.78f);
            const float atmosphere=std::pow(1-nz,3)*.28f;
            auto channel=[&](int a,int b) {return Uint8(std::clamp((a*(1-mix)+b*mix)*detail*(.18f+diffuse)+atmosphere*110,0.0f,255.0f));};
            pixel=SDL_MapRGBA(surface->format,channel(base.r,land.r),channel(base.g,land.g),channel(base.b,land.b),
                Uint8(std::clamp((1-std::sqrt(d))*size*.5f,0.0f,1.0f)*255));
        }
        auto *row=reinterpret_cast<Uint32*>(static_cast<Uint8*>(surface->pixels)+y*surface->pitch);row[x]=pixel;
    }
    SDL_UnlockSurface(surface);textures.planet=SDL_CreateTextureFromSurface(textures.renderer,surface);SDL_FreeSurface(surface);
    if (textures.planet) {SDL_SetTextureBlendMode(textures.planet,SDL_BLENDMODE_BLEND);SDL_SetTextureScaleMode(textures.planet,SDL_ScaleModeLinear);}
    return textures.planet;
}
static std::string sfDisplayText(const std::string &input)
{
    std::string out;
    for (size_t i=0;i<input.size();++i) {
        const unsigned char c=input[i];
        if (c<128) out+=c>='a' && c<='z' ? char(c-'a'+'A') : char(c);
        else if (c==0xc3 && i+1<input.size()) {
            const unsigned char n=input[++i];
            if ((n>=0x80&&n<=0x85)||(n>=0xa0&&n<=0xa5)) out+='A';
            else if ((n>=0x88&&n<=0x8b)||(n>=0xa8&&n<=0xab)) out+='E';
            else if ((n>=0x8c&&n<=0x8f)||(n>=0xac&&n<=0xaf)) out+='I';
            else if ((n>=0x92&&n<=0x96)||(n>=0xb2&&n<=0xb6)) out+='O';
            else if ((n>=0x99&&n<=0x9c)||(n>=0xb9&&n<=0xbc)) out+='U';
            else if (n==0x87||n==0xa7) out+='C';else if (n==0x91||n==0xb1) out+='N';else out+='?';
        } else if ((c&0xc0)!=0x80) out+='?';
    }
    return out;
}
static void sfCoopText(SDL_Renderer *r,int x,int y,const std::string &value,int maxWidth,int preferred=3,SDL_Color color={228,241,255,255})
{
    const auto text=sfDisplayText(value);int w,h;SDL_GetRendererOutputSize(r,&w,&h);
    const int scale=std::max(1,std::min({preferred,maxWidth/std::max(1,int(text.size())*6),std::max(1,h/330)}));
    sfUiText(r,x,y,text.c_str(),scale,color.r,color.g,color.b,color.a);
}
static void sfCoopCentered(SDL_Renderer *r,int width,int y,const std::string &value,int preferred=3,SDL_Color color={228,241,255,255})
{
    const auto text=sfDisplayText(value);int w,h;SDL_GetRendererOutputSize(r,&w,&h);
    const int scale=std::max(1,std::min({preferred,int(width*.9f)/std::max(1,int(text.size())*6),std::max(1,h/130)}));
    sfUiText(r,(width-sfUiTextWidth(text.c_str(),scale))/2,y,text.c_str(),scale,color.r,color.g,color.b,color.a);
}
static void sfCoopButton(SDL_Renderer *r,SDL_Rect rect,const std::string &label)
{
    sfUiPanel(r,rect,7,24,42,70,220,205);
    const int scale=std::max(1,std::min(int(rect.h/12),int(rect.w*.9f)/std::max(1,int(label.size())*6)));
    sfUiText(r,rect.x+(rect.w-sfUiTextWidth(label.c_str(),scale))/2,rect.y+(rect.h-7*scale)/2,label.c_str(),scale,231,250,255);
}
static std::array<SDL_Vertex,81> sfBossVertices(SDL_Texture *atlas,int boss,tupl centre,float radius,float time,float hit=0,float dying=0)
{
    std::array<SDL_Vertex,81> vertices{};int tw,th;SDL_QueryTexture(atlas,nullptr,nullptr,&tw,&th);
    const auto src=sfBossAtlasRect(atlas,boss);const int family=boss%10;
    for (int row=0;row<9;++row) for (int col=0;col<9;++col) {
        const float u=col/8.0f,v=row/8.0f,x=u*2-1,y=v*2-1,edge=std::max(std::abs(x),std::abs(y));
        const float breathing=1+.025f*std::sin(time*2+boss);
        float dx=.022f*edge*std::sin(time*3+y*4+family),dy=.019f*edge*std::sin(time*2.7f+x*5+boss);
        if (family==2 || family==7) {dx+=.06f*std::max(0.0f,y)*std::sin(time*4+y*5+x*2);dy+=.045f*std::max(0.0f,y)*std::cos(time*3+x*4);}
        if (family==1 || family==8) {dy+=.06f*std::abs(x)*std::sin(time*5+y*6);dx+=.025f*y*std::sin(time*4);}
        if (family==5) dy+=.10f*x*x*std::sin(time*4+std::abs(x)*2);
        if (family==0 || family==4 || family==6 || family==9) {
            const float a=std::atan2(y,x);dx+=x*.035f*edge*std::sin(time*3+a*4);dy+=y*.035f*edge*std::sin(time*3+a*4);
        }
        if (family==3) {dx+=x*.06f*std::sin(time*3)*(1-y*y);dy+=y*.04f*std::sin(time*3+.6f);}
        const float death=std::clamp(dying,0.0f,1.0f),collapse=1-death*.75f,turn=.04f*std::sin(time*.8f+family);
        const float px=(x*breathing+dx)*radius*collapse,py=(y*breathing+dy)*radius*collapse;
        auto &vert=vertices[row*9+col];
        vert.position={centre.x+px*std::cos(turn)-py*std::sin(turn),centre.y+px*std::sin(turn)+py*std::cos(turn)};
        vert.color={255,Uint8(hit>0 ? 195 : 255),Uint8(hit>0 ? 165 : 255),Uint8(255*(1-death))};
        vert.tex_coord={(src.x+(.005f+u*.99f)*src.w)/tw,(src.y+(.005f+v*.99f)*src.h)/th};
    }
    return vertices;
}
static void sfDrawBoss(SDL_Renderer *r,SDL_Texture *atlas,int boss,tupl centre,float radius,float time,float hit=0,float dying=0)
{
    if (!atlas) {sfCoopDisc(r,centre.x,centre.y,radius*.6f,{180,60,230,220});return;}
    const auto vertices=sfBossVertices(atlas,boss,centre,radius,time,hit,dying);
    std::array<int,384> indices{};int n=0;
    for (int y=0;y<8;++y) for (int x=0;x<8;++x) {
        const int a=y*9+x;for (int i : {a,a+1,a+9,a+1,a+10,a+9}) indices[n++]=i;
    }
    SDL_SetTextureBlendMode(atlas,SDL_BLENDMODE_BLEND);
    SDL_RenderGeometry(r,atlas,vertices.data(),int(vertices.size()),indices.data(),int(indices.size()));
}
static void sfDrawCampaignSpace(SDL_Renderer *r,int boss,float time,int width,int height)
{
    auto &t=sfCoopTextures(r);SDL_SetRenderDrawColor(r,2,5,16,255);SDL_RenderClear(r);
    SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);
    const SDL_Color moods[]={{105,180,235,255},{230,160,100,255},{160,110,225,255},{100,210,165,255},{150,195,240,255},{230,110,145,255}};
    const auto mood=moods[sfBossCatalog()[boss].backdrop];
    if (t.galaxy) {
        int tw,th;SDL_QueryTexture(t.galaxy,nullptr,nullptr,&tw,&th);
        const float aspect=float(width)/height;SDL_Rect source{0,0,tw,th};
        if (tw/float(th)>aspect) {source.w=int(th*aspect);source.x=int((tw-source.w)*(.5f+.4f*std::sin(boss*1.71f+time*.011f)));}
        else {source.h=int(tw/aspect);source.y=int((th-source.h)*(.5f+.4f*std::sin(boss*1.71f+time*.011f)));}
        SDL_Rect full{0,0,width,height};
        SDL_SetTextureColorMod(t.galaxy,mood.r,mood.g,mood.b);SDL_RenderCopy(r,t.galaxy,&source,&full);
        SDL_SetTextureColorMod(t.galaxy,255,255,255);
        SDL_SetRenderDrawColor(r,2,5,14,125);SDL_RenderFillRect(r,&full);
    }
    uint32_t seed=0x1968ab12u+boss*71539u;
    for (int i=0;i<110;++i) {
        seed=seed*1664525u+1013904223u;const int x=seed%std::max(1,width);
        seed=seed*1664525u+1013904223u;const int y=seed%std::max(1,height);
        const Uint8 alpha=Uint8(90+65*(1+std::sin(time*.7f+i)));
        SDL_SetRenderDrawColor(r,185,225,255,alpha);SDL_RenderDrawPoint(r,x,y);
    }
    auto *planet=sfCampaignPlanet(t,boss);
    const float diameter=std::min(width,height)*(.37f+.012f*(boss%7));
    const float x=width*(.5f+.26f*std::sin(boss*1.37f+time*.024f));
    const float y=height*(boss%2 ? .76f+.035f*std::sin(time*.019f+boss) : .23f+.035f*std::sin(time*.019f+boss));
    if (boss%7==3) for (int i=0;i<5;++i) {
        std::array<SDL_FPoint,65> ring;
        for (int k=0;k<=64;++k) {const float a=k*2*float(PI)/64;ring[k]={x+std::cos(a)*diameter*(.64f+i*.009f),y+std::sin(a)*diameter*.16f+std::cos(a)*diameter*.12f};}
        SDL_SetRenderDrawColor(r,mood.r,mood.g,mood.b,100);SDL_RenderDrawLinesF(r,ring.data(),int(ring.size()));
    }
    if (planet) {
        SDL_Rect dest{int(x-diameter*.5f),int(y-diameter*.5f),int(diameter),int(diameter)};
        SDL_SetTextureAlphaMod(planet,225);SDL_RenderCopyEx(r,planet,nullptr,&dest,time*.4f,nullptr,SDL_FLIP_NONE);
    }
}
static void sfCoopDrawArena(SDL_Renderer *r,int width,int height)
{
    auto &t=sfCoopTextures(r);sfDrawCampaignSpace(r,sfCoop.boss,sfCoop.time+sfCoop.phaseTime,width,height);
    for (const auto &b : sfCoop.beams) {
        const bool active=b.age>=b.warning;const float dx=std::cos(b.angle),dy=std::sin(b.angle),range=std::max(width,height)*2;
        SDL_SetRenderDrawColor(r,255,active ? 210 : 80,active ? 150 : 90,active ? 235 : 150);
        const int thick=active ? std::max(3,int(std::min(width,height)*.027f)) : 2;
        for (int i=-thick/2;i<=thick/2;++i) SDL_RenderDrawLine(r,int(b.origin.x-dy*i),int(b.origin.y+dx*i),int(b.origin.x+dx*range-dy*i),int(b.origin.y+dy*range+dx*i));
    }
    for (const auto &w : sfCoop.waves) for (int i=0;i<3;++i)
        sfUiCircle(r,int(w.origin.x),int(w.origin.y),int(w.age<.7f ? sfCoopBossRadius()+w.age*25 : w.radius)+i,230,80,190);
    for (const auto &rock : sfCoop.rocks) if (t.rock) {
        SDL_Rect dest{int(rock.position.x-rock.radius),int(rock.position.y-rock.radius),int(rock.radius*2),int(rock.radius*2)};
        SDL_RenderCopyEx(r,t.rock,nullptr,&dest,sfCoop.time*17,nullptr,SDL_FLIP_NONE);
    }
    for (const auto &d : sfCoop.dust) sfCoopDisc(r,d.position.x,d.position.y,2.5f,{125,235,255,220});
    for (const auto &s : sfCoop.shots) {
        SDL_Color color=s.owner==0 ? SDL_Color{255,175,75,255} : s.owner==1 ? SDL_Color{100,220,255,255} : SDL_Color{255,70,140,255};
        if (s.kind==2) color=s.age<1.2f ? SDL_Color{255,200,75,190} : SDL_Color{255,70,50,255};
        sfCoopDisc(r,s.position.x,s.position.y,s.radius*1.7f,{color.r,color.g,color.b,65});
        sfCoopDisc(r,s.position.x,s.position.y,s.radius,color);
    }
    const float death=sfCoop.phase==SfCoopPhase::Dying ? std::clamp(sfCoop.phaseTime/1.6f,0.0f,1.0f) :
        sfCoop.phase==SfCoopPhase::Name || sfCoop.phase==SfCoopPhase::Saved ? 1 : 0;
    if (sfCoop.warning>0 && death==0) for (int i=0;i<2;++i)
        sfUiCircle(r,int(sfCoop.position.x),int(sfCoop.position.y),int(sfCoopBossRadius()*(1.1f+.14f*sfCoop.warning))+i*2,255,140,100);
    sfDrawBoss(r,t.bosses,sfCoop.boss,sfCoop.position,sfCoopBossRadius(),sfCoop.time+sfCoop.phaseTime,sfCoop.hit,death);
    if (death>0 && death<1) for (int i=0;i<40;++i) {
        const float a=i*2.39996f,d=sfCoopBossRadius()*(.5f+death*(1.5f+(i%4)*.4f));
        sfCoopDisc(r,sfCoop.position.x+std::cos(a)*d,sfCoop.position.y+std::sin(a)*d,std::max(1.0f,(1-death)*7),{255,Uint8(90+i%120),100,Uint8(255*(1-death))});
    }
    for (int owner=0;owner<2;++owner) {
        const auto *s=sfCoopShip(owner);auto *texture=owner==0 ? t.orange : t.blue;const int size=int(std::min(width,height)*.15f);
        SDL_Rect rect{int(s->x-size*.5f),int(s->y-size*.5f),size,size};
        if (texture) {
            SDL_SetTextureColorMod(texture,s->pv>0 ? 255 : 95,s->pv>0 ? 255 : 95,s->pv>0 ? 255 : 95);
            SDL_SetTextureAlphaMod(texture,sfCoop.invulnerable[owner]>0 && int(sfCoop.time*12)%2 ? 140 : 255);
            SDL_RenderCopyEx(r,texture,nullptr,&rect,sfCoop.time*(owner==0 ? 17 : -17),nullptr,SDL_FLIP_NONE);
            SDL_SetTextureColorMod(texture,255,255,255);SDL_SetTextureAlphaMod(texture,255);
        }
        if (s->pv<=0) {
            sfCoopText(r,int(s->x-size*.6f),int(s->y+size*.6f),sfCoop.revives>0 ? "SECOURIR 2 S" : "HORS COMBAT",int(size*1.5f),2,{255,150,150,255});
            sfUiCircle(r,int(s->x),int(s->y),int(sfCoopShipRadius()*1.8f),230,100,100);
        }
    }
    sfDrawTacticalEffects(r);
    const int margin=std::max(8,width/30),barWidth=int(width*.58f),barHeight=std::max(5,std::min(width,height)/80);
    sfCoopText(r,margin,int(height*.025f),"BOSS "+std::to_string(sfCoop.boss+1)+" / 50",int(width*.7f),std::max(2,width/220),{255,190,120,255});
    sfCoopText(r,margin,int(height*.05f),sfCoopProfile().name,int(width*.72f),std::max(2,width/260));
    SDL_Rect bar{margin,int(height*.079f),barWidth,barHeight};SDL_SetRenderDrawColor(r,30,28,45,235);SDL_RenderFillRect(r,&bar);
    bar.w=int(barWidth*std::clamp(sfCoop.health/sfCoopProfile().health,0.0f,1.0f));SDL_SetRenderDrawColor(r,230,65,125,255);SDL_RenderFillRect(r,&bar);
    sfCoopButton(r,{int(width*.81f),int(height*.02f),int(width*.16f),int(height*.062f)},"PAUSE");
    for (int owner=0;owner<2;++owner) {
        const auto *s=sfCoopShip(owner);const int x=owner==0 ? margin : width/2+margin/2,w=int(width*.43f),y=int(height*.935f);
        const SDL_Color color=owner==0 ? SDL_Color{255,180,95,255} : SDL_Color{100,210,255,255};
        sfCoopText(r,x,int(height*.908f),owner==0 ? (sfActiveMode==SF_COOP_AI ? "ORION IA" : "PILOTE ORANGE") : "PILOTE BLEU",w,std::max(2,width/280),color);
        SDL_Rect hp{x,y,w,barHeight};SDL_SetRenderDrawColor(r,30,40,60,220);SDL_RenderFillRect(r,&hp);
        hp.w=int(w*std::clamp(s->pv/1000,0.0f,1.0f));SDL_SetRenderDrawColor(r,color.r,color.g,color.b,255);SDL_RenderFillRect(r,&hp);
        sfCoopText(r,x,y+barHeight+4,"ENERGIE "+std::to_string(int(std::lround(100*(1-sfShipHeat(s->nrj)/50))))+"%",w,std::max(2,width/300),color);
    }
}
