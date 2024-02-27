#ifndef __T_HPP_INCLUDED__   // if x.h hasn't been included yet...
#define __T_HPP_INCLUDED__   //   #define this so the compiler knows it has been included


#include <time.h>
#include <list>
#include <iostream> 
#include <cmath>
#define PI 3.141592653589793238462643383
 //3.14159265
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#define IMG_PATHmiss "./resources/assets/pict/missilebb.png"
#define IMG_PATHcrabe "./resources/assets/pict/crabe.png"
#define IMG_PATHiaoo "./resources/assets/pict/iaoo.png"
#define IMG_PATHiaoo0 "./resources/assets/pict/iaoo0.png"
#define IMG_PATHrouage "./resources/assets/pict/rouage.png"

#define IMG_PATHlum "./resources/assets/pict/lum3.png"
#define IMG_PATHcont "./resources/assets/pict/cont.png"
#define IMG_PATHa1 "./resources/assets/pict/aa1.png"
#define IMG_PATHa2 "./resources/assets/pict/aa2.png"
#define IMG_PATHa3 "./resources/assets/pict/aa3.png"
#define IMG_PATHa4 "./resources/assets/pict/aa4.png"
#define IMG_PATHpr "./resources/assets/pict/pb.png"
#define IMG_PATHpous "./resources/assets/pict/pouscccc.png"
#define IMG_PATHtouch "./resources/assets/pict/touchnbc.png"

#define IMG_PATHcoeurrl "./resources/assets/pict/coeurrl.png"
#define IMG_PATHcoeurbl "./resources/assets/pict/coeurbl.png"
#define IMG_PATHecl "./resources/assets/pict/ecl.png"
#define IMG_PATHloosej1 "./resources/assets/pict/loosej1.png"
#define IMG_PATHloosej2 "./resources/assets/pict/loosej2.png"

#define IMG_PATHtif "./resources/assets/pict/sbtiff.tif"

#define IMG_PATHgunrl "./resources/assets/pict/gunrl.png"
#define IMG_PATHgunbl "./resources/assets/pict/gunbl.png"
#define IMG_PATHsrl "./resources/assets/pict/sbl.png"
#define IMG_PATHsbl "./resources/assets/pict/srl.png"
#define IMG_PATHfeux "./resources/assets/pict/feuxtrlow.png"
#define IMG_PATHexplo "./resources/assets/pict/explobb.png"
#define IMG_PATH "./resources/assets/pict/so.png"
#define IMG_PATHtj2 "./resources/assets/pict/orberr.png"
#define IMG_PATHp "./resources/assets/pict/p.png"
#define IMG_PATHtj1 "./resources/assets/pict/orbebb.png"
#define IMG_PATH2 "./resources/assets/pict/sb.png"
#define IMG_PATHnop "./resources/assets/pict/nop.png"
#define IMG_PATHfond0 "./resources/assets/pict/fond4hlz.png"
#define IMG_PATHfond00 "./resources/assets/pict/suno.png"
#define IMG_PATHfond "./resources/assets/pict/marssoeur3.png"
#define IMG_PATHfond000 "./resources/assets/pict/sunrcc.png"
//#define IMG_PATHfond000 "./resources/assets/pict/sunc.png"
#define IMG_PATHfondast "./resources/assets/pict/ast.png"
#define IMG_PATHfondast2 "./resources/assets/pict/ast2.png"
#define IMG_PATHfondast3 "./resources/assets/pict/ast3.png"
#define IMG_PATHfondast4 "./resources/assets/pict/ast4.png"
#define IMG_PATHfondast5 "./resources/assets/pict/ast5.png"
#define IMG_PATHfondjup "./resources/assets/pict/jupsoeur4.png"
#include <Col.cs>
#include <fablib.hpp>
int WIDTH =780;
int HEIGHT =1680;
using namespace std;
extern float k0;
//extern tests c;
//using namespace sf;
tests *c=new tests;
//float testx=0;float testy=0;

        

class sprite
{
   
   public:
   bool died=false;
   std::string name;
   float pv;
   int idx;
   bool animated=false;
   bool ctrl,outx,outy;
   bool minage=false;
   int frame,frames,ir1,ir2,ir3,ir4;
  float x,y,x0,y0,x00,y00,speed;
   float xm,ym,w,h,x000,y000;
   float vx,vy;
   //public:
   float sh,sw,sx,sy,mx,my,r1,r2,r3,r4;
   float radstep,deg;
   int id=100;
   bool soutx=false;
   bool souty=false;
   int incr,timer,tout,rand0;
   double as,asign;
   float nrj;
     // | SDL_FLIP_VERTICAL;SDL_FLIP_NONE SDL_FLIP_HORIZONTAL
SDL_RendererFlip flip = SDL_FLIP_NONE;  
	

   
   
  
   
  public: 
   sprite(){
       as=1;//anglespeed
       asign=1;
       radstep=180/PI;
       ctrl=false;
       animated=true;
       frames=10;
       frame=0;
       pv=1000;
       died=false;
       x=500;
       y=500;
       vx=0;
       vy=0;
       h=HEIGHT/5;
       w=HEIGHT/5;
       speed=1;
       mx=x-w/2;
       my=y-h/2;
       xm=x+mx;
       ym=y+my;
       nrj=0;
       
      //mx=w/2;
      //my=h/2;
      //xm=x-mx;
      //ym=+y-my;
       
       
       incr=0;
       tout=0;
       sh=h;
       sw=w;
       sx=x;
       sy=y;
   }
       
   sprite(float nx,float ny,float nw, float nh){
       x=nx;
       y=ny;
       w=nw;
       h=nh;
       frames=3600;
       speed=1;
       
       //mx=xw/2;
       //my=y-h/2;
       
       
       }
 void setxywh(float nx,float ny,float nw, float nh){
       x=nx;
       y=ny;
       w=nw;
       h=nh;
       frames=3600;
       speed=1;
       
       //mx=xw/2;
       //my=y-h/2;
       
       
       }
   
void set(float nx,float ny,float nw, float nh,int ns){
       x=nx;
       y=ny;
       w=nw;
       h=nh;
       frames=1096;
       speed=ns;
       sx=nx;
       sy=ny;
       sh=nh;
       sw=nw;
       
         // | SDL_FLIP_VERTICAL;SDL_FLIP_NONE SDL_FLIP_HORIZONTAL
//SDL_RendererFlip flip = SDL_FLIP_NONE;  
	
}
       
void setv
(float nx,float ny,float nw, float nh,float nvx,float nvy, int ns){
       x=nx;
       y=ny;
       w=nw;//int((nw+nw+rand()%int(nw))/3);
       h=int((nh+nh+rand()%int(nh))/3);
       vx=nvx;
       vy=nvy;
       timer=1;
       frames=1096;
       speed=ns;
       ir1=HEIGHT/3;
       if(x==-1) x=rand()%ir1+ir1;
       if(y==-1) y=rand()%ir1+ir1;
      
       
       if(vx==-1)  vx=((rand()%WIDTH)-WIDTH/2)*0.005;
       if(vy==-1) vy=((rand()%HEIGHT)-HEIGHT/2)*0.005;
       vx/=1;
       vy/=1;
       sx=nx;
       sy=ny;
       sh=nh;
       sw=nw;
       as=rand()%60;
       rand0=rand()%1500;
       
       if(rand0>750) asign=1;else asign =-1;
       if (rand0>500 and rand0<1000)
       flip = SDL_FLIP_NONE;else  if (rand0>750)
       flip = SDL_FLIP_VERTICAL;else  
       flip = SDL_FLIP_HORIZONTAL;  
       
        
}
       
       //mx=xw/2;
       //my=y-h/2;
       

void startup(){
    mx=w/2;
      my=h/2;
      xm=x-mx;
      ym=+y-my;
}
void setcycles(){
    frames=1096;
    //speed=1;
}
   void update()
   {
      if (pv>1000) pv=1000;
      startup();
      //radstep=360/frames/PI;
      nrj*=0.997;
      
     if (animated){
     frame += speed*k0;
     
     int n = frames/k0;
     if (frame >= n) {speed *=-1;}
     if (frame<=-n) {speed *=-1;}
     if (frame!=0) {h+=speed;w+=speed*k0;}
     if (frame!=0) {
     //h=2*sh*sin (frame*radstep);
     //w=2*sw*sin (frame*radstep);
     //h=sh;w=sw;
     }
     
     }
   }


void vib()
   {
      startup();
      
     frame += speed;
     
     int n = frames;
     if (frame > n) {frame=1;}
     if (frame<=0) {frame=n;}
     //if (frame!=0) {h*=;}
     if (frame!=0) {
     x=int(sx+(cos (k0*frame*radstep*0.001)*0.001*h));
     y=int(sy+(sin (k0*frame*radstep*0.001)*0.001*h));
     w=int(sw+(cos (k0*frame*radstep*0.001)*0.001*h));
     h=int(sh+(sin (k0*frame*radstep*0.001)*0.001*h));
     
     //h=sh;w=sw;
     }   
     
    
   
   }
   void vitae()
   {
      startup();
      setcycles();
      
     frame += speed;
     
     int n = frames;
     if (frame > n) {frame=1;}
     if (frame<=0) {frame=n;}
     //if (frame!=0) {h*=;}
     if (frame!=0) {
     x=int(sx+(cos (k0*frame*radstep*0.001)*0.01*h));
     y=int(sy+(sin (k0*frame*radstep*0.001)*0.01*h));
     w=int(sx+(cos (k0*frame*radstep*0.001)*0.01*h));
     h=int(sy+(sin (k0*frame*radstep*0.001)*0.01*h));
     
     //h=sh;w=sw;
     }   
     
    
   
   }
     
void up360()
   {
      
      startup();
      setcycles();
      
      
     frame += speed;
     
     int n = frames;
     if (frame > n) {frame=1;}
     if (frame<=0) {frame=n;}
     //if (frame!=0) {h+=speed;w+=speed;}
     if (frame!=0) {
     x=int(sx+(cos (k0*frame*radstep*0.0001)*0.5*h));
     y=int(sy+(sin (k0*frame*radstep*0.0001)*0.5*h));
     
     //h=sh;w=sw;
     }   
     
   
   }
   void cont()
{
     if (not(ctrl)){
     x+=vx;
     y+=vy;
     }
   } 
void unctrl()
{
     if (not(ctrl)){
     x+=vx;
     y+=vy;
     if (name!="cont") {
     vx*=0.99;
     vy*=0.99;}
     }
     else{
        x000=x00;
        x00=x0;
        x0 = x;
        y000=y00;
        y00=y0;
	    y0 = y;
	    vx=-0.5*(x000-x0);
	    vy=-0.5*(y000-y0);
	    
     }
     //std::cout << " : "<<vx<<" : ";
     
}
void collide(){
    outx=false;outy=false;
    outx=not c->inlng(x,WIDTH/2,0.9*WIDTH/2);
    outy=not c->inlng(y,HEIGHT/2,0.9*HEIGHT/2);
    //if (x<0) outx=true
    //
    //outx=bool ((sqrt(xm*xm)-WIDTH*1)>0);
    //outy=bool ((sqrt(ym*ym)-HEIGHT*1)>0);
     //if (x>WIDTH-100 or x<0+100) outx=true;
     //if (y>HEIGHT-100 or y<0+100) outy=true;
     if (outx and soutx){
     vx*=-1;
     soutx=false;
     x+=1*vx;}
     if (not soutx and not outx)soutx=true;
     
     if (outy and souty){ 
     vy*=-1;souty=false;
     y+=1*vy;} 
     if (not souty and not outy) souty=true;
}


void  updatetir()
   {
     startup();
     //dx=cos(angle*DEGTORAD)*6;
     //dy=sin(angle*DEGTORAD)*6;
     // angle+=rand()%7-3;  /*try this*/
     x+=1*vx*k0;
     y+=1*vy*k0;
     frame += speed;
     
     int n = frames;
     if (frame >= n) {frame=0;}
     //if (frame<=2) {frame=n;}
      //x=int(x+(cos (frame*radstep*0.001)*0.1*h));
     //y=int(y+(sin (frame*radstep*0.001)*0.1*h));
     if (name!="miss"){w=int(sw+(cos (k0*frame*radstep*0.001)*0.1*h));
     h=int(sh+(sin (k0*frame*radstep*0.001)*0.1*h));
     } else{ w=sw*0.5;h=sh;}
     

     if (x>WIDTH+w or x<-w or y>HEIGHT+h or y<-h) pv=0;
   }
//asté

void ast()
{
    startup();
    
     x+=vx*k0;
     y+=vy*k0;
     if (timer!=0) {timer+=1;
     if (timer>250) timer=0;}

    outx=false;outy=false;
    
    if(not inab(x,-W/2,W+W/2) and not soutx) outx=true;
    else soutx=false;
    if (not inab(y,-H/2,H+H/2) and not souty) outy=true;
    else souty=false;
    //if (x<0) outx=true
    //
    //outx=bool ((sqrt(xm*xm)-WIDTH*1)>0);
    //outy=bool ((sqrt(ym*ym)-HEIGHT*1)>0);
     //if (x>WIDTH-100 or x<0+100) outx=true;
     //if (y>HEIGHT-100 or y<0+100) outy=true;
     if (outx and not soutx){
     vx*=-1;
     soutx=true;
     }
     
     
     if (outy and not souty){ 
     vy*=-1;souty=true;
     } }
//asté

};

class eexpl: public sprite
{

public:
bool i_NamXyHframes=false;
using sprite::sprite;
using sprite::mx;
 bool pbexpl=true;

eexpl(std::string nn,float xx,float yy,float tt,float ff,
float vxx,float vyy){
    name=nn;x=xx;y=yy;sh=tt;sw=tt;frames=ff;
    vx=vxx;vy=vyy;
    //w=1;h=1;
    
    w=(frames/k0+frame)*2*k0;//w=0.5*(frame*1+int(sw+(cos (frame*radstep*0.01)*0.01*h)));
     h=(frames/k0+frame)*2*k0;
     startup();
     pbexpl=true;
     //upexpl();
    //mx=w/2;
      //my=h/2;
      //xm=x-mx;
      //ym=y-my;
}

//void setexpl(std::string nn,float xx,float yy,float tt,float ff){
    //name=nn;x=xx;y=yy;sh=tt;sw=tt;frames=ff;
    //w=0;h=0;
//}//  name x y taille frames
bool fdvj(){// fin de vie explosion de joueurs
    
    return ((name=="j") and (frame>((13/k0)/13)*9 and pbexpl));
}
    
void  upexpl()
   {
       
     startup();
     x+=vx;y+=vy;
     //setcycles();
     //speed=1;
     frame += speed;
     //sh+=4;
     //sw+=4;
     
     int n = 13/k0;
     if (frame >= n) {speed=-1;}
     if (frame<((13/k0)/13)*-12) {pv=0;}
      //x=int(x+(cos (frame*radstep*0.001)*0.1*h));
     //y=int(y+(sin (frame*radstep*0.001)*0.1*h));
     w=(k0*frames/k0+frame)*2+cos (k0*frame*radstep*0.0035)*10;//w=0.5*(frame*1+int(sw+(cos (frame*radstep*0.01)*0.01*h)));
     h=(k0*frames/k0+frame)*2+sin (k0*frame*radstep*0.0035)*10;//h=0.5*(frame*1+int(sh+(sin (frame*radstep*0.01)*0.01*h)));
     
     

     //if (x>WIDTH+w or x<-w or y>HEIGHT+h or y<-h) pv=0;
   }
   };

class bj1: public sprite
{
   public:
   bj1()
   {
     setcycles();
     name="tj2";
     pv=1;
     x=500;
     y=500;
     h=50;
     w=50;
     sh=h;
     sw=w;
     frame=rand()%900;
   }

   void  updatetirb()
   {
         
     //dx=cos(angle*DEGTORAD)*6;
     //dy=sin(angle*DEGTORAD)*6;
     // angle+=rand()%7-3;  /*try this*/
     //x+=5*vx;
     //y+=5*vy;

     if (x>WIDTH or x<0 or y>HEIGHT or y<0) {
         pv=0;x=-1000;y=-1000;
         }
     
   }
};
   class bj2: public sprite
{
   public:
   bj2()
   {
     setcycles();
     name="tj1";
     pv=1;
     pv=1;
     x=500;
     y=500;
     h=50;
     w=50;
     sh=h;
     sw=w;
     frame=rand()%900;
   }

   void  updatetir()
   {
     //dx=cos(angle*DEGTORAD)*6;
     //dy=sin(angle*DEGTORAD)*6;
     // angle+=rand()%7-3;  /*try this*/
     //x+=5*vx;
     //y+=5*vy;

     if (x>WIDTH or x<0 or y>HEIGHT or y<0) pv=0;
   }

};
class vect{ 
//class t start
public:
int id; 
float x,y;
float xo=0;
float yo=0;
float xf=0;
float yf=0;
int angle=90;
float vx;
float vy;
int f;
bool actif;
int movet=0;

public:vect(){
actif=false;
id=100;
}

public: vect(int newid, int newx, int newy){ 
id = newid; 
x = newx;
y = newy;
if (xo==0) xo=x;
if (yo==0) yo=y;


 }
 
public: vect(int newid, int newx,int newy,int newangle){
id = newid; 
x = newx;
y = newy;
angle=newangle;

}
bool moved(){
        movet+=1;
        if (movet>3) {
            movet=0;
            return true;
        }
        else return false;
        
    }

void update(){
    if (xo==0) xo=x;
    if (yo==0) yo=y;
    
    } 
void settings(int Id,int X,int Y)
   {
     id=Id;x=X; y=Y;
   }
void print(){
}	 
};//class t end

class touch: public vect{
    public:
    bool used=true;
    bool up=false;
    bool down=false;
    bool move=false;
    using vect::vect;
    int movet=0;
};

class part: public sprite{
    public:
    float x,y,s,vx,vy;
    float pv=1;
    
    part(float xx,float yy,float ss){
        x=xx;y=yy;s=ss;
        vx=rand()%10-5;
        vy=rand()%10-5;
        pv=1;
       
        
        }
     void up(){
         
         x+=vx*0.001*s;y+=vy*0.001*s;s*=0.000000000001;
         if (s<1) pv=0;
     }
     
    
    
};
class mask{
    public:
    std::string name;
    SDL_Rect rect;// pour simplifier  le code car
    float x,y;
    int h,w,sh,sw;
    std::bitset<3000000> m;//2000 par 1000

        mask (int ww,int hh,std::string nname){
        
        //bool m[ww*hh];
       
        
        name=nname;
        sw=ww;sh=hh;
        w=ww;h=hh;

        //m[0]=false;
        //for (auto i:m){i=2;}
        }
        
void exemple() {//16exa 64bit
   unsigned  long a = 0x1F0;      // pattern  ...
   unsigned long b = 0x90F;      // pattern  ...
   unsigned long c=a | b;
   unsigned long d=a ^ b;
   
   
   cout  << hex << ( a ) << endl;
    cout  << hex << ( b ) << endl;
    cout  << hex << ( a | b ) << endl;
    cout  << hex << ( a ^ b ) << endl;
   cout  << hex << ( c ^ d ) << endl;   // prints "aaaa" pattern 1010 ...
}
void remplir(SDL_Rect *fillrect, mask *imgfill,mask *imgrend,bool setxor)// passer l image est son rect
{
    //float flx=fillrect->x/imgfill->x;
    //float fly=fillrect->y/imgfill->y;
    //float coefx=fillrect->x/imgfill->x;
    //float coefy=fillrect->y/imgfill->y;
    int ia=0;
    int somwh;int somw;int somh;
    for(int i=0;i<fillrect->h;i++){
       ia=i;
      
    for (int ii=0;ii<fillrect->w;ii++){
         
         somh=(fillrect->y+ia)*imgrend->w;
         somw=(fillrect->x+ii);
         somwh=somw+somh;
         // de yy=rect.y to (rect.y+rect.h)
        // de xx=rect.x to (rect.x + rect.w)
        // imgrend[yy*imgrend.w
        }//endi
    }//end ii
   // de x rend+ 
    //int intflx = int(flx);
    //int intfly= int(fly);
    

    //x[1] = 0;
    //x[2] = 0;
    //x[0]=1;
    // Note x[0-4]  valid 0 est à droite

    //std::cout << x << std::endl;
} //end fill fonction
//concept a collide b?
//bool matchba(auto a,auto b) 
//c= a xor b 
//d =a or b
// e = c xor d if( e!=0 ) return true;else return false

};
class rempl: public mask{
    public:
    using mask::mask;
};

class parts{
    public:
    float x,y;
    float vx,vy;
    float pv=600/k0;
    
    parts (float xx,float yy){
    x=xx;y=yy;
    vx=(rand()%WIDTH)-WIDTH/2;
    vy=(rand()%HEIGHT)-HEIGHT/2;
    //pv=3000;
        }  
   void update() {
       pv-=1;if (pv<0) pv=0;
       x+=vx*0.001*k0*sin(pv*0.002);
       y+=vy*0.001*k0*sin(pv*0.002);
       //x+=(500-1000)*0.000001;
       //y+=(500-1000)*0.000001;
       //x+=1;y+=1;     
}



};
void rien(){
int uSDL_RenderCopyEx(SDL_Renderer * renderer,
                   SDL_Texture * texture,
                   const SDL_Rect * srcrect,
                   const SDL_Rect * dstrect,
                   const double angle,
                   const SDL_Point *center,
                   const SDL_RendererFlip flip);//gRenderer, mTexture, clip, &renderQuad,
}





#endif 