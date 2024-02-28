#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <th1.h>
#include <t.hpp>
#include <th2.h>
#include <th3.h>
#include <th4.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
//#include <t.cs>
#include <t.hpp>
//#include <fablib.hpp> //dans t.hpp

//#include <Col.cs>
#include <cstdlib>
//#include <chrono>
#include <list>
#include <cmath>
#define ppi 3.141592653589793238462643383 

//#include
//#include "SDL_events_c.h" sdl dans la lib du prog
//#include <SDL_events>
//./resources./resources/./resources/assets/ forr compiler v2304
//#include <boost/thread/thread.hpp>
#include <ctime>


//float rt = ppi/180;

//global for extern va>
extern tests *c;
float r1,r2,r3,r4,r5,r6,r7,r8,r9;
float  k0=1;

//std::list<touch*> Vec[10];

float xt[100];
float yt[100];
bool iaast=true;
bool setgui=false;
bool setia=false;
int nbt=0;
int contgui=0;



touch *Vec=new touch;
//touch *Vecg=new touch;
//touch *Vecd=new touch;
//touch *Vecg2=new touch;
//touch *Vecd2=new touch;
//touch *Vecg3=new touch;
//touch *Vecd3=new touch;
// here define aspect basement.
// var environment xy envxy et envminpoint
// produit xy div par  envxy de reference par 4
// prevoir sur thread fps void def usedpoint
int envxy,usedpoint,envxyref;
//test de fablib dans ia
enti *iago=new enti;
enti *iago1=new enti;
enti *iacalc=new enti;
enti *iatake= new enti;

float refwh;//sera sera la base de taille w*h
parts *ts = new parts(0,0);
parts *ps = new parts(0,0);

sprite *Spritej1=new sprite;
sprite *iaentre=new sprite;
sprite *ia1=new sprite;
sprite *ia0=new sprite;
sprite *ia00=new sprite;
sprite *ia3=new sprite;
sprite *ia4=new sprite;

sprite *Spritej2=new sprite;
sprite *ia2=new sprite;

sprite *Spritefond=new sprite;
sprite *Spritefond2=new sprite;
sprite *Spritefondast=new sprite;
sprite *Spritefondast2=new sprite;
sprite *Spritefondast3=new sprite;
sprite *Spritefondast4=new sprite;
sprite *Spritefondast5=new sprite;
sprite *Spritefond000=new sprite;
sprite *Spritefondjup=new sprite;
sprite * Suiveur=new sprite;
sprite *a1=new sprite;
sprite *a3=new sprite;
sprite *a4=new sprite;
sprite *b1=new sprite;
sprite *a2=new sprite;
sprite *b2=new sprite;
eexpl *e1=new eexpl;
part *b3=new part(0,0,0);
sprite *Spritesbl=new sprite;
sprite *Spritesrl=new sprite;
sprite *Spritegunbl=new sprite;
sprite *Spritegunrl=new sprite;
sprite *Spritecoeurrl=new sprite;
sprite *Spritecoeurbl=new sprite;
sprite *loosej1=new sprite;
sprite *loosej2=new sprite;
sprite *Spriteeclr=new sprite;
sprite *Spriteeclb=new sprite;
sprite *SpriteVEC=new sprite;
sprite *cont=new sprite;
sprite *iaoo=new sprite;
sprite *rouage1=new sprite;
sprite *rouage2=new sprite;


std::list<sprite*> sa1;
std::list<sprite*> sa2;


std::list<sprite*> entitiesj1;
std::list<sprite*> entitiesj2;
std::list<eexpl*> explos;
std::list<sprite*> burnsj1;
std::list<sprite*> burnsj2;
std::list<sprite*> asts;


std::list<SDL_Rect> texrtj1;
std::list<SDL_Rect> texrtj2;
std::list<SDL_Rect> texrexplos;
std::list<SDL_Rect> texrburns;
std::list<SDL_Rect> texrasts;
std::list<SDL_Rect> texra1;
std::list<SDL_Rect> texra2;
std::list<SDL_Rect> texrp;

std::list<part*> debris;
std::list<parts*> particules;
std::list<parts*> particulesr;


int tirj1=0;
int tirj2=0;
bool tirjz = false;
bool tirj1z = false;
bool tirj2z = false;
bool explo1z = false;
bool explo2z = false;
bool choixast=false;//1 pour pas de choix ciblé


int tirj2max=10;


//touch *tl= new touch();
//tl->settings(i,500,500);
//Vec.push_back(tl);
             
                
//vect *vectlist = new vect(1000,0,0); //good one sample

int bcltests=100000000;
//apap apli en arriere plan
bool suiveur(){
	if (Suiveur->pv==0) return true;
	}
bool apap=false;
bool runed=true;
bool tics;
bool tics1;
bool tics2;
bool tics3;
bool tics4;
int cycles=0;
bool sdlstarted=true; // pour var ebent th1
double coef=1;

//int passevar=99990000;
double cor=1.03;
int intmint=10000;
int intlatence=100;
double stableproc;
float tw,th,vx1,vx2,vy1,vy2;
int maxa=100;
int incra1=0;
int incra2=0;
double angletest=0;


//double mintime=100;//0.2;//meilleur benchmark 60000 50 000 groupe d'operations stablestest sur ce tel à 85% samsung a42. 200 micro secondes
//globalvar<
//ici classes
//class ts{
//std::list <t> listt;
    //listt.push_front;
//ici classes et statics

//
sprite *varcol=new sprite;
void convs(eexpl *aa){
   varcol->setxywh(
   aa->x,aa->y,aa->w,aa->h
   );  
    }


bool colee(sprite *a,sprite *b)
{
  r1=2*0.85;
  return ((b->x - a->x)*(b->x - a->x)+
         (b->y - a->y)*(b->y - a->y))<
         ((a->sh + b->sh)/r1*(a->sh + b->sh)/r1)/r1;

  //return ((b->xm - a->xm)*(b->xm - a->xm)+
        // (b->ym - a->ym)*(b->ym - a->ym))<
       //  ((a->sh + b->sh)/r1*(a->sh + b->sh)/r1)/r1;
}
bool coleee(eexpl *a,sprite *b){
    convs(a);
    return colee(varcol,b);
    }
 bool cole(float xx,float yy,sprite *b){
    varcol->x=xx;varcol->y=yy;
    varcol->w=b->w;varcol->h=b->h;
    return colee(varcol,b);
    }
bool inxy(sprite *a)
{
  r1=2*1.01;
  return (a->x>0-w*0.2 and a->x<W*1.2) and 
  (a->y>0-H*0.2 and a->y<H*1.2);

  //return ((b->xm - a->xm)*(b->xm - a->xm)+
        // (b->ym - a->ym)*(b->ym - a->ym))<
       //  ((a->sh + b->sh)/r1*(a->sh + b->sh)/r1)/r1;
}

void eclats(sprite *c,sprite *cc){
    //std::cout << " in a ";
    float x=c->x;float y=c->y;float t =c->w;float tt=cc->w;
    if (t<tt) t=tt;
    if ((t>HEIGHT/200)){//anc100
      //"std::cout << " in b ";
      sprite *a1 = new sprite;
	    a1->setv(x,y,t/2,t/2,-1,-1,1);
	    a1->pv=1;a1->name="a1";
	    sprite *a2 = new sprite;
	a2->setv(x,y,t/2,t/2,-1,-1,1);
	a2->pv=1;a2->name="a2";
	    sprite *a3 = new sprite;
	    a3->setv(x,y,t/2,t/2,-1,-1,1);
	    a3->pv=1;a3->name="a3";
	    sprite *a4 = new sprite;
	a4->setv(x,y,t/2,t/2,-1,-1,1);
	a4->pv=1;a4->name="a4";
	//while(colee(a1,a2) or colee(a1,a3) 
	//or colee(a1,a4) or colee(a2,a3) 
	//or colee(a2,a4) or colee(a3,a4)){
	    //for (int i=0;i<10;++i){
	    //a1->ast();a2->ast();a3->ast();a4->ast();}
	    a1->incr=incra1+1;a2->incr=incra1+2;
	    a3->incr=incra1+3;a4->incr=incra1+4;
	    sa1.push_back(a4);  
	    sa1.push_back(a2);  
	    sa1.push_back(a1);
	    sa1.push_back(a3);incra1+=4;
}}
void setasts(int c){
for (int i=0;i<c;++i) {
	    sprite *a1 = new sprite;
	    a1->setv(-1,-1,HEIGHT/10,HEIGHT/10,-1,-1,1);
	    a1->pv=1;a1->name="a1";
	    incra1+=1;
	    a1->incr=incra1;
	    a1->x=-W/4;
	    a1->y=H+H/4;
	    sa1.push_back(a1);
	    sprite *a2 = new sprite;
	    a2->setv(-1,-1,HEIGHT/10,HEIGHT/10,-1,-1,1);
	    a2->pv=1;a2->name="a2";
	    incra1+=1;
	    a2->incr=incra1;
	    a2->x=W+W/4;
	    a2->y=-H/4;
	    sa1.push_back(a2);
	    sprite *a3 = new sprite;
	    a3->setv(-1,-1,HEIGHT/10,HEIGHT/10,-1,-1,1);
	    a3->pv=1;a3->name="a3";
	    incra1+=1;
	    a3->incr=incra1;
	    a3->x=W+W/4;
	    a3->y=H+H/4;
	    sa1.push_back(a3);
	    //incra2+=1;
	    sprite *a4 = new sprite;
	    a4->setv(-1,-1,HEIGHT/10,HEIGHT/10,-1,-1,1);
	    incra1+=1;
	    a4->pv=1;a4->name="a4";
	    a4->incr=incra1;
	    a4->x=-W/4;
	    a4->y=-H/4;
	    sa1.push_back(a4);
	    
	    
		}}
void setastswall(){
	int c=1;
for (int i=0;i<c;++i) {
	int coefsisewall=20;
	    sprite *a1 = new sprite;
	    a1->setv(-1,-1,HEIGHT/coefsisewall,HEIGHT/coefsisewall,-1,-1,1);
	    a1->pv=1;a1->name="a1";
	    incra1+=1;
	    a1->incr=incra1;
	    a1->x=W/4;
	    a1->y=H/2;
	    a1->vx=0;a1->vy=0;a1->h*=1.5;a1->w*=1.5;
	    sa1.push_back(a1);
	    sprite *a2 = new sprite;
	    a2->setv(-1,-1,HEIGHT/coefsisewall,HEIGHT/coefsisewall,-1,-1,1);
	    a2->pv=1;a2->name="a2";
	    incra1+=1;
	    a2->incr=incra1;
	    a2->x=(W/4)*3;
	    a2->y=H/2;
	    a2->vx=0;a2->vy=0;a2->h*=1.4;a2->w*=1.4;
	    sa1.push_back(a2);
	    sprite *a3 = new sprite;
	    a3->setv(-1,-1,HEIGHT/coefsisewall,HEIGHT/coefsisewall,-1,-1,1);
	    a3->pv=1;a3->name="a3";
	    incra1+=1;
	    a3->incr=incra1;
	    a3->x=W/2;
	    a3->y=H/2.5;
	    a3->vx=0;a3->vy=0;a3->h*=1.4;a3->w*=1.4;
	    sa1.push_back(a3);
	    //incra2+=1;
	    sprite *a4 = new sprite;
	    a4->setv(-1,-1,HEIGHT/coefsisewall,HEIGHT/coefsisewall,-1,-1,1);
	    incra1+=1;
	    a4->pv=1;a4->name="a4";
	    a4->incr=incra1;
	    a4->x=W/2;
	    a4->y=H/1.6;
	    a4->vx=0;a4->vy=0;a4->h*=1.3;a4->w*=1.3;
	    sa1.push_back(a4);
	    
	    
		}}

void closesdl()
{
    //Free loaded images
    //xxxTexture.free();

    //Free the sound effects
    //Mix_FreeChunk( xxx );
    
    
    //Free the music
    //Mix_FreeMusic( xxx );
    //xxx = NULL;

    //Destroy window    
    //SDL_DestroyRenderer( Renderer );
    //SDL_DestroyWindow( Win );
    //Win = NULL;
    //Renderer = NULL;

    //Quit SDL subsystems
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

//ici void threads>


void ttics(double fps2)
{ 
//while(runed){
	//double atime[100];//pour debug tics
    //for (int i=1;i<100;++i){
        //atime[i]=intmint;
    //}
    //int btime=0;//pour debug tics
	bool st =false;
	//double realtic;//pour debug tics
	double a;
	double b;
	auto deb = std::chrono::high_resolution_clock::now();
	for (int i=1;i<bcltests*8;++i) {// pour test
	//std::cout<< "physics" << std::endl;
	tics=false;
	usleep(intlatence);
	tics=true;
	//btime++;//pour debug tics
	//if(btime>100) btime=0;//pour debug tics
	//usleep(intmint*0.5);
	    if (st){
        st=false;
        //usleep(int(1000*mintime*coef));
        //usleep(int(1000*mintime-76));
        auto end = std::chrono::high_resolution_clock::now();
        
        //std::cout <<"end"<< std::flush;
        std::chrono::duration<double, std::milli> elapsed2 = end-deb;
        b=elapsed2.count();
        while ((b-a)<intmint/1000*0.99){
             usleep(intmint/100);
             auto end = std::chrono::high_resolution_clock::now();
             std::chrono::duration<double, std::milli> elapsed2 = end-deb;
             b=elapsed2.count();
        }
        tics=false;
        usleep(intlatence);
        tics=true;
        cycles++;
        if (cycles>8) cycles=0;
        if ((cycles==8) or (cycles==6) or(cycles==4) or (cycles==2) or (cycles==1)) tics2=false;
        if ((cycles==8) or (cycles==4)) tics3=false;
        if (cycles==8) tics4=false;
        //atime[btime]=b-a;//pour debug tics
        //for (int it=1;it<100;++it){//pour debug tics
        //realtic=atime[it]+realtic;
        //}
        //realtic/=100;//pour debug tics
        //std::cout <<" tic   》》》" << b-a<< " 《 《"<< realtic << std::endl;//pour debug tics
        //stableproc=reftime95;
        //atime[btime]=mintime/(b-a);
    }
    else{
        //std::cout << mintime << "  " << mintime/(b-a)<<" Hello waiter \n" << std::flush;
        st=true;
        //std::cout <<"start"<< std::flush;
        auto start = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed1 = start-deb;
        //std::cout << "》     tic        》" << elapsed1.count() << " ms\n";
        a=elapsed1.count();
    }//else
    
	
	//usleep(1000*mintime*coef-cor*3500);
}//end runed ou fori
}
void threadphysics(double fps)
{ 

//while(runed){
	//bool up=false;
	
	
	
	usleep(intmint);
	for (int i=1;i<bcltests*8;++i) {// pour test
	//std::cout<< "physics" << std::endl;
	while (tics) usleep(intlatence*3);
    //th1();
    Spritej1->collide();
    ia1->collide();
    ia2->collide();
    Spritej2->collide();
    		//ia1 repulse nearbye ia2 faraway atractive

    
    
    
    
    //usleep(intlatence*3);
    
    
	
	//usleep(1000*mintime*coef-cor*3500);
}//end runed ou fori
}
//here cleaning code for uncopying! fps part of
float missnrjminus(sprite *e){
	    float a=sqrt(e->vy*e->vy)
          *sqrt(e->vy*e->vy)*0.010;
          return a;
          }
float misspvminus(sprite *e){
	   float a=rand()%10+e->nrj*e->nrj*0.03*25;
          return a;
          }
float pvminus(sprite *e){
    float a=rand()%10+e->nrj*e->nrj*0.03;
    return a;
    }	    
float nrjminus(sprite *e){
    float a=sqrt(e->vy*e->vy)
          *sqrt(e->vy*e->vy)*0.005;
    return a;
}

float vectoriser(float va,float vaa){
    float rep=sqrt(va*va)/(sqrt((va*va))+sqrt((vaa*vaa)));
    return rep;
}


float vectorisersign(float va,float vaa){
    float rep=sqrt(va*va)/(sqrt((va*va))+sqrt((vaa*vaa)));
    if (va!=0) rep*=va/sqrt(va*va);
    
    return rep;
}

void sdlxy(enti *ei, SDL_Rect *eei){
    eei->x=ei->xy.x-eei->w/2;
    eei->y=ei->xy.y-eei->h/2;
    
}
void tirerj2(){
            if (tirj2<50)
            {
            
            Spritej2->nrj+=1;
            tirj2+=1;
            //if (tirj2>50) tirj2=0;
           bj1 *b1 = new bj1();
           b1->idx=tirj2;
           b1->x=Spritej2->x;
           b1->y=Spritej2->y;
           b1->vx=0.5*(rand()%3-1);
           b1->vy=(60-Spritej2->nrj)*-0.4;
           if (Spritej2->nrj<1.5){b1->name="miss";Spritej2->nrj+=10;
           tirjz=true;
           b1->vy*=0.01;b1->h*=2;b1->vx=0;}
           else tirj1z=true;
           if (Spritej2->nrj>50) Spritej2->nrj=50;
           entitiesj1.push_back(b1);
           
        }  
    
    }
 
//  SDL_RenderDrawPoints(renderer, points, 360);

	 
 parts  *partab(sprite *a,sprite *b){
  
        parts *c=new parts(xymr(a->x,b->x,a->w,b->w),xymr(a->y,b->y,a->h,b->h));
          
        c->vx+=(a->vx-b->vx)*30;
        c->vy+=(a->vy-b->vy)*30;
        return c;
    }
    void partsforiw(sprite *a, sprite *b){
        for (int i=1;i<30;++i) {
            parts *c=new parts(xymr(a->x,b->x,a->w,b->w),xymr(a->y,b->y,a->h,b->h));
          
            c->vx+=(a->vx+b->vx)*100;
            c->vy+=(a->vy+b->vy)*100;
            
            particules.push_back(c);
            }
        }
 void partsforired(sprite *a, sprite *b){
        for (int i=1;i<30;++i) {
            parts *c=new parts(xymr(a->x,b->x,a->w,b->w),xymr(a->y,b->y,a->h,b->h));
          
            c->vx+=(a->vx+b->vx)*100;
            c->vy+=(a->vy+b->vy)*100;
            
            particulesr.push_back(c);
            }
        }
        
    //test2210 join code opti xymr est dans la fablib

//ppoints[0]=0,0;
	//float factor = (float)M_PI / 180.0f;

	

void tirerj1(){
           if (tirj1<50)  {
                
            tirj1+=1;
            Spritej1->nrj+=1;
           bj2 *b2 = new bj2();
           b2->x=Spritej1->x;
           b2->y=Spritej1->y;
           b2->vx=0.5*(rand()%3-1);//(tx-Spritej1->y)/200;//ar1/ar4;//0.1*r1;//(r1/r3)*3;//c->v2dx(Spritej1->x,Spritej1->y,tx,ty);
           b2->vy=(60-Spritej1->nrj)*0.4;//ar2/ar4;//0.1*r2;//(r2/r3)*3;//c->v2dy(Spritej1->x
               if (Spritej1->nrj<1.5){b2->name="miss";Spritej1->nrj+=10;
           b2->vy*=0.01;b2->h*=2;b2->vx=0;
           tirjz=true;
           } else tirj1z=true;
           if (Spritej1->nrj>50) Spritej1->nrj=50;
            
           entitiesj2.push_back(b2);
        } 
    
    }
    
// impact réajustement pour x et y
int timerisrouage=0;

bool isrouage1=false;

	bool isrouageg(){
	   timerisrouage=2;
	isrouage1= true;    
	return isrouage1;
	    }

bool isrouage2=false;	
void resettimerrouages(){
    isrouage1=false;isrouage2=false;
    }	
	bool isrouaged(){
	    timerisrouage=2;
	    isrouage2=true;
  }
bool isrouages(){
    
    return isrouage1 and isrouage2; 
    
    }
void threadfps(double fps2)
{
    
    /////////
    
    //////////////
    runed=true;
    SDL_Renderer *renderer = NULL;
    SDL_Renderer *virtrend= NULL;
    
    
    
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
			runed=false;
	//Unit32 flgrend=SDL_RENDERER_ACCELERATED;
	SDL_Window* win= SDL_CreateWindow("", 
	SDL_WINDOWPOS_UNDEFINED, 
	SDL_WINDOWPOS_UNDEFINED, 
	WIDTH, HEIGHT, 0); 
    
    	
    //bool Exit = false;// m et mmesachat 
      Uint32 FullscreenFlag = 
      SDL_WINDOW_FULLSCREEN_DESKTOP;
      //SDL_WINDOW_FULLSCREEN; 
    
    //Uint32 FullscreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP;//SDL_WINDOW_FULLSCREEN; 
    bool IsFullscreen = SDL_GetWindowFlags(win) & FullscreenFlag; 
    SDL_SetWindowFullscreen(win, IsFullscreen ? 0 : FullscreenFlag); 
    SDL_ShowCursor(IsFullscreen);
    
	
	// create the window and renderer
	// note that the renderer is accelerated
	//win = SDL_CreateWindow("Image Loading", 100, 100, WIDTH, HEIGHT, 0);
	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    
    
    SDL_DisplayMode DM;
    SDL_GetDesktopDisplayMode(0,&DM);
    //SDL_GetCurrentDisplayMode(0, &DM);
    tw=DM.w*0.99;
    th= DM.h*0.979;
    setw(DM.w);seth(DM.h);
    setwh();setd1();
    envxyref=WIDTH*HEIGHT;
    WIDTH=tw*1.1;HEIGHT=th*1.1;
    envxy=WIDTH*HEIGHT;
    usedpoint=int(envxy/envxyref);
    
    SDL_Point particulespts[22];
    particulespts[0].x=0;
    particulespts[0].y=0;
    particulespts[1].x=1;
    particulespts[1].y=0;
     particulespts[2].x=1;
    particulespts[2].y=1;
    particulespts[3].x=2;
    particulespts[3].y=0;
    particulespts[4].x=2;
    particulespts[4].y=2;
    particulespts[5].x=0;
    particulespts[5].y=0;
     particulespts[6].x=0;
    particulespts[6].y=0;
    particulespts[7].x=0;
    particulespts[7].y=0;
    float s[20];
    for (int i=0;i<11;i++){
        s[i]=(50/k0)*i;
        
    }
    
    
    
    

    //tw=WIDTH;th=HEIGHT;
    SDL_Window* winvirt= SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_HIDDEN); 
    
    virtrend = SDL_CreateRenderer(winvirt, -1, SDL_RENDERER_ACCELERATED);
   
    
    
    //__________sounds
    //extern SDL_Window* win;

//Mix_Music *music = loadMusic("./resources/assets/sounds/music.ogg");

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
		//return 1;
	}
	//SDL_Window *window = SDL_CreateWindow("Hello SDL2-mixer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
	if (win == NULL)
	{
		fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
		//return 1;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	{
		fprintf(stderr, "Mix_OpenAudio Error: %s\n", Mix_GetError());
		//return 1;
	}
	
	
	Mix_AllocateChannels(10); // Allouer cannaux 
    int son=MIX_MAX_VOLUME/6;
    Mix_Volume(0, son); // Mets le son a 100% en volume pour le premier cannaux
    Mix_Volume(1, son); 
    Mix_Volume(2, son); // Mets le son a 100% en volume pour le premier cannaux
    Mix_Volume(3, son); 
    Mix_Volume(4, son); // Mets le son a 100% en volume pour le premier cannaux
    Mix_Volume(5, son); 
    Mix_Volume(6, son); // Mets le son a 100% en volume pour le premier cannaux
    Mix_Volume(7, son); 
    Mix_Volume(8, son); // Mets le son a 100% en volume pour le premier cannaux
    Mix_Volume(9, son); 
    Mix_Volume(10, son); // Mets le son a 100% en volume pour le premier cannaux
    
	Mix_Chunk *explo2;
	explo2= Mix_LoadWAV("./resources/assets/sounds/explo2.ogg");
	
	Mix_Chunk *explo1;
	explo1= Mix_LoadWAV("./resources/assets/sounds/explo1.ogg");
	
	
	Mix_Chunk *tir2;
		tir2= Mix_LoadWAV("./resources/assets/sounds/laser2.ogg");
	
	Mix_Chunk *tir1;
	tir1= Mix_LoadWAV("./resources/assets/sounds/laser.ogg");
	//Mix_PlayChannel(2, tir, 0);
	Mix_Chunk *entre= Mix_LoadWAV("./resources/assets/sounds/entre.wav");
	
	Mix_Music *music;
	music = Mix_LoadMUS("./resources/assets/sounds/music.ogg");
	
	Mix_Chunk *sndmiss ;
	sndmiss= Mix_LoadWAV("./resources/assets/sounds/miss2.mp3");

	//Mix_Music *music = loadMusic("./resources/assets/sounds/music.ogg");
	//Mix_Music *music = loadMusic("music.ogg");
	if (music == NULL)
	{
		fprintf(stderr, "Couldn't load music\n");
		//return 1;
	}
	if (Mix_PlayMusic(music, 1) == -1)
	{
		fprintf(stderr, "Couldn't play music\n");
		//return 1;
	}
	Mix_PlayMusic(music, -1);
	
//___________sounds
    
    SDL_Texture *img = NULL;
	
    img = IMG_LoadTexture(renderer, IMG_PATH);
	SDL_QueryTexture(img, NULL, NULL, &w, &h);
    SDL_Texture *img2 = NULL;
	img2 = IMG_LoadTexture(renderer, IMG_PATH2);
	SDL_QueryTexture(img2, NULL, NULL, &w, &h);
	SDL_Texture *imgfond = NULL;
	imgfond = IMG_LoadTexture(renderer, IMG_PATHfond);
	SDL_QueryTexture(imgfond, NULL, NULL, &w, &h);
	SDL_Texture *imgfond0 = NULL;
	imgfond0 = IMG_LoadTexture(renderer, IMG_PATHfond0);
	SDL_QueryTexture(imgfond0, NULL, NULL, &w, &h);
	SDL_Texture *imgfond00 = NULL;
	imgfond00 = IMG_LoadTexture(renderer, IMG_PATHfond00);
	SDL_QueryTexture(imgfond00, NULL, NULL, &w, &h);
    SDL_Texture *imgfond000 = NULL;
	imgfond000 = IMG_LoadTexture(renderer, IMG_PATHfond000);
	SDL_QueryTexture(imgfond000, NULL, NULL, &w, &h);
     SDL_Texture *imgfondast = NULL;
	imgfondast = IMG_LoadTexture(renderer, IMG_PATHfondast);
	SDL_QueryTexture(imgfondast, NULL, NULL, &w, &h);
	SDL_Texture *imgfondast2 = NULL;
	imgfondast2 = IMG_LoadTexture(renderer, IMG_PATHfondast2);
	SDL_QueryTexture(imgfondast2, NULL, NULL, &w, &h);
	SDL_Texture *imgfondast3 = NULL;
	imgfondast3 = IMG_LoadTexture(renderer, IMG_PATHfondast3);
	SDL_QueryTexture(imgfondast3, NULL, NULL, &w, &h);
	SDL_Texture *imgfondast4 = NULL;
	imgfondast4 = IMG_LoadTexture(renderer, IMG_PATHfondast4);
	SDL_QueryTexture(imgfondast4, NULL, NULL, &w, &h);
	SDL_Texture *imgfondast5 = NULL;
	imgfondast5 = IMG_LoadTexture(renderer, IMG_PATHfondast5);
	SDL_QueryTexture(imgfondast5, NULL, NULL, &w, &h);
    
    SDL_Texture *imgfondjup = NULL;
	imgfondjup = IMG_LoadTexture(renderer, IMG_PATHfondjup);
	SDL_QueryTexture(imgfondjup, NULL, NULL, &w, &h);
	
	SDL_Texture *imgexplo = NULL;
	imgexplo = IMG_LoadTexture(renderer, IMG_PATHexplo);
	SDL_QueryTexture(imgexplo, NULL, NULL, &w, &h);

	SDL_Texture *imgsrl = NULL;
	imgsrl = IMG_LoadTexture(renderer, IMG_PATHsrl);
	SDL_QueryTexture(imgsrl, NULL, NULL, &w, &h);
	
	SDL_Texture *imgsbl = NULL;
	imgsbl = IMG_LoadTexture(renderer, IMG_PATHsbl);
	SDL_QueryTexture(imgsbl, NULL, NULL, &w, &h);
	
	SDL_Texture *imggunrl = NULL;
	imggunrl = IMG_LoadTexture(renderer, IMG_PATHgunrl);
	SDL_QueryTexture(imggunrl, NULL, NULL, &w, &h);
	
	SDL_Texture *imggunbl = NULL;
	imggunbl = IMG_LoadTexture(renderer, IMG_PATHgunbl);
	SDL_QueryTexture(imggunbl, NULL, NULL, &w, &h);
	
	SDL_Texture *imgcoeurrl = NULL;
	imgcoeurrl = IMG_LoadTexture(renderer, IMG_PATHcoeurrl);
	SDL_QueryTexture(imgcoeurrl, NULL, NULL, &w, &h);
	
	SDL_Texture *imgcoeurbl = NULL;
	imgcoeurbl = IMG_LoadTexture(renderer, IMG_PATHcoeurbl);
	SDL_QueryTexture(imgcoeurbl, NULL, NULL, &w, &h);
	
	SDL_Texture *imgeclr = NULL;
	imgeclr = IMG_LoadTexture(renderer, IMG_PATHecl);
	SDL_QueryTexture(imgeclr, NULL, NULL, &w, &h);
	
	SDL_Texture *imgeclb = NULL;
	imgeclb = IMG_LoadTexture(renderer, IMG_PATHecl);
	SDL_QueryTexture(imgeclb, NULL, NULL, &w, &h);
	
	SDL_Texture *imgburn = NULL;
	imgburn = IMG_LoadTexture(renderer, IMG_PATHfeux);
	SDL_QueryTexture(imgburn, NULL, NULL, &w, &h);
	
	SDL_Texture *imgcont = IMG_LoadTexture(renderer, IMG_PATHcont);
	SDL_QueryTexture(imgcont, NULL, NULL, &w, &h);
	
	SDL_Texture *imgp = IMG_LoadTexture(renderer, IMG_PATHp);
	SDL_QueryTexture(imgp, NULL, NULL, &w, &h);

	SDL_Texture *imga1 = IMG_LoadTexture(renderer, IMG_PATHa1);
	SDL_QueryTexture(imga1, NULL, NULL, &w, &h);
	
	SDL_Texture *imga2 = IMG_LoadTexture(renderer, IMG_PATHa2);
	SDL_QueryTexture(imga2, NULL, NULL, &w, &h);

    SDL_Texture *imga3 = IMG_LoadTexture(renderer, IMG_PATHa3);
	SDL_QueryTexture(imga3, NULL, NULL, &w, &h);
	
	SDL_Texture *imga4 = IMG_LoadTexture(renderer, IMG_PATHa4);
	SDL_QueryTexture(imga4, NULL, NULL, &w, &h);
    
    SDL_Texture *imgpr = IMG_LoadTexture(renderer, IMG_PATHpr);
	SDL_QueryTexture(imgpr, NULL, NULL, &w, &h);
    
    SDL_Texture *imgpous = IMG_LoadTexture(renderer, IMG_PATHpous);
	SDL_QueryTexture(imgpous, NULL, NULL, &w, &h);
    
    SDL_Texture *imgtouch = IMG_LoadTexture(renderer, IMG_PATHtouch);
	SDL_QueryTexture(imgtouch, NULL, NULL, &w, &h);
    
    SDL_Texture *imglum = IMG_LoadTexture(renderer, IMG_PATHlum);
	SDL_QueryTexture(imglum, NULL, NULL, &w, &h);
  
    SDL_Texture *imgmiss = IMG_LoadTexture(renderer, IMG_PATHmiss);
	SDL_QueryTexture(imgmiss, NULL, NULL, &w, &h);
    
    SDL_Texture *imgtif = IMG_LoadTexture(virtrend, IMG_PATHtif);
	SDL_QueryTexture(imgtif, NULL, NULL, &w, &h);
    
		
	SDL_Texture *imgtj1=NULL;
	SDL_Texture *imgtj2=NULL;
	imgtj1= IMG_LoadTexture(renderer, IMG_PATHtj1);
	SDL_QueryTexture(imgtj1, NULL, NULL, &w, &h);
	imgtj2= IMG_LoadTexture(renderer, IMG_PATHtj2);
	SDL_QueryTexture(imgtj2, NULL, NULL, &w, &h);
    
    SDL_Texture *imgiaoo= IMG_LoadTexture(renderer, IMG_PATHiaoo);
	SDL_QueryTexture(imgiaoo, NULL, NULL, &w, &h);
	
	SDL_Texture *imgiaoo0= IMG_LoadTexture(renderer, IMG_PATHiaoo0);
	SDL_QueryTexture(imgiaoo0, NULL, NULL, &w, &h);
	
	SDL_Rect texiaoo={ 500, 500, 100, 100 };
    SDL_Texture *imgrouage= IMG_LoadTexture(renderer, IMG_PATHrouage);
	SDL_QueryTexture(imgrouage, NULL, NULL, &w, &h);
	

    SDL_Texture *imgcrabe= IMG_LoadTexture(renderer, IMG_PATHcrabe);
	SDL_QueryTexture(imgcrabe, NULL, NULL, &w, &h);
	SDL_Rect texcrabe={ 500, 500, 100, 100 };
	iago->xy.limxy.set(100,100,W-100,H-100);
	      sprite lockast= sprite(-100,500,1,1); //
	      
	      SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
//SDL_RenderSetLogicalSize(win, 640, 480);
	     //Uint8 state = SDL_APPACTIVE//SDL_GetAppState();sdl1?
	      //lockast creating
	 //{ x, y, mWidth, mHeight }
	 // get the width and height of 
	 // get the width and height of the texture
	// put the location where we want the texture to be drawn into a rectangle
	// I'm also scaling the texture 2x simply by setting the width and height
	//SDL_Rect texr;
	
	//SDL_Rect *texrtj1[100];
	//SDL_Rect *texrtj2[100];
		loosej2->pv=0;
		loosej1->pv=0;
        
	int r10=HEIGHT/30;
	
	for (int i=0;i<1;++i) {
	    SDL_Rect *j2 = new SDL_Rect;
	    SDL_Rect *j1 = new SDL_Rect;
	    j2->w=r10;
	    j2->h=r10;
	    j2->x=-1000;
	    j2->y=-1000;
	    j1->w=r10;
	    j1->h=r10;
	    j1->x=-1000;
	    j1->y=-1000;
	    
	    texrtj2.push_back(*j2);
	    texrtj1.push_back(*j2);
		}
		for (int i=0;i<1;++i) {
	    SDL_Rect *j2 = new SDL_Rect;
	    j2->w=1;
	    j2->h=1;
	    j2->x=-1000;
	    j2->y=-1000;
	    texrexplos.push_back(*j2);
		}
		for (int i=0;i<1;++i) {
	    SDL_Rect *j2 = new SDL_Rect;
	    j2->w=1;
	    j2->h=1;
	    j2->x=-1000;
	    j2->y=-1000;
	    texrburns.push_back(*j2);
		}
		for (int i=0;i<1;++i) {
	    SDL_Rect *j2 = new SDL_Rect;
	    j2->w=2;
	    j2->h=2;
	    texrp.push_back(*j2);
		}
		for (int i=0;i<1;++i) {
	    SDL_Rect *j2 = new SDL_Rect;
	    j2->w=100;
	    j2->h=100;
	    texra1.push_back(*j2);
		}
			for (int i=0;i<1;++i) {
	    SDL_Rect *j2 = new SDL_Rect;
	    j2->w=100;
	    j2->h=100;
	    texra2.push_back(*j2);
		}
	
	//SDL_Rect texrtj10[100];
	    //texrtj10(1)->x=-1000;
	    //texrtj2(i).y=-1000;
	    //texrtj2(i).w=r10;
	    //texrtj2(i).h=r10;
	       // | SDL_FLIP_VERTICAL;SDL_FLIP_NONE SDL_FLIP_HORIZONTAL
SDL_RendererFlip flip = SDL_FLIP_NONE;  
	SDL_Rect texrrouage1;
	SDL_Rect texrrouage2;
    SDL_Rect texreclr;
    SDL_Rect texreclb;
	SDL_Rect texrcoeurbl;
	SDL_Rect texrcoeurrl;
	SDL_Rect texrgunbl;
	SDL_Rect texrgunrl;
	SDL_Rect texrcont;
	SDL_Rect texriaoo;
	SDL_Rect texrpr;
	SDL_Rect texrpr2;
	SDL_Rect texrsbl;
	SDL_Rect texrsrl;
	SDL_Rect texr;
	SDL_Rect texrd;
	SDL_Rect texrfond;
	SDL_Rect texrfondast;
	SDL_Rect texrfondast2;
	SDL_Rect texrfondast3;
	SDL_Rect texrfondast4;
	SDL_Rect texrfondast5;
	SDL_Rect texrfondjup;
	//SDL_Rect textest;
	//SDL_Rect tvide;
	
	SDL_Rect texrfond0;
	SDL_Rect texrfond00;
	SDL_Rect texrfond000;
	
	texrfond0.x = 0; 
	texrfond0.y = 0;
	texrfond0.w=WIDTH;
	texrfond0.h=HEIGHT;
	
	texrfond00.x = WIDTH/2+WIDTH/3/2; 
	texrfond00.y = HEIGHT/3+WIDTH/3/2;
	texrfond00.w= WIDTH*10;
	texrfond00.h=WIDTH*3;
	texrfond000.x = WIDTH/2-WIDTH/3/2; 
	texrfond000.y = HEIGHT/2-WIDTH/3/2;
	texrfond000.w= WIDTH*3;
	texrfond000.h=WIDTH*3;
	Spritej2->x =WIDTH*0.70;
	Spritej2->y =HEIGHT*0.75;
	Spritej1->y=HEIGHT*0.15;
	Spritej1->x=WIDTH*0.25;
	//Spritesrl->x=100; 
	//Spritesrl->y=1000;
	
	//Spritesbl->x=800 ;
	//Spritesbl->y=1000;

r1=HEIGHT/2;r2=HEIGHT/3;
r4=HEIGHT;r3=WIDTH/2-WIDTH/3/2;
r5=HEIGHT/2+WIDTH/3/2;r6=HEIGHT/18;
Spritefond->set(r1,r1,r2,r2,1);
Spritefond2->setv(r6,r3,r1,r1,0,0,-2);
Spritefond000->setv(r6,r3,r1,r1,0,0,1);
Spritefondast2->setv(-1,-1,r4,r4,-1,-1,1);
Spritefondast3->setv(-1,-1,r4,r4,-1,-1,1);
Spritefondast4->setv(-1,-1,r4,r4,-1,-1,1);
Spritefondast5->setv(-1,-1,r4,r4,-1,-1,1);
setastswall();
Spritefondast->setv(-1,-1,r4,r4,-1,-1,1);
Spritefondjup->setv(r4,r4,r4,r4,0,0,-1);
    

rouage1->setv(WIDTH*0.75+r6/2 , HEIGHT/2.2-r6*0.8,r6,r6,0,0,-1);
rouage2->setv(WIDTH*0.05+r6/2,HEIGHT/2.1+r6*1.8,r6,r6,0,0,-1);
Spritesrl->setv(700,r3,r6,r6,0,0,-1);
Spritesbl->setv(300,r3,r6,r6,0,0,-1);
Spritecoeurrl->setv(700,r3,r6,r6,0,0,-1);
Spritecoeurbl->setv(300,r3,r6,r6,0,0,-1);
Spritegunrl->setv(700,r3,r6,r6,0,0,-1);
Spritegunbl->setv(300,r3,r6,r6,0,0,-1);
Spriteeclb->setv(700,r3,r6,r6,0,0,-1);
Spriteeclr->setv(300,r3,r6,r6,0,0,-1);
cont->vx=1;
cont->vy=1;
iaoo->vx=1;iaoo->name="cont";
iaoo->vy=1;iaoo->ctrl=false;
cont->ctrl=false;
cont->name="cont";
cont->pv=0;
float coeft=0.8;
Spritej1->w*=coeft;
Spritej1->h*=coeft;
Spritej2->w*=coeft;
Spritej2->h*=coeft;
Spritej1->sw*=coeft;
Spritej1->sh*=coeft;
Spritej2->sw*=coeft;
Spritej2->sh*=coeft;
ia00->w*=coeft;
ia00->h*=coeft;
ia00->sw*=coeft;
ia00->sh*=coeft;
ia4->w*=coeft;
ia4->h*=coeft;
ia4->sw*=coeft;
ia4->sh*=coeft;
int rnd1=rand()%360;//for good burns aspect
tupl j2lock;//projection of move for good aim
//test2210
//SDL_Point ppoints[36];
//for (int i=1;i<36;++i)
// {
	
//	ppoints[i].x=((i*10)*(0 + 3 * cos(i * rt)));
	
    //ppoints[i].y=((i*10)*(0 + 3* sin(i * rt)))};}
	
coeft=1.5;

enti *intrusionj2=new enti();
enti *intrusionj1=new enti();
ia1->w*=coeft;
ia1->h*=coeft;
ia1->sw*=coeft;
ia1->sh*=coeft;
ia0->w*=coeft;
ia0->h*=coeft;
ia0->sw*=coeft;
ia0->sh*=coeft;
coeft=3;
ia2->sw*=coeft;
ia2->sh*=coeft;
ia2->w*=coeft;
ia2->h*=coeft;
coeft=5;
ia3->w*=coeft;
ia3->h*=coeft;
ia3->sw*=coeft;
ia3->sh*=coeft;
iago1->xy.setst(tupl(Spritej2->x,Spritej2->y));

Suiveur->w=ia1->w;Suiveur->h=ia1->h;
Suiveur->sw=ia1->sw;Suiveur->sh=ia1->sh;
Suiveur->pv=0;
bool suiveurmajaf=true;

	//cont->updatetir();



Spritefond2->frame=119;
//Spritej1->name="j1";
//Spritej2->name="j2";

//entitiesj1.push_back(Spritej1);
//entitiesj2.push_back(Spritej2);

Spritefondjup->frame=800;
//iago->xy.x=Spritej1->x;
//iago->xy.y=Spritej1->y;

//SDL_SetHint(SDL_HINT_ORIENTATIONS, "Portrait");
iago1->xy.limxy=iago->xy.limxy;
	
    //for (int i=1;i<bcltests*4;++i) {
       
        // pour test
while(runed){
	//std::cout << "fps " <<"  " <<std::endl;
	while (tics2) usleep(intlatence*3);
	tics2=true;
	usleep(intlatence*3);
	//usleep(intmint*2-intlatence);
	//th2();
	//SDL_SetHint(SDL_HINT_ORIENTATIONS, "Portrait");
	
	//maj coo j1 et 2 pour calc
	intrusionj1->xy.setxy(Spritej1->x
	,Spritej1->y);	
	intrusionj2->xy.setxy(Spritej2->x+Spritej2->vx*10
	,Spritej2->y+Spritej2->vy*10
	);
//	defence j1 j2 debut
{
	// defence j1 start
if ((Spritej2->pv>0)
and (Spritej1->pv>0)){
	if (Spritej2->y<0.5*H
	and
	(rand()%100>90)){
		Mix_PlayChannel(2, tir1, 0);
	bj2 *b2= new bj2();
	b2->x=0;
	b2->y=H/2;
	intrusionj2->azimm(0,H/2,-0.05);
	b2->vx=intrusionj2->v.vx;b2->vy=intrusionj2->v.vy;
	entitiesj2.push_back(b2);}
		if (Spritej2->y<0.5*H
	and
	(rand()%100>90)){
		Mix_PlayChannel(2, tir1, 0);
	bj2 *b2= new bj2();
	b2->x=W;
	b2->y=0;
	intrusionj2->azimm(W,0,-0.05);
	b2->vx=intrusionj2->v.vx;b2->vy=intrusionj2->v.vy;
	entitiesj2.push_back(b2);}
	
	if (Spritej2->y<0.5*H
	and
	(rand()%100>90)){
		Mix_PlayChannel(2, tir1, 0);
	bj2 *b2= new bj2();
	b2->x=0;
	b2->y=0;
	intrusionj2->azimm(0,0,-0.05);
	b2->vx=intrusionj2->v.vx;b2->vy=intrusionj2->v.vy;
	entitiesj2.push_back(b2);}
//	}
	// defence j1 end
	
	//>>>>>>copiej1 defence j1 j2<<<<<<
	
	// defence j2 start
	{
	
		if (Spritej1->y>0.5*H
	and
	(rand()%100>90)){
		Mix_PlayChannel(2, tir2, 0);
	bj1 *b1= new bj1();
	b1->x=W;
	b1->y=H/2;
	intrusionj1->azimm(W,H/2,-0.05);
	b1->vx=intrusionj1->v.vx;b1->vy=intrusionj1->v.vy;
	entitiesj1.push_back(b1);}
		if (Spritej1->y>0.5*H
	and
	(rand()%100>90)){
		Mix_PlayChannel(2, tir2, 0);
	bj1 *b1= new bj1();
	b1->x=0;
	b1->y=H;
	intrusionj1->azimm(0,H,-0.05);
	b1->vx=intrusionj1->v.vx;b1->vy=intrusionj1->v.vy;
	entitiesj1.push_back(b1);}
	if (Spritej1->y>0.5*H
	and
	(rand()%100>90)){
		Mix_PlayChannel(2, tir2, 0);
	bj1 *b1= new bj1();
	b1->x=W;
	b1->y=H;
	intrusionj1->azimm(W,H,-0.05);
	b1->vx=intrusionj1->v.vx;b1->vy=intrusionj1->v.vy;
	entitiesj1.push_back(b1);}
	}
	// defence j2 end
}
}
//	defence j1j2fin
	
	for(auto i=burnsj2.begin();i!=burnsj2.end();)
    {
      sprite *e = *i;
      e->updatetir();
      
      
                
      if (loosej1->pv==0 and loosej2->pv==0) {i=burnsj2.erase(i); delete e;
      }
      else i++;
    }
    for(auto i=burnsj1.begin();i!=burnsj1.end();)
    {
      sprite *e = *i;
      e->updatetir();
                
      if (loosej1->pv==0 and loosej2->pv==0) {i=burnsj1.erase(i); 
      delete e;
      }
      else i++;
    }
	//Spritej1->nrj=0;//testnrj
	//Spritej2->nrj=0;
	
	for(auto i=entitiesj1.begin();i!=entitiesj1.end();)
    {
      sprite *e = *i;
      e->updatetir();
      
       if (colee(Spritej1,e)) {
          partsforired(e,Spritej1);
         // for (int i=1;i<50;++i) {
	//	parts *ts = new parts(xymr(e->x,Spritej1->x,e->w,Spritej1->w),xymr(e->y,Spritej1->y,e->h,Spritej1->h));
       //   ts->vx+=(Spritej1->vx-e->vx)*-30;//test2210
	//	ts->vy+=(Spritej1->vy-e->vy)*-30;
	//	ts=partab(e,Spritej1);
       //  particulesr.push_back(ts);
       // }
              if (e->name=="miss") {
        Spritej1->pv-=misspvminus(Spritej1);
        Spritej1->nrj+=missnrjminus(e);
        }
              else{
          Spritej1->pv-=pvminus(Spritej1);
          Spritej1->nrj+=nrjminus(e);
              }
          
    
          Mix_PlayChannel(3, explo2, 0);
          e->pv=0;
          
          eexpl *e1=new eexpl("j",xymr(e->x,Spritej1->x,e->w,Spritej1->w),xymr(e->y,Spritej1->y,e->h,Spritej1->h),
          HEIGHT/40,60,
          Spritej1->vx*0.1,Spritej1->vy*0.1);
           
           //sprite *b1 = new sprite();
           //b1->setexplo("j1",e->x,e->y,HEIGHT/40,60); 
           explos.push_back(e1);           
      } 
      if (e->pv==0) {i=entitiesj1.erase(i); delete e;
      tirj2-=1;//delete b1;
      }
      else i++;
    }
    //tirs
    for(auto i=entitiesj2.begin();i!=entitiesj2.end();)
    {
      
      sprite *e = *i;
      
      e->updatetir();
      if (colee(Spritej2,e)) {//test2210
      partsforired(e,Spritej2);
        //  for (int i=1;i<50;++i) {
	//	parts *ts = new parts(xymr(
	//	e->x,Spritej2->x,e->w,Spritej2->w),xymr(
	//	e->y,Spritej2->y,e->h,Spritej2->h));
	//	ts->vx+=e->vx*30;
	//	ts->vy+=e->vy*30;
	//	ts=partab(e,Spritej2);
        //   particulesr.push_back(ts);
           //}
           if (e->name=="miss"){ 
           Spritej2->pv-=misspvminus(Spritej2);
          Spritej2->nrj+=missnrjminus(e);
          }       
     else{
          Spritej2->pv-=pvminus(Spritej2);
          Spritej2->nrj+=nrjminus(e);
     }
          Mix_PlayChannel(4, explo2, 0);
          e->pv=0;
          eexpl *e1=new eexpl("j",xymr(e->x,Spritej2->x,e->w,Spritej2->w),xymr(e->y,Spritej2->y,e->h,Spritej2->h), HEIGHT/40
         ,60,Spritej2->vx*0.1,Spritej2->vy*0.1);
           
           //sprite *b2 = new sprite();
           //b2->setexplo("j2",e->x,e->y,HEIGHT/40,60);
           explos.push_back(e1);
      }
    
      if (e->pv==0) {i=entitiesj2.erase(i); delete e;
      tirj1-=1; }
      else i++;
    }
      //<tirs 
 
      
      //<col ast
      
      lockast.setxywh(-100,-100,1,1);    
      j2lock.setxy(Spritej2->x+Spritej2->vx*15
      , Spritej1->y);
      
      //tierer sur le j2
      if ((not setgui) and setia){
      if ((fa(Spritej1->x-j2lock.x)<W/5)){
      if (rand()%(50*int(Spritej1->nrj
      ))<10) tirerj1();
      for (int i=0;i<6;i++){
      parts *ts = new parts(
	        Suiveur->x
	        ,
	        Suiveur->y
	        );
	        
		
         //  particules.push_back(ts); // visualiser suiveur
      }}else if (Spritej1->nrj<1) tirerj1();
      ia3->x=Spritej1->x;
      ia3->y=Spritej1->y;
      iago->update();
      iago->azimm(W/2,H/5,0.5);
      iago->azimm(j2lock.x,j2lock.y,3);
      ia0->x=iago->xy.x;
      ia0->y=iago->xy.y;
      ia00->x=iago->xy.x+cos(iago->v.angle*rt)*ia00->w*0.5;
      ia00->y=iago->xy.y+cos(iago->v.angle*rt)*ia00->w*0.5;
      //if (colee(Spritej2,ia0) or colee(Spritej2,ia00)){
      //     lockast.setxywh(Spritej2->x,Spritej2->y,Spritej2->w/10,Spritej2->h/10); 

       //   } //ia0 fuite endif
       iago->v.setvi();}
       //tirer sir  j2
//lockast.setxywh(-100,-100,1,1);// boucle astéroides 
if ((incra1)<8) setasts(2);//anc 40

//ast1308 start
 for(auto i=sa1.begin();i!=sa1.end();)
    {
      
      float cm=HEIGHT/1000;
      sprite *e = *i;
       if (inab(e->y,Spritej1->y,H*0.7)
       and inab(e->x,0,W)
       and e->vy>0 
       and inab(fa(e->vx),fa(e->vy)*0.2,fa(e->vy)*0.9)){
          iacalc->v.setv(tuplv(e->vx,e->vy));
          iacalc->v.setvi();
          
          //if (iacalc->v.angle>181 and
          //iacalc->v.angle<359 ) {
              //tirer minage
              if ((not setgui) and setia){
              if (rand()%(10*int(Spritej1->nrj))<10) tirerj1();}
              iago->azimm(e->x+e->vx,e->y-Spritej1->h*0.6,5);
              //ia00->x=iago->xy.x;//diminuer la detection
              //ia00->y=iago->xy.y;
              ia0->x=iago->xy.x;//diminuer la detection
              ia0->y=iago->xy.y-Spritej1->h*0.5;
         
          // }
              } //ia1 minage
      
      // suiveurtest si detect par lock alors suiveur création
      if (((colee(e,ia00) or colee(e,ia0)) and e->y<H/2 and !suiveur() and e->vy>0) ){
          lockast.setxywh(e->x,e->y,e->w,e->h);
          lockast.vx=e->vx;
          lockast.vy=e->vy;
          Suiveur->pv=1;
          iago->xy=tupl(e->x,e->y-ia0->h);
          } //ia0 fuite endif
          if (suiveur() and suiveurmajaf){
          	{ 
          	if (Suiveur->y>H/4){
          	Suiveur->y=H/4;
          	Suiveur->pv=0;suiveurmajaf=false;
          	}
          	
          	
          	if (colee(e,Suiveur) and suiveurmajaf){
          	
          	Suiveur->setxywh(e->x,e->y,e->w,e->h);
          	
          	}
          //	lockast.vx=0;lockast.vy=0;lockast.x=W*0.5;lockast.y=H*0.25;
          }
          	
          	
          	
          	
          	}
 //   }
          
          
      e->ast();
       if (colee(Spritej1,e)) {
         partsforired(e,Spritej1);
         // for (int i=1;i<50;++i) {
	//	parts *ts = new parts(xymr(e->x,Spritej1->x,e->w,Spritej1->w),xymr(e->y,Spritej1->y,e->h,Spritej1->h));
           //test2210
        //   ts->vx+=e->vx*30;
	//	ts->vy+=e->vy*30;
          // particulesr.push_back(ts);
           //}
          Spritej1->pv-=Spritej1->nrj*Spritej1->nrj*e->w*e->h*0.00002;
          Mix_PlayChannel(3, entre, 0);
          Spritej1->nrj+=e->w*e->h*0.0001;
          e->pv=0;
          eexpl *e1=new eexpl("pous",xymr(e->x,Spritej1->x,e->w,Spritej1->w),xymr(e->y,Spritej1->y,e->h,Spritej1->h),
          HEIGHT/40, (e->h+e->w)/4,
          (e->vx+Spritej1->vx)*0.1,
          (e->vy+Spritej1->vy)*0.1);
           
           //sprite *b1 = new sprite();
           //b1->name="pous";
           //b1->setexplo("pous",e->x,e->y,HEIGHT/40,90);
           explos.push_back(e1);
      }
             if (colee(Spritej2,e)) {
                 partsforired(e,Spritej2);
        // for (int i=1;i<50;++i) {
	//	parts *ts = new parts(xymr(e->x,Spritej2->x,e->w,Spritej2->w),xymr(e->y,Spritej2->y,e->h,Spritej2->h));
           //test2210
      //  ts->vx+=e->vx*30;
		//ts->vy+=e->vy*30;
          // particulesr.push_back(ts);
           //}
          Spritej2->pv-=Spritej2->nrj*Spritej2->nrj*e->w*e->h*0.00002;
          
          Mix_PlayChannel(3, entre, 0);
          Spritej2->nrj+=e->w*e->h*0.0001;
          e->pv=0;
          eexpl *e1=new eexpl("pous",xymr(e->x,Spritej2->x,e->w,Spritej2->w),xymr(e->y,Spritej2->y,e->h,Spritej2->h),HEIGHT/40,
          (e->h+e->w)/4,Spritej2->vx*0.1,Spritej2->vy*0.1);
           
           //sprite *b1 = new sprite();
           //b1->setexplo("pous",e->x,e->y,HEIGHT/40,90);
           explos.push_back(e1);
      }
      
	 for(auto f:sa1){
	     if (not inxy(f) ){
	         f->tout+=1;if (f->tout>20000) f->pv=0;
	         if ((incra1)>200/k0) f->pv=0;
	         } else f->tout=0;
	 }
                //>col ast
      for(auto i:sa1)
    {
      sprite *e = i;
      for(auto ii:sa1)
    {
      sprite *ee = ii;
      //e->ast();
       if (inxy(e) and colee(ee,e) 
       and (ee->incr!=e->incr) 
       and e->pv!=0 and ee->pv!=0 and
       e->timer==0 and ee->timer==0) {
          partsforiw(e,ee);
         // for (int i=1;i<50;++i) {
		//parts *ts = new parts(xymr(e->x,ee->x,e->w,ee->w),xymr(e->y,ee->y,e->h,ee->h));
        //   ts->vx+=e->vx*30;//test2210
	//	ts->vy+=e->vy*30;
          // particules.push_back(ts);
           //}
          Mix_PlayChannel(5, entre, 0);
          //std::cout << "lunch";
          
          eexpl *e1=new eexpl("pous",xymr(e->x,ee->x,e->w,ee->w),xymr(e->y,ee->y,e->h,ee->h),(e->h+e->w+ee->w+ee->h)/2,
          (e->h+e->w+ee->w+ee->h)/8,
          (e->vx+e->vx)*0.1,(ee->vy+ee->vy)*0.1);//anc90
          
           //sprite *b1 = new sprite();
           //b1->setexplo("pous",e->x,e->y,HEIGHT/40,90);
           explos.push_back(e1);
           //if ((sqrt(e->h*e->w)+sqrt(ee->h*ee->w)/
          // sqrt(sqrt (e->h*e->w)-sqrt (ee->h*ee->w)*sqrt (e->h*e->w)-sqrt (ee->h*ee->w))
          // )<3){
              float h1=sqrt (e->h*e->w);
              float h2=sqrt (ee->h*ee->w);
              if(h1==h2 or
              (h1<h2 and h2/h1<1.05) or
              (h1>h2 and h1/h2<1.05)){
          eclats(e,ee);eclats(e,ee);eclats(e,ee);eclats(e,ee);
          e->pv=0;
          ee->pv=0;
           } else if (h1>h2) {
               ee->pv=0;e->w+=ee->w*0.01;e->h+=0.01*ee->h;
               } 
                       else{
                            ee->w+=e->w*0.01;ee->h+=0.01*e->h;
                            e->pv=0;
                       }

      }
      
       //ii++;
    }//i++;
    }
    // vol ast
      for (auto p:entitiesj1){
        
        if (colee(p,e)) {  
        partsforiw(e,p);
        
      //  for (int i=1;i<50;++i) {
	//	parts *ts = new parts(xymr(e->x,p->x,e->w,p->w),xymr(e->y,p->y,e->h,p->h));
      //    ts->vx+=e->vx*30;
	//	ts->vy+=e->vy*30; 
         //  particules.push_back(ts); 
           // }//test2210
        Mix_PlayChannel(3, explo2, 0);
         e->pv=1;p->pv=0;e->w-=cm;e->h-=cm;
           //sprite *b1 = new sprite();
           //b1->setexplo("tj1",e->x,e->y,HEIGHT/40,60);
        eexpl *e1=new eexpl("pous",xymr(e->x,p->x,e->w,p->w),xymr(e->y,p->y,e->w,p->h),HEIGHT/40,
        (e->h+e->w)/4,(e->vx+p->vx)*0.1,(e->vy+p->vy)*0.1);
           
           explos.push_back(e1);
        }}
        for (auto p:entitiesj2){
        if (colee(p,e)) {  
        //test2210
        partsforiw(e,p);
      //  for (int i=1;i<50;++i) {
		//parts *ts = new parts(xymr(
	//	e->x,p->x,e->w,p->w),xymr(e->y,p->y,e->h,p->h));
        //  ts->vx+=e->vx*30;
	//	ts->vy+=e->vy*30;
		//vectpartabc(&e,&p,&ts);
         //   particules.push_back(ts);//test2210
           
          // }
        Mix_PlayChannel(3, explo2, 0);
         e->pv=1;p->pv=0;e->w-=cm;e->h-=cm;
           //sprite *b1 = new sprite();
           //b1->setexplo("pous",e->x,e->y,HEIGHT/40,90);
           eexpl *e1=new eexpl("pous",xymr(e->x,p->x,e->w,p->w),xymr(e->y,p->y,e->h,p->h),HEIGHT/40,
           (e->h+e->w)/4,
           (e->vx+p->vx)*0.1,(e->vy+p->vy)*0.1);
           
           explos.push_back(e1);
        }}
        if (e->w<HEIGHT/100) e->pv=0;
          
      if (e->pv==0) {i=sa1.erase(i); delete e;
      incra1-=1;
      }
      else i++;
    }
    
if ((not setgui) and setia){

    if (lockast.x!=-100){//si detection d astéroide proche
         iago->azimm(lockast.x,lockast.y,D1*0.01);
      iago->v.invpi();iago->v.inv();iago->v.setvi();
      iago->v.force*=sqrt(lockast.w*lockast.h)*0.0001;
      iago->v.setai();
      iacalc->v.vx=lockast.vx;
      iacalc->v.vx=lockast.vx;iacalc->v.setvi();
      float difangle=fa(iacalc->v.angle-iago->v.angle);
      if(difangle<20){
          difangle=20;
          iago->v.force=iago->v.force*(difangle);
          }
      else difangle =0;
      if (iacalc->v.angle>iago->v.angle){
          iago->v.angle+=difangle;
          //iago->v.force=iago->v.force*(difangle);
          iago->v.setai();  
              }else{
                  iago->v.angle-=difangle;
                  iago->v.force=iago->v.force*(45-difangle);
                  iago->v.setai();  
              }
          if ( ((iago->xy.x<W*0.2) or (iago->xy.x>W*0.8)) and iago->xy.y<H*0.3)
           {
              if (fa(lockast.y-iago->xy.y)<Spritej1->w/1.8)
              iago->xy.y=0.3*H;else
              iago->xy.x=0.5*W;
          }
         if (iago->xy.y>H*0.3){
             if (fa(lockast.x-iago->xy.x)<Spritej1->w/1.8)
              iago->xy.x=0.5*W;else
              iago->xy.y=0.3*H;
             
               
         }
              
        if (iago->v.angle>360)iago->v.angle-=360;
      if (iago->v.angle<0)iago->v.angle+=360;
      }

    iago1->xy.limxy=iago->xy.limxy;
    
    iago1->xy.setxy(Spritej1->x,Spritej1->y);
    iago1->update();
    iago1->azimm(iago->xy.x,iago->xy.y,59);
    //ia s arette avant les tourelles
    if (iago->xy.y>H/2.2){
    	iago->xy.y=H/2.2;
    	
    	}
    Spritej1->x=iago->xy.x;
    Spritej1->y=iago->xy.y;
}
    
    //parts
    //delete lockast;///ialockast delete
    for(auto i=particulesr.begin();i!=particulesr.end();){
          parts *e=*i;
          
      if (particulesr.size()>1000/k0) e->pv=0;    
      if (e->pv==0) {
          particulesr.erase(i);delete e; 
       }
      i++;
    }
     for(auto i=particules.begin();i!=particules.end();){
          parts *e=*i;
         if (particules.size()>1000/k0) e->pv=0;      
      if (e->pv==0) {
          particules.erase(i);delete e; 
       }
      i++;
    }
    for (auto ps:particules) {
            if (((ps->x - Spritej2->x)*(ps->x - Spritej2->x)+
         (ps->y - Spritej2->y)*(ps->y - Spritej2->y))<
         (( Spritej2->h*0.5)/1*(Spritej2->h*0.5)/1)/1) {
              
                Spritej2->nrj*=0.98;Spritej2->pv+=
                (ps->pv*k0/600)*0.3;
            ps->pv=0;
        }
               if (((ps->x - Spritej1->x)*(ps->x - Spritej1->x)+
         (ps->y - Spritej1->y)*(ps->y - Spritej1->y))<
         (( Spritej1->h*0.5)/1*(Spritej1->h*0.5)/1)/1) {
                
                Spritej1->nrj*=0.98;Spritej1->pv+=
                (ps->pv*k0/600)*0.3;
            ps->pv=0;
        }
    }

    
    //parts
    rnd1=rand()%360;
    for(auto f:burnsj2){
	    //if  (cos(angletest)>0.1){
	        for (int i=0;i<1;i++){
	        parts *ts = new parts(
	        Spritej2->x+cos(angletest+rnd1)*0.5*Spritej2->mx
	        ,
	        Spritej2->y+sin(angletest+rnd1)*0.5*Spritej2->my
	        );
           particulesr.push_back(ts);
	        }
	 // }
	    }
	    for(auto f:burnsj1){
	    //if  (cos(angletest)>0.1){
	        for (int i=0;i<1;i++){
	        parts *ts = new parts(
	        Spritej1->x+cos(angletest+rnd1)*0.5*Spritej1->mx
	        ,
	        Spritej1->y+sin(angletest+rnd1)*0.5*Spritej1->my
	        );
           particulesr.push_back(ts);
	        }
	 // }
	    }
    //<a1 a2 a3 a4

 
    
    if (not c->inlng(cont->x,WIDTH/2,0.9*WIDTH/2)){
	   cont->vx*=-1;}
    if (not c->inlng(cont->y,HEIGHT/2,0.9*HEIGHT/2)){
        cont->vy*=-1;}
     if (not c->inlng(iaoo->x,WIDTH/2,0.9*WIDTH/2)){
	   iaoo->vx*=-1;}
    if (not c->inlng(iaoo->y,HEIGHT/2,0.9*HEIGHT/2)){
        iaoo->vy*=-1;}

     if (explo1z) {Mix_PlayChannel(2, explo1, 0);
     explo1z=false;}
     if (explo2z) {Mix_PlayChannel(2, explo2, 0);
     explo2z=false;}
     if (tirj1z) {
         Mix_PlayChannel(2, tir1, 0);
     tirj1z=false;}
     if (tirj2z) {Mix_PlayChannel(2, tir2, 0);
     tirj2z=false;}
          if (tirjz) {
         Mix_PlayChannel(2, sndmiss, 0);
     tirjz=false;}
     
	Spritej1->unctrl();
	iaentre->x=Spritej1->x;iaentre->y=Spritej1->y;
	iaentre->w=Spritej1->w;iaentre->h=Spritej1->h;
	iaentre->y+=iaentre->h/2;
	ia1->unctrl();
	
	ia2->unctrl();
	
	Spritej2->unctrl();
	
	
	
	texrcoeurrl.x = WIDTH*(0.05+(1000-Spritej1->pv)*0.00005); 
	texrcoeurrl.y =HEIGHT*(1/2.8+(1000-Spritej1->pv)*0.00005);
	texrcoeurrl.w= Spritecoeurrl->w;
	texrcoeurrl.h=Spritecoeurrl->h;
	//coeurs
	texrcoeurbl.x = WIDTH*(0.75+(1000-Spritej2->pv)*0.00005); 
	texrcoeurbl.y = HEIGHT*(1/1.7+(1000-Spritej2->pv)*0.00005);
	texrcoeurbl.w= Spritecoeurbl->w;
	texrcoeurbl.h=Spritecoeurbl->h;
	
	texrgunrl.x = WIDTH*0.05; 
	texrgunrl.y =HEIGHT/2.4;
	texrgunrl.w= Spritegunrl->w;
	texrgunrl.h=Spritegunrl->h;
	//guns
	texrgunbl.x = WIDTH*0.75; 
	texrgunbl.y = HEIGHT/1.9;
	texrgunbl.w= Spritegunbl->w;
	texrgunbl.h=Spritegunbl->h;
		
	texrsrl.x = WIDTH*0.75; 
	texrsrl.y =HEIGHT/2.2;
	texrsrl.w= Spritesrl->w;
	texrsrl.h=Spritesrl->h;
	//shields	
	texrsbl.x = WIDTH*0.05; 
	texrsbl.y = HEIGHT/2.1;
	texrsbl.w= Spritesbl->w;
	texrsbl.h=Spritesbl->h;
	
	texreclr.x = WIDTH*0.05; 
	texreclr.y =HEIGHT*(1/2.2-Spritej1->nrj*0.0009);
	texreclr.w= Spriteeclr->w;
	texreclr.h=Spriteeclr->h;
	//ecl
	texreclb.x = WIDTH*0.75; 
	texreclb.y = HEIGHT*(1/2.1+Spritej2->nrj*0.0009);
	texreclb.w= Spriteeclb->w;
	texreclb.h=Spriteeclb->h;
	
	rouage1->updatetir();
	rouage2->updatetir();
	texrrouage1.x = rouage1->xm;
	texrrouage1.y =rouage1->ym;
	texrrouage1.w= rouage1->w;
	texrrouage1.h=rouage1->h;
	//rouages 1et 2
	texrrouage2.x = rouage2->xm;
	texrrouage2.y =rouage2->ym;
	texrrouage2.w= rouage2->w;
	texrrouage2.h=rouage2->h;
	
	texr.x = Spritej1->xm; 
	texr.y = Spritej1->ym;
	texr.w= Spritej1->w;
	texr.h=Spritej1->h;
	
	texrd.x = Spritej2->xm; 
	texrd.y = Spritej2->ym;
	texrd.w= Spritej2->w;
	texrd.h=Spritej2->h;
	
	texrfond.x = Spritefond->xm; 
	texrfond.y = Spritefond->ym;
	texrfond.w= Spritefond->w;
	texrfond.h=Spritefond->h;
	
	texrfondast.x = Spritefondast->xm; 
	texrfondast.y = Spritefondast->ym;
	texrfondast.w= Spritefondast->w;
	texrfondast.h=Spritefondast->h;
	
	texrfondast2.x = Spritefondast2->xm; 
	texrfondast2.y = Spritefondast2->ym;
	texrfondast2.w= Spritefondast2->w;
	texrfondast2.h=Spritefondast2->h;
	
	texrfondast3.x = Spritefondast3->xm; 
	texrfondast3.y = Spritefondast3->ym;
	texrfondast3.w= Spritefondast3->w;
	texrfondast3.h=Spritefondast3->h;
	
	texrfondast4.x = Spritefondast4->xm; 
	texrfondast4.y = Spritefondast4->ym;
	texrfondast4.w= Spritefondast4->w;
	texrfondast4.h=Spritefondast4->h;
	
	texrcont.x = cont->xm; 
	texrcont.y = cont->ym;
	texrcont.w= cont->w;
	texrcont.h=cont->h;
	
	texriaoo.x=iaoo->xm;
	texriaoo.y=iaoo->ym;
	texriaoo.w=iaoo->w;
	texriaoo.h=iaoo->h;
	
	texrpr.x = ia1->xm; 
	texrpr.y = ia1->ym;
	texrpr.w= ia1->w;
	texrpr.h=ia1->h;
	texrpr2.x = ia2->xm; 
	texrpr2.y = ia2->ym;
	texrpr2.w= ia2->w;
	texrpr2.h=ia2->h;
	//iago.xy.x=500;iago.xy.y=500;
	//test void ok sdlxy(iago,&texcrabe);
	sdlxy(iago,&texcrabe);
	//texcrabe.x = ia2->x;
	 //texcrabe.y =ia2->y,
	
	
	texrfondast5.x = Spritefondast5->xm; 
	texrfondast5.y = Spritefondast5->ym;
	texrfondast5.w= Spritefondast5->w;
	texrfondast5.h=Spritefondast5->h;
	
	texrfond00.x = Spritefond000->xm; 
	texrfond00.y = Spritefond000->ym;
	texrfond00.w= Spritefond000->w;
	texrfond00.h=Spritefond000->h;
	texrfond000.x = Spritefond2->xm*1; 
	texrfond000.y = Spritefond2->ym*1;
	texrfond000.w= Spritefond2->w*1;
	texrfond000.h=Spritefond2->h*1;
	
	texrfondjup.x = Spritefondjup->xm*1; 
	texrfondjup.y = Spritefondjup->ym*1;
	texrfondjup.w= Spritefondjup->w*1;
	texrfondjup.h=Spritefondjup->h*1;
	
		 
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	
	    
	    
		// copy the texture to the rendering context
		
		    SDL_RenderCopy(renderer, imgfond0, NULL, &texrfond0);
		    SDL_RenderCopy(renderer, imgfond00, NULL, &texrfond00);
		    SDL_RenderCopy(renderer, imgfond000, NULL, &texrfond000);
		    //SDL_RenderCopy(renderer, imgfondast, NULL, &texrfondast);
		    //SDL_RenderCopy(renderer, imgfondast2, NULL, &texrfondast2);
		    //SDL_RenderCopy(renderer, imgfondast3, NULL, &texrfondast3);
		    //SDL_RenderCopy(renderer, imgfondast4, NULL, &texrfondast4);
		    //SDL_RenderCopy(renderer, imgfondast5, NULL, &texrfondast5);
		    SDL_RenderCopy(renderer, imgfond, NULL, &texrfond);
			SDL_RenderCopy(renderer, imgfondjup, NULL, &texrfondjup);
			//SDL_RenderCopy(renderer, img, NULL, &texr);

//SDL_Rect r=texr;
    angletest+=0.1*k0;if (angletest>1.7E+308) angletest=0;
    //aff j1 statt
    {
     SDL_RenderCopyEx(renderer, img,NULL,&texr,angletest,NULL,flip);
     
       for(auto e:texrburns)
		for(auto f:burnsj1){
		e.x=Spritej1->x-f->w/1.4;
		e.y=Spritej1->y-f->h/1.4;;
		e.w=f->w*1.5;
		e.h=f->h*1.5; 
	
		SDL_RenderCopy(renderer, imgburn, NULL, &e);
	    }
    }
  // aff  j1 end
	{
	// aff j2 start
		SDL_RenderCopyEx(renderer, img2,NULL,&texrd,angletest*-1,NULL,flip);
		for(auto e:texrburns)
		for(auto f:burnsj2){
		e.x=Spritej2->x-f->w/1.4;
		e.y=Spritej2->y-f->h/1.4;
		e.w=f->w*1.5;
		e.h=f->h*1.5; 
	
	SDL_RenderCopy(renderer, imgburn, NULL, &e);
	    }
	}
// aff j2 end
            //SDL_RenderCopy(renderer, img2, NULL, &texrd);
            SDL_RenderCopy(renderer, imgsrl, NULL, &texrsrl);
          
            SDL_RenderCopy(renderer, imgsbl, NULL, &texrsbl);
          SDL_RenderCopy(renderer, imgrouage, NULL, &texrrouage1);
            SDL_RenderCopy(renderer, imgrouage, NULL, &texrrouage2);

	

	    


for(auto e:texrtj1)
	for(auto f:entitiesj1){
	e.x=f->xm;
	e.y=f->ym;
	e.w=f->w;
	e.h=f->h; 
	if (f->name=="miss") {e.h*=2;e.w*=0.5;//b
//	Spritej2->nrj=0;
	f->vx=(f->x-Spritej1->x)*-0.1;
	f->vy*=1.15;f->y+=8*k0*-1;
	SDL_RenderCopyEx(renderer, imgmiss,NULL,&e,360-f->vx*-2,NULL,flip);
	for (int i=0;i<15;i++){
	        parts *ts = new parts(
	        f->xm+f->mx/2
	        ,
	        f->y+f->my*3
	        );
           particulesr.push_back(ts);
	}
	}
    else
	SDL_RenderCopy(renderer, imgtj1, NULL, &e);
	    }
	    
	for(auto e:texrtj2)
	for(auto f:entitiesj2){
	e.x=f->xm;
	e.y=f->ym;
	e.w=f->w;
	e.h=f->h; 
	if (f->name=="miss") {e.h*=2;e.w*=0.5;//h
//	Spritej1->nrj=0;
		f->vx=(f->x-Spritej2->x)*-0.1;
			f->vy*=1.15;f->y+=8*k0;
	SDL_RenderCopyEx(renderer, imgmiss,NULL,&e,180-f->vx*2,NULL,flip);
	for (int i=0;i<15;i++){
	        parts *ts = new parts(
	        f->xm+f->mx/2
	        ,
	        f->y-f->my*1.5
	        );
           particulesr.push_back(ts);
	}
	}
	else SDL_RenderCopy(renderer, imgtj2, NULL, &e);
	    }
	    
   for(auto e:texra1)
	for(auto f:sa1){
	    	
    {
	e.x=f->xm;
	e.y=f->ym;
	e.w=f->w;
	e.h=f->h; 
	if (inxy(f)){
	if (f->name=="a1")  {SDL_RenderCopyEx(renderer, imga1,NULL,&e,angletest*f->as*f->asign,NULL,f->flip);
	}//SDL_RenderCopy(renderer, imglum, NULL, &e);}
  
    if (f->name=="a2") {SDL_RenderCopyEx(renderer, imga2,NULL,&e,angletest*f->as*f->asign,NULL,f->flip);
    }//SDL_RenderCopy(renderer, imglum, NULL, &e);}
    if (f->name=="a3"){ SDL_RenderCopyEx(renderer, imga3,NULL,&e,angletest*f->as*f->asign,NULL,f->flip);
    }//SDL_RenderCopy(renderer, imglum, NULL, &e);}
    if (f->name=="a4"){ SDL_RenderCopyEx(renderer, imga4,NULL,&e,angletest*f->as*f->asign,NULL,f->flip);
    }//SDL_RenderCopy(renderer, imglum, NULL, &e);}
	 }//in xy
	}}
	
		
			
	for (auto ps:particules) {

	       ps->pv-=1*k0;
        SDL_SetRenderDrawColor(
        renderer, 
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0);
        SDL_RenderDrawPoint(renderer, ps->x, ps->y);
 if (ps->pv/k0>s[10]){
 SDL_SetRenderDrawColor(
        renderer, 
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0-100);
        SDL_RenderDrawPoint(renderer, ps->x+1, ps->y+1);
 }
if  (ps->pv/k0>s[9]){
 SDL_SetRenderDrawColor(
        renderer, 
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0-100);
        SDL_RenderDrawPoint(renderer, ps->x-1, ps->y-1);
 }
if  (ps->pv/k0>s[8]){
 
 
 SDL_SetRenderDrawColor(
        renderer, 
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0-100);
        SDL_RenderDrawPoint(renderer, ps->x+1, ps->y-1);
 }
if  (ps->pv/k0>s[7]){
 SDL_SetRenderDrawColor(
        renderer, 
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0-100);
        SDL_RenderDrawPoint(renderer, ps->x-1, ps->y+1);
 }
  if (ps->pv/k0>s[6]){
 SDL_SetRenderDrawColor(
        renderer, 
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0-100);
        SDL_RenderDrawPoint(renderer, ps->x, ps->y-1);
 }
  if (ps->pv/k0>s[5]){
  SDL_SetRenderDrawColor(
        renderer, 
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0-100);
        SDL_RenderDrawPoint(renderer, ps->x, ps->y+1);
 }
  if (ps->pv/k0>s[4]){
  SDL_SetRenderDrawColor(
        renderer, 
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0-100);
        SDL_RenderDrawPoint(renderer, ps->x-1, ps->y);
 }
  if (ps->pv/k0>s[3]){
  SDL_SetRenderDrawColor(
        renderer, 
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0-100);
        SDL_RenderDrawPoint(renderer, ps->x+1, ps->y);
 }

        ps->update();
        }
    for (auto ps:particulesr) {
        SDL_SetRenderDrawColor(
        renderer, 
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0+100+rand()%55,
        0,//ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0);
        SDL_RenderDrawPoint(renderer, ps->x, ps->y);
 if (ps->pv/k0>s[15]){
 SDL_SetRenderDrawColor(
        renderer, 
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0+100+rand()%55,
        0,//ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0-100);
        SDL_RenderDrawPoint(renderer, ps->x+1, ps->y+1);
 }
if  (ps->pv/k0>s[14]){
 SDL_SetRenderDrawColor(
        renderer, 
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0+100+rand()%55,
        0,//ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0-100);
        SDL_RenderDrawPoint(renderer, ps->x-1, ps->y-1);
 }
if  (ps->pv/k0>s[13]){
 SDL_SetRenderDrawColor(
        renderer, 
        ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0+100+rand()%55,
        0,//ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0-100);
        SDL_RenderDrawPoint(renderer, ps->x+1, ps->y-1);
 }
if  (ps->pv/k0>s[12]){
 SDL_SetRenderDrawColor(
        renderer, 
        ps->pv/6*k0+100+rand()%55,
        0,//ps->pv/6*k0+100+rand()%55,
        0,//ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0-100);
        SDL_RenderDrawPoint(renderer, ps->x-1, ps->y+1);
 }
  if (ps->pv/k0>s[11]){
 SDL_SetRenderDrawColor(
        renderer, 
        ps->pv/6*k0+100+rand()%55,
        0,//ps->pv/6*k0+100+rand()%55,
        0,//ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0-100);
        SDL_RenderDrawPoint(renderer, ps->x, ps->y-1);
 }
  if (ps->pv/k0>s[10]){
  SDL_SetRenderDrawColor(
        renderer, 
        ps->pv/6*k0+100+rand()%55,
        0,//ps->pv/6*k0+100+rand()%55,
        0,//ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0-100);
        SDL_RenderDrawPoint(renderer, ps->x, ps->y+1);
 }
  if (ps->pv/k0>s[9]){
  SDL_SetRenderDrawColor(
        renderer, 
        ps->pv/6*k0+100+rand()%55,
        0,//ps->pv/6*k0+100+rand()%55,
        0,//ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0-100);
        SDL_RenderDrawPoint(renderer, ps->x-1, ps->y);
 }
  if (ps->pv/k0>s[8]){
  SDL_SetRenderDrawColor(
        renderer, 
        ps->pv/6*k0+100+rand()%55,
        0,//ps->pv/6*k0+100+rand()%55,
        0,//ps->pv/6*k0+100+rand()%55,
        ps->pv/6*k0-100);
        SDL_RenderDrawPoint(renderer, ps->x+1, ps->y);
 }

        ps->update();
        }
      nbt=0;
      if (contgui!=0){
          contgui-=1;
      }
	for(auto e:texrexplos)
	for(auto f:explos){
	e.x=f->xm;
	e.y=f->ym;
	e.w=f->w;
	e.h=f->h; 
	
	if ((f->name=="pous") or(f->name=="tj2") or(f->name=="tj1")) SDL_RenderCopyEx(renderer, imgpous,NULL,&e,angletest*f->as*f->asign*5,NULL,f->flip);
//SDL_RenderCopy(renderer, imgpous, NULL, &e);
    else if (f->name=="touch") {
    	f->w*=1.2;
    	f->h*=1.2;
    	//maj
        SDL_RenderCopy(renderer, imgtouch, NULL, &e);
   
   if (coleee(f,rouage1) or coleee(f,rouage2)) {
       nbt++;if (contgui==0 and coleee(f,rouage1))
       isrouageg();
   if (contgui==0 and coleee(f,rouage2))
       isrouaged();
   }
   if (nbt>1 and contgui==0 and isrouages()){ 
   resettimerrouages();nbt=0;setgui=not setgui;contgui=100;}
   if (setgui and contgui==0 and colee(iaoo,f)){contgui=100;setia=not setia;}
   } else SDL_RenderCopyEx(renderer, imgexplo,NULL,&e,angletest*f->as*-1*10,NULL,f->flip);
//SDL_RenderCopy(renderer, imgexplo, NULL, &e);
	    }
	 if (timerisrouage!=0)   {timerisrouage--;
	     if (timerisrouage==1) {
	         contgui=100;
	         resettimerrouages();
	         }
	     }
	 
	    
	    for(auto i=explos.begin();i!=explos.end();)
    {
      eexpl *e = *i;
       if (e->fdvj()){
           e->pbexpl=false;
       eexpl *e1=new eexpl("pous",e->x,e->y,HEIGHT/40,
       80,e->vx*0.1,e->vy*0.1);
             explos.push_front(e1);
             }  
                           
      if (e->pv==0) {i=explos.erase(i); delete e;
      }
      else { e->upexpl();
      i++;}
    }
    //crabaff vect deg meaning
    //SDL_RenderCopyEx(renderer, imgcrabe,NULL,&texcrabe,iago->v.angle,NULL,flip);

	//continuer? aff
	if (cont->pv!=1){SDL_RenderCopy(renderer, imgcont, NULL, &texrcont);
    cont->update();
	cont->unctrl();
    }
    if (setgui){
    if (setia){SDL_RenderCopy(renderer,imgiaoo,NULL,&texriaoo);
      iaoo->update();iaoo->unctrl();
          }
     if (not setia ){SDL_RenderCopy(renderer,imgiaoo0,NULL,&texriaoo);
      iaoo->update();iaoo->unctrl();
        }
    }
    //SDL_SetRenderDrawColor(renderer, 255,25,255,255);
    
    
    //for(auto a:texrp)
    //for(auto i=debris.begin();i!=debris.end();)
   // {
      //part *e = *i;
   
      //SDL_SetRenderDrawColor(renderer, 255,255,255,255);

  //  a.x=e->x;a.y=e->y;
    //if (e->pv==0) {i=debris.erase(i); delete e;}
    //else i++;
    //SDL_RenderCopy(renderer, imgp, NULL, &a);}
            
												//usleep(1000*0.9);//1milisec car u en micros
//      gu i
           SDL_RenderCopy(renderer, imggunrl, NULL, &texrgunrl);
            SDL_RenderCopy(renderer, imggunbl, NULL, &texrgunbl);
            SDL_RenderCopy(renderer, imgcoeurrl, NULL, &texrcoeurrl);
            SDL_RenderCopy(renderer, imgcoeurbl, NULL, &texrcoeurbl);
            SDL_RenderCopy(renderer, imgeclb, NULL, &texreclb);
            SDL_RenderCopy(renderer, imgeclr, NULL, &texreclr);

//SDL_RenderCopy(renderer, imgpr, NULL, &texrpr);
//SDL_RenderCopy(renderer, imgpr, NULL, &texrpr2);
	
	if (apap){while (apap){usleep(intlatence*5);} 
	SDL_DestroyRenderer( renderer );
    //win.free(); si arriere plan detruire le rend et recharger les textures
	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	
    
    img = IMG_LoadTexture(renderer, IMG_PATH);
    SDL_QueryTexture(img, NULL, NULL, &w, &h);
    img2 = NULL;
    img2 = IMG_LoadTexture(renderer, IMG_PATH2);
    SDL_QueryTexture(img2, NULL, NULL, &w, &h);
    imgfond = NULL;
    imgfond = IMG_LoadTexture(renderer, IMG_PATHfond);
    SDL_QueryTexture(imgfond, NULL, NULL, &w, &h);
    imgfond0 = NULL;
    imgfond0 = IMG_LoadTexture(renderer, IMG_PATHfond0);
    SDL_QueryTexture(imgfond0, NULL, NULL, &w, &h);
    imgfond00 = NULL;
    imgfond00 = IMG_LoadTexture(renderer, IMG_PATHfond00);
    SDL_QueryTexture(imgfond00, NULL, NULL, &w, &h);
    imgfond000 = NULL;
    imgfond000 = IMG_LoadTexture(renderer, IMG_PATHfond000);
    SDL_QueryTexture(imgfond000, NULL, NULL, &w, &h);
     imgfondast = NULL;
    imgfondast = IMG_LoadTexture(renderer, IMG_PATHfondast);
    SDL_QueryTexture(imgfondast, NULL, NULL, &w, &h);
    imgfondast2 = NULL;
    imgfondast2 = IMG_LoadTexture(renderer, IMG_PATHfondast2);
    SDL_QueryTexture(imgfondast2, NULL, NULL, &w, &h);
    imgfondast3 = NULL;
    imgfondast3 = IMG_LoadTexture(renderer, IMG_PATHfondast3);
    SDL_QueryTexture(imgfondast3, NULL, NULL, &w, &h);
    imgfondast4 = NULL;
    imgfondast4 = IMG_LoadTexture(renderer, IMG_PATHfondast4);
    SDL_QueryTexture(imgfondast4, NULL, NULL, &w, &h);
    imgfondast5 = NULL;
    imgfondast5 = IMG_LoadTexture(renderer, IMG_PATHfondast5);
    SDL_QueryTexture(imgfondast5, NULL, NULL, &w, &h);
    
    imgfondjup = NULL;
    imgfondjup = IMG_LoadTexture(renderer, IMG_PATHfondjup);
    SDL_QueryTexture(imgfondjup, NULL, NULL, &w, &h);
    
    imgexplo = NULL;
    imgexplo = IMG_LoadTexture(renderer, IMG_PATHexplo);
    SDL_QueryTexture(imgexplo, NULL, NULL, &w, &h);

    imgsrl = NULL;
    imgsrl = IMG_LoadTexture(renderer, IMG_PATHsrl);
    SDL_QueryTexture(imgsrl, NULL, NULL, &w, &h);
    
    imgsbl = NULL;
    imgsbl = IMG_LoadTexture(renderer, IMG_PATHsbl);
    SDL_QueryTexture(imgsbl, NULL, NULL, &w, &h);
    
    imggunrl = NULL;
    imggunrl = IMG_LoadTexture(renderer, IMG_PATHgunrl);
    SDL_QueryTexture(imggunrl, NULL, NULL, &w, &h);
    
    imggunbl = NULL;
    imggunbl = IMG_LoadTexture(renderer, IMG_PATHgunbl);
    SDL_QueryTexture(imggunbl, NULL, NULL, &w, &h);
    
    imgcoeurrl = NULL;
    imgcoeurrl = IMG_LoadTexture(renderer, IMG_PATHcoeurrl);
    SDL_QueryTexture(imgcoeurrl, NULL, NULL, &w, &h);
    
    imgcoeurbl = NULL;
    imgcoeurbl = IMG_LoadTexture(renderer, IMG_PATHcoeurbl);
    SDL_QueryTexture(imgcoeurbl, NULL, NULL, &w, &h);
    
    imgeclr = NULL;
    imgeclr = IMG_LoadTexture(renderer, IMG_PATHecl);
    SDL_QueryTexture(imgeclr, NULL, NULL, &w, &h);
    
    imgeclb = NULL;
    imgeclb = IMG_LoadTexture(renderer, IMG_PATHecl);
    SDL_QueryTexture(imgeclb, NULL, NULL, &w, &h);
    
    imgburn = NULL;
    imgburn = IMG_LoadTexture(renderer, IMG_PATHfeux);
    SDL_QueryTexture(imgburn, NULL, NULL, &w, &h);
    
    imgcont = IMG_LoadTexture(renderer, IMG_PATHcont);
    SDL_QueryTexture(imgcont, NULL, NULL, &w, &h);
    
    imgp = IMG_LoadTexture(renderer, IMG_PATHp);
    SDL_QueryTexture(imgp, NULL, NULL, &w, &h);

    imga1 = IMG_LoadTexture(renderer, IMG_PATHa1);
    SDL_QueryTexture(imga1, NULL, NULL, &w, &h);
    
    imga2 = IMG_LoadTexture(renderer, IMG_PATHa2);
    SDL_QueryTexture(imga2, NULL, NULL, &w, &h);

    imga3 = IMG_LoadTexture(renderer, IMG_PATHa3);
    SDL_QueryTexture(imga3, NULL, NULL, &w, &h);
    
    imga4 = IMG_LoadTexture(renderer, IMG_PATHa4);
    SDL_QueryTexture(imga4, NULL, NULL, &w, &h);
    
    imgpr = IMG_LoadTexture(renderer, IMG_PATHpr);
    SDL_QueryTexture(imgpr, NULL, NULL, &w, &h);
    
    imgpous = IMG_LoadTexture(renderer, IMG_PATHpous);
    SDL_QueryTexture(imgpous, NULL, NULL, &w, &h);
    
    imgtouch = IMG_LoadTexture(renderer, IMG_PATHtouch);
    SDL_QueryTexture(imgtouch, NULL, NULL, &w, &h);
    
    imglum = IMG_LoadTexture(renderer, IMG_PATHlum);
    SDL_QueryTexture(imglum, NULL, NULL, &w, &h);
  
    imgmiss = IMG_LoadTexture(renderer, IMG_PATHmiss);
    SDL_QueryTexture(imgmiss, NULL, NULL, &w, &h);
    
    imgtif = IMG_LoadTexture(virtrend, IMG_PATHtif);
    SDL_QueryTexture(imgtif, NULL, NULL, &w, &h);
    
        
    imgtj1=NULL;
    imgtj2=NULL;
    imgtj1= IMG_LoadTexture(renderer, IMG_PATHtj1);
    SDL_QueryTexture(imgtj1, NULL, NULL, &w, &h);
    imgtj2= IMG_LoadTexture(renderer, IMG_PATHtj2);
    SDL_QueryTexture(imgtj2, NULL, NULL, &w, &h);
    
    imgiaoo= IMG_LoadTexture(renderer, IMG_PATHiaoo);
    SDL_QueryTexture(imgiaoo, NULL, NULL, &w, &h);
    
    imgiaoo0= IMG_LoadTexture(renderer, IMG_PATHiaoo0);
    SDL_QueryTexture(imgiaoo0, NULL, NULL, &w, &h);
    
    
    imgrouage= IMG_LoadTexture(renderer, IMG_PATHrouage);
    SDL_QueryTexture(imgrouage, NULL, NULL, &w, &h);
    

    imgcrabe= IMG_LoadTexture(renderer, IMG_PATHcrabe);
    SDL_QueryTexture(imgcrabe, NULL, NULL, &w, &h);

	

	
	}
 SDL_RenderPresent(renderer);

	
			//if(not Mix_PlayingMusic()) {//mix
			//Mix_FreeMusic(music);
	        //Mix_CloseAudio();
			    //(music);
			//}
			    //Mix_Music *music = loadMusic("music.ogg");
			    //
	
//usleep(1000*mintime*2*-cor*3500);

}//end runed
//runed=false;
}
void threadaux1(double fps2)
{ 
int taux2b=0;
while(runed){
  //  for (int i=1;i<(bcltests*2);++i) {// pour test
	//std::cout << "aux1" << std::endl;
	while (tics3) usleep(intmint/100);
	tics3=true;
	usleep(intlatence*4);

	//usleep((intmint*4)-intlatence);
	//std::cout << " tempo " <<(intmint)-intlatence;
	//th3();
	Spritej1->update();
	Spritej2->update();
	//ia1->update();
	//ia2->update();
	//iago->update();
	//Spritej1->unctrl();
	//Spritej2->unctrl();
	//Spritefondast->ast();
	//Spritefondast2->ast();
	//Spritefondast3->ast();
	//Spritefondast4->ast();
	//Spritefondast5->ast();
	Spritefond->up360();
	Spritefond000->vib();
	Spritefond2->vib();
	Spritefondjup->up360();
	//for(auto f:sa1)f->ast();
		 SpriteVEC->x=Vec->x;
	SpriteVEC->y=Vec->y;
	SpriteVEC->h=100;
	SpriteVEC->w=100;
	         
	 if (loosej1->pv>0 or  loosej2->pv>0){
	     taux2b+=1;
	     if(taux2b>70){
	         cont->pv=0;
	     
	         //cont->vy=1;
	         //cont->vx=1;
	         //cont->sh=HEIGHT/10;
	         //cont->sw=HEIGHT/10;   
	         //cont->h=HEIGHT/10;
	         //cont->w=HEIGHT/10;
	         //_cont->update();      
	         //cont->updatetir();
	         //cont->unctrl(); 
	                    
	if (Vec->down){
	        
	    //taux2b=0;
	    
	    if (colee(SpriteVEC,cont)){
	        iago->xy.setxy(W*025,H*0.25);
	        setastswall();
	        taux2b=0;
	        Spritej2->x =WIDTH*0.70;
	Spritej2->y =HEIGHT*0.75;
	Spritej1->y=HEIGHT*0.15;
	Spritej1->x=WIDTH*0.25;
	        Spritej1->pv=1000;
	        Spritej1->nrj=0;
	         loosej1->pv=0;
	         Spritej2->pv=1000;
	         Spritej2->nrj=0;
	         loosej2->pv=0;}}
	    
	 } }else {
	         cont->pv=1;
	         //cont->sh=0;
	         //cont->sw=0;
	         //cont->x=6000;
	         //cont->y=6000;}//vec-down        <
	 }

	
	

     if( not c->inlng(Spritej1->x,WIDTH/2,0.9*WIDTH/2)){
        ia2->x-=ia1->x-WIDTH/2;
        if (ia0->deg>180) ia0->deg-=180;else ia0->deg+=180;
    }
    if( not c->inlng(Spritej1->y,HEIGHT/2,0.9*HEIGHT/2)){
        ia2->y-=ia1->y-HEIGHT/4;
        if (ia0->deg>180) ia0->deg-=180;else ia0->deg+=180;

    }
  
	//usleep(1000*mintime*4*-cor*3500);
	 
}//end runed
}
void threadaux2(double fps2)
{ 

//Mix_FreeMusic(music);
//for (int i=1;i<bcltests;++i) {// pour test
while(runed){
	
	//std::cout << "aux2" << std::endl;
	while (tics4) usleep(intlatence*4);
	tics4=true;
	usleep(intlatence*2);
	Spritesbl->updatetir();
	Spritesrl->updatetir();
	Spritegunbl->updatetir();
	Spritegunrl->updatetir();
	Spritecoeurbl->updatetir();
	Spritecoeurrl->updatetir();
	Spriteeclb->updatetir();
	Spriteeclr->updatetir();
	Spritecoeurbl->sw=Spritej2->pv*0.1;
	Spritecoeurbl->sh=Spritej2->pv*0.1;
	Spritecoeurrl->sw=Spritej1->pv*0.1;
	Spritecoeurrl->sh=Spritej1->pv*0.1;
	
	
	//Spritefondjup->vitae();
	//Spritefondjup->vitae();
	
	//for (int iii=1;iii<1000;++iii) {
          //part *b3 = new part(b2->x,b2->y,rand()%60000);
          //debris.push_back(b3);}
          
     //for(auto a:debris) a->up();
	
	//if(sqrt(cont->vx*cont->vx)+sqrt(cont->vy*cont->vy)<1){
	
           	

	if(Spritej1->pv<1 and loosej1->pv==0) {
	loosej1->pv=1;
	for (int ii=1;ii<3;++ii) {
	sprite *b1 = new sprite();
	
           b1->name="j1";
           b1->sh*=0.3;
           b1->sw*=0.3;
           b1->frame=1+ii*50;
           b1->frames=956;
           //b1->sx=Spritej2->y-Vec->y;
           //b1->sy=Spritej2->y-Vec->y;
           burnsj1.push_back(b1);
	}}
	if(Spritej2->pv<1 and loosej2->pv==0) {
	loosej2->pv=1;
	for (int ii=1;ii<3;++ii) {
	sprite *b1 = new sprite();
           b1->name="j1";
           b1->sh*=0.3;
           b1->sw*=0.3;
           //b1->sx=0;//Spritej2->y-Spritej2->w/2;//-Vec->y;
           //b1->sy=0;//Spritej2->y-Spritej2->h/2;//Vec->y;
           b1->frame=1+ii*50;
           b1->frames=956;
           burnsj2.push_back(b1);
           
           //cont->x=600;
	        //cont->y=500;
	}}
	if(tirj1 ==0 and Spritej1->pv<1001 and Spritej1->pv>5){
	Spritej1->pv+=0.1*k0;
	//taux2a=0;
	}
    if(tirj2 ==0 and Spritej2->pv<1001 and Spritej2->pv>5){
	Spritej2->pv+=0.1*k0;
	//taux2a=0;
	}
	
	 cont->update();      //anim continue
	 //cont->updatetir();
	 //cont->unctrl(); 
	 //cont->sh=500;
	  //cont->sw=500;
	   //cont->x=000;
	   //cont->y=000;
	   //cont->vx=0.5;
	    //cont->vy=0.5;

	 
	//usleep(intmint*8-intlatence);
	//th4();
	
	
	
	
	//Spritej1->y =Spritej1->y-Spritej1->vy;
	//Spritej1->x =Spritej1->x-Spritej1->vx;
	
	
	//usleep(1000*mintime*8-cor*3500);
}//end runed
}
void rEpartiteur(){
    
    
    bool up,choisir,action;
    
    bool unused=true;
    int tid;
    float tx,ty;
    while(runed){
    
//range les events à court délais.

        //pollloop
    SDL_Event e;
    
    //if (sdlstarted) nop=0;
    //SDL_WaitEvent(SDL_Event au lieu de while SDL_PollEvent
    usleep(intlatence*4);
    //SDL_PumpEvents();
    //while(true){
    while ( SDL_WaitEvent ( &e )) {
        //tid=SDL_GetTouchDevice(0);
        //std::cout << " start id pour 0 >" << tid<< std::endl;
        //usleep(intlatence*100);
        //près switch pour multitouch deux voire plus
        up=false;
        Vec->up=false;
        Vec->move=false;
        Vec->down=false;
        choisir=false;
        unused=true;
        action=false;
        tid=e.tfinger.fingerId;
             //tx=e.tfinger.x*tw*1-tw*0.01;
            //ty=e.tfinger.y*th*1-th*0.01;
        tx=e.tfinger.x*tw*1;//-tw*0.1;
         ty=e.tfinger.y*th*1;//-th*0.01;
        Vec->x=tx;Vec->y=ty;Vec->id=tid;
        
        //std::cout<< "  "<<e.tfinger.fingerId<<"  "<<SDL_GetTouchDevice(0)<<std::endl;
     
    switch( e.type) {
        case SDL_QUIT:{
        	closesdl();
        	break;
        	}
        
        //arriere plan glprorect
        case SDL_APP_DIDENTERBACKGROUND:{
        	apap=true;
        	break;
        	}
        	case SDL_APP_DIDENTERFOREGROUND:{
        		apap=false;
        		break;}
        case SDL_FINGERDOWN: {
           //sprite *b1 = new sprite();
           
           //yt[tid]=ty;xt[tid]=tx;
           //b1->setexplo("touch",tx,ty,HEIGHT/40,80);
           
           eexpl *e1=new eexpl("touch",tx,ty*1.09,HEIGHT/40,100,0,0);
           
           //b1->x=tx;
           //b1->y=ty;
           //b1->frame=1;
           //b1->frames=80000;
           explos.push_back(e1);
                //std::cout <<tid<<"- (do) -";
                //std::cout <<e.tfinger.touchId<<"     down"<<e.tfinger.fingerId<<std::endl;
                //std::cout <<e.tfinger.x*WIDTH<<"     idem"<<WIDTH/2<<std::endl;
                up=false;
                
                Vec->down=true;
                action=true;
                break;
        }//out finger down
        
        
        case SDL_FINGERMOTION: {
                    
                 //std::cout<< "- mo -";
                 //std::cout <<"            motion"<<e.tfinger.fingerId<<std::endl ;
                action=true;
         	    Vec->move=true;
         
         	    
         	    break;         
        }//out finger move
        case SDL_FINGERUP: {
                 up=true;
                 //std::cout<< "- (up) -" << tid;
                 //std::cout <<"            up"<<e.tfinger.fingerId<<std::endl;
         	    Vec->up=true;
         	    action=false; 
                if (Spritej2->id==tid){
                        Spritej2->ctrl=false;
                        Spritej2->id=100;
                    }
                    if (Spritej1->id==tid){
                        Spritej1->ctrl=false;
                        Spritej1->id=100;
                    }
                    break;     	
    }//out fingerup
    //default: std::cout << "";
    }//switch
       
    //post switch in
    //postswitch out
    //}//pollloop
    //} //switch loop

	        
	 //std::cout <<"id >>"<<e.tfinger.fingerId<< std::endl;
    //std::cout <<e.type<<"  << type <end>0.5w "<<std::endl;
 if (action and (loosej1->pv<1 and loosej2->pv<1)) { //in action
// std::cout <<" a ";
 
 
 if (ty>HEIGHT/2){//mid screen
                    //std::cout <<" b ";
     if (Vec->down and Spritej2->id==100 and Spritej1->id!=tid){
                        Spritej2->ctrl=true;
                        Spritej2->id=tid;
                        //std::cout <<" c1 ";
                    }
    if (Spritej2->id!=tid and Spritej1->id!=tid and (Vec->down )){//or Vec->moved()
            
            if (Spritej2->id!=100)tirerj2();
     
        
                }     
                    
        }//mid screen
         else {
                 if (!setia and Vec->down and Spritej1->id==100 and Spritej2->id!=tid){
                        //std::cout <<" c2 ";
                        Spritej1->ctrl=true;
                        Spritej1->id=tid;
          }         
             
            if (Spritej2->id!=tid and Spritej1->id!=tid and (Vec->down)){
            if (Spritej1->id!=100)tirerj1();
       }         
             
             
     
          }  
      //out action
     
    bool inx = c->inlng(tx,WIDTH/2,0.9*WIDTH/2);
    bool iny= c->inlng(ty,HEIGHT/2,0.9*HEIGHT/2);

   if (Spritej1->ctrl and tid==Spritej1->id){
          if (inx) Spritej1->x=tx;
          if (iny) Spritej1->y=ty;
         // std::cout <<" d1 ";       
      }
   if (Spritej2->ctrl and tid==Spritej2->id){
          if (inx) Spritej2->x=tx;
          if (iny) Spritej2->y=ty;
          //std::cout <<" d2 ";
       }
        
 }//action     
              
} //out eventloopif while event
    }//end while runed
}//end void repart
void nosync(double fps2){ 
//lanceur de boucles courtes sans sync
 while (runed){
    //
    rEpartiteur();
 }
}//end void nonsync
// ici void thread<

// cstdlib is needed for atoi()


//le 25 juillet_21 sauv et prog auro adjust cpu 
//mesure une 0.0001ms execution
//la fonction d'atente est bloquée entre 0.3 et .03 ms.
// impossible de sync les threads comme ça en dessous de 01ms
 // tentative pour etaloner les void par thread à un temps univ. ou qui s'en raproche.

int main()
{
    //hors timer
    
    
    
    //hors timer

    double atime[100];
    for (int i=1;i<100;++i){
        atime[i]=1;
    }
    double incrsetfps=0.1;
    double reftime95=0.93;// milli
    double refcoef95 =0.6;
    double fps2=2;//pour 100 fps 20 milli
    int btime=0;
    double mintime=10;
    double adjtime=10;
    double a=0;
    double b=0;
    bool lunchfps=false;
    bool lunchaux1=false;
    bool lunchaux2=false;
    int incrfps=0;
    int incraux1=0;
    int incraux2=0;
    bool st=false;//switchtimer
    auto deb = std::chrono::high_resolution_clock::now();
  // boucle def correction cor (b) de f《ax+b
    for (int i=1;i<100;++i) {
        auto start = std::chrono::high_resolution_clock::now();
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::micro> speedtest = end-start;
        std::cout << "-"<<speedtest.count()<<std::flush;
        usleep(1000);
        for (int ii=1;ii<100;++ii) {
            
            a=0;
            b=0;
        
        }
    }
   auto end = std::chrono::high_resolution_clock::now();
   std::chrono::duration<double, std::milli> cort = end-deb;
   cor=cort.count()/100;
  //boucle auto sync
  while (refcoef95<0.95){
    mintime=reftime95;
    for (int i=1;i<50;++i) {//test adjustement start 
     using namespace std::chrono_literals;
    
    adjtime=0;
    for (int ii=1;ii<40;++ii){
        adjtime+=atime[ii];
    }
    coef=adjtime/40; 
    
    //usleep(1000*mintime*coef);
    
    ++btime;
    if (btime>40){
        btime=0;
    }
    if (st){
        st=false;
        usleep(int(1000*mintime*coef));
        //usleep(int(1000*mintime-76));
        auto end = std::chrono::high_resolution_clock::now();
        
        //std::cout <<"end"<< std::flush;
        std::chrono::duration<double, std::milli> elapsed2 = end-deb;
        std::cout << "》     t2        》》 " << elapsed2.count() << " ms\n";
        //std::chrono::duration<double, std::milli> elapsed = elapsed2-elapsed1;
        b=elapsed2.count();
        std::cout << mintime<<" <mintime 》     Waited>    》》》" << (b-a)<< " ms  coef 》" << coef<< " \n";
       stableproc=reftime95;
       atime[btime]=mintime/(b-a);
    }
    else{
        std::cout << mintime << "  " << mintime/(b-a)<<" Hello waiter \n" << std::flush;
        st=true;
        //std::cout <<"start"<< std::flush;
        auto start = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed1 = start-deb;
        std::cout << "》     t1        》" << elapsed1.count() << " ms\n";
        a=elapsed1.count();
    }//else
    
    }//phase in phases exemple 20ms pour 50fps
  reftime95+=incrsetfps;
  incrsetfps*=2;
  refcoef95=coef;
  //usleep(100000); //pour visualisation ou respiration buffers
  }//phases
 //set fps and physics(fps×10)
 intlatence=(int(mintime*1000/100*5));
 std::cout << " latence: " << intlatence;
 //simuler 4 à 22. x<5<10<20 >
 //stableproc=9;
 if (stableproc<8){// avant pour 100fps <5
    fps2=8;
    //intmint=8000;fps 5
    intmint=8000;
    k0=1;
    std::cout << "》     reglé60 pour fps 100  car cpu en dessous de 5 ms"<< reftime95;
    //fps100
 }
 else{
     if (stableproc<10){
         fps2=10;
         //intmint=10000;
         intmint=10000;
         k0=1*60/50;
         std::cout << "》      fps   50 car cpu en dessous de 10 ms"<< reftime95;
    //fps50
     }
     else{
          if (stableproc<20){
             fps2=20;
             intmint=20000;
             k0=1*60/25;
             std::cout << "》      fps  25 car cpu en dessous de 20 ms"<< reftime95;
        //fps25
          }
          else{
             fps2=40;
             intmint=40000;
             k0=1*60/12.5;
             std::cout << "》      fps 12.5 car  cpu instable au dessus de 20 ms"<< reftime95;
             
     }//25
     }//else50    
 }//else fps 100
 mintime=fps2;
 st=false;
 
//ici gestion threads》》》》》》》 

//while (runed){
    
    
    for (int i=1;i<2;++i) {//test adjustement start 
    //usleep(1000*mintime-cor*3500);
    
    

	

    //runed=false;
    
    
    incrfps++;
    if (incrfps>-1){
            if (st){
        st=false;
        auto end = std::chrono::high_resolution_clock::now();
        //std::cout <<"end"<< std::flush;
        std::chrono::duration<double, std::milli> elapsed2 = end-deb;
        //std::cout << "》FPS 》 " << elapsed2.count() << " ms\n";
        //std::chrono::duration<double, std::milli> elapsed = elapsed2-elapsed1;
        b=elapsed2.count();
        std::cout <<b-a<<" p/ms " << 1/((b-a)/1000)<< " FPS / coef  》" << coef<<" physicsT ms "<<mintime<< " \n";
       //atime[btime]=(mintime)/(b-a);
    }
    else{
        //std::cout << mintime << "  " << mintime/(b-a)<<" Hello waiter \n" << std::flush;
        st=true;
        //std::cout <<"start"<< std::flush;
        auto start = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed1 = start-deb;
        //std::cout << "》     t1        》" << elapsed1.count() << " ms\n";
        a=elapsed1.count();
    }//else
        incrfps=0;
        incraux1++;
        incraux2++;
        if (incraux1>-2){
            incraux1=0;
            lunchaux1=true;
        }
        if (incraux2>-3){
            incraux2=0;
            lunchaux2=true;
        }
        lunchfps=true;
    }
    
    std::thread t(&nosync, fps2);
    std::thread t0(&ttics, fps2);
    
    std::thread t1(&threadphysics, fps2);
    std::thread t2(&threadfps, fps2);
    std::thread t3(&threadaux1, fps2);
    std::thread t4(&threadaux2, fps2);
    
        
    t0.join();    
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t.join();
    lunchfps=false;
    lunchaux1=false;
    lunchaux2=false;
    
    
    if (lunchfps){
        lunchfps=false;
        std::thread t2(&threadfps, fps2);
        t2.join();
    }
    if (lunchaux1){
        std::thread t3(&threadaux1, fps2);
        t3.join();
    lunchaux1=false;
    }
    if (lunchaux2){
        std::thread t4(&threadaux2, fps2);
        t4.join();
    lunchaux2=false;
    }
    
    
    
}//while runed end brace

//ici gestion threads《《《《《 
    
    
}//main
//
//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //SDL_Delay(5);
    //std::this_thread::sleep_for(0.0000001ms);
    //auto end = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double, std::milli> elapsed = end-start;
    //std::cout << "Waited " << elapsed.count() << " ms\n";
    //auto start = std::chrono::high_resolution_clock::now();
    //auto end = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double, std::milli> elapsed1 = end-start;
