#include <iostream>
#include <cmath>
//#include <fb.h>
#define ppi 3.141592653589793238462643383 
//float rt = ppi/180;
//float dt=180*ppi;
#define pi 3.141592653589793238462643383 
using namespace std;
//abreviation s pour spatial,
//
// construction d'une librérie perso
// converir des vecteurs en degrés conv360
// construire des tuple pour pouv utiliser des fonct.à retour de tuple.
// ok pour 2 d 3d def 2 et 3 termes float et vecteurs aussi
//liste de la lib:
//tupl et tuplz 2D et 3D coordonées.
//tuplv et tuplvz 2D et 3D vecteurs
// vlong longeur absolue du vecteur.
// vlongz idem mais 3d
// vtan 1 sur 2 vecteurs ramené à tang 1 et signé
// vtanxy retoure tuple de vect(2d)
//vtanz idem mais 3d
//move t1 t2 de x et y
//anglef et anglefz angle deg et force theta et phi pour la 3d
//tuplvt pour vect -1|0|1 pour usage trigo à tan=1
//vecteurs qui utilise tout les type de vecteurs v vt angle f.
//--‐-----methodes:
//--‐-‐----vt1t2 creation d un vecteur grace à move t1t2
//----------methodes set
//enti pour super class tupl tuplv anglef tuplh avec pv
// methodes:
//moveto se déplace dans enti
//goto app avec t1=t2

//utiliser les constantes dans la fablib
float rt = pi/180;
float W;float H;float WH;float D1;
void setw(float wi){W=wi;}
void seth(float hi){H=hi;}
void setwh(){WH=W*H;}//sqrtmax
void setd1(){D1=sqrt(WH);}//segment max
void setall(float wi,float hi){setw(wi);seth(hi);setwh();setd1();}
bool inab(float c,float a,float b){
   return (c-a>0 and b-c>0);
      }
bool outab(float c,float a,float b){
   return (c-a<0 or b-c<0);
      }


float invp(float longi){//inversement proportionnnel du vecteur
  //float signi;
  //if (not (longi<0))signi=1;else signi =-1;
  //marche float rep=1/longi*WH;//(longi/WH)*D1*800;//sqrt(WH)/longi;
  //rep*=signi;   
 
  float rep=((D1*2)/longi)*D1;
    return rep; }
    
float fa(float ai){//
    return sqrt(ai*ai);
}
float vlong(float va,float vaa){
    float tang=sqrt((va*va)+(vaa*vaa));  
    return tang;
}
float vlongz(float va,float vaa,float vaaa){
    float tang=sqrt((va*va)+(vaa*vaa)+(vaaa*vaaa*vaaa));  
    return tang;
}


float vtan(float va,float vaa){
    float tang=sqrt((va*va)+(vaa*vaa));
    float rep=va/tang;    
    return rep;
}// plus vtan tupl plus loin

float vtanz(float va,float vaa,float vaaa){
    float tang=sqrt((va*va)+(vaa*vaa)+(vaaa*vaaa*vaaa));
    float rep=va/tang;    
    return rep;
}
float conv360(float cx,float cy){
    //trigo en entrée
    float rep=0;
    float vx=cx;
    float vy=cy;
    float ax= acos(cx)*180/pi;
	float ay=asin(cy)*180/pi;
    if (vx>=0 and vy>=0)
    rep=ay;
    if (vx<0 and vy>=0)
    rep=ax;
    if (vx<0 and vy<0)
    rep=(ay*-1)+180;
    if (vx>=0 and vy<0)
    rep=90+(ay*1)+180+90;
    return rep;
}
float xymr(float xa, float xb,float ra,float rb){
    float cr=(xa+xb)/2;
    float racoef=rb/ra;
    if (racoef>1.4){
         cr=(xa+cr)/2;
        if  (racoef>2.5){
         cr=(xa+cr)/2;}
         }
    if (racoef<1.4){
    cr=(xb+cr)/2;
    if (racoef<2.5){
    cr=(xb+cr)/2;}
    }
    
    return cr;
}



//float dt=180*ppi;


class lim
{
   public:
   float x1;float x2;float y1;float y2;
   
  public: 
   lim(){x1=0;x2=0;y1=0;y2=0;}
   lim(float x1i,float y1i,float x2i,float y2i)
   {x1=x1i;y1=y1i;x2=x2i;y2=y2i;}
   lim(float x2i,float y2i)
   {x1=0;y1=0;x2=x2i;y2=y2i;}
    void set
    (float x1i,float y1i,float x2i,float y2i)
   {x1=x1i;y1=y1i;x2=x2i;y2=y2i;}
   void setl(lim li){x1=li.x1;y1=li.y1;x2=li.x2;y2=li.y2;}
   void setxy(float x2i,float y2i){
       x1=0;y1=0;x2=x2i;y2=y2i;  
   }
};
    
class tuplv
{
   public:
   float vx;
   float vy;
  public: 
  tuplv(){vx=0;vy=0;}
  tuplv(float vxi,float vyi){vx=vxi;vy=vyi;}
  tuplv(lim li){vx=li.x2-li.x1;vy=li.y2-li.y1;}
   void set(float vxi,float vyi){vx=vxi;vy=vyi;}
   void setl(lim li){vx=li.x2-li.x1;vy=li.y2-li.y1;}
    
}; 
class tupl
{
   
   public:
   float x;
   float y;
   lim limxy;
   
  public: 
   tupl(){x=0;y=0;}
   tupl(float xi,float yi){    
   x=xi;y=yi;
      }
      tupl(float xi,float yi,lim limi){    
   x=xi;y=yi;limxy=limi;
      }
      void setxy(float xi,float yi){
      x=xi;y=yi;
      }
    void setst(tupl si){
      x=si.x;y=si.y;
      }
      void setvt(tuplv vi){
      x+=vi.vx;y+=vi.vy;
      }
      void setl(){setll(limxy);}
      void setv(float vxi,float vyi){
      x+=vxi;y+=vyi;
      }
      void setll(lim l1l2){
      if (x<l1l2.x1)x=l1l2.x1;if (y<l1l2.y1)y=l1l2.y1;
      if (x>l1l2.x2)x=l1l2.x2;if (y>l1l2.y2)y=l1l2.y2;
      }
     void setl(float lx1i,float ly1i,float lx2i,float ly2i){
      if (x<lx1i)x=lx1i;if (y<ly1i)y=ly1i;
      if (x>lx2i)x=lx2i;if (y>ly2i)y=ly2i;
      }
    };
    tuplv vtant(float va,float vaa){
    tuplv rep=tuplv(vtan(va,vaa),vtan(vaa,va));
    return rep;//tr trig vect
}
class move
{  
   public:
   tupl xyt1;
   tupl xyt2;
  public:
  move(){xyt1.x=0,xyt1.y=0;xyt2.x=0;xyt2.y=0;}
  
    };   
class tuplz:public tupl
{
   public:
   float z;
   
  public: 
  tuplz(){z=0;}
   tuplz(float xi,float yi,float zi)
   :tupl(xi,yi){ z=zi;}
    };   
class anglef
{
   
   public:
   float angle;
   float force;
   
  public: 
   anglef(){angle=0;force=0;}
   anglef(float anglei,float forcei){
    angle=anglei;force=forcei;}
    };   
class anglefz:public anglef//acos pour z?
{
   
   public:
   float anglep;   
  public: 
   anglefz(float anglei,float anglepi,float forcei)
   :anglef(anglei,forcei){
       anglep=anglepi;
    }
    };   

class tuplvt
{
   public:
   float vxt;//t pour trigo 0-1 à tangente =1
   float vyt;//t pour trigo 0-1 à tangente =1
   float forcet;//sauvegarde du coef vectoriel 
  public: 
  tuplvt(){vxt=0;vyt=0;}
  tuplvt(float vxti,float vyti,float forceti){
    vxt=vxti;vyt=vyti;forcet=forceti;}
    
};     

class vecteurs: public tuplv, public anglef
,public tuplvt, public move
{
   public:
  
  public: 
  vecteurs(){vx=0;vy=0;}
  vecteurs(float vxi,float vxy){
       vx=vxi;
       vy=vxi;
       forcet=vlong(vx,vy);
       vxt=vtan(vx,vy);
       vyt=vtan(vy,vx);
  }
  vecteurs(tuplv vxyi){
             
       vx=vxyi.vx;
       vy=vxyi.vy;
       forcet=vlong(vx,vy);
       vxt=vtan(vx,vy);
       vyt=vtan(vy,vx);
    }
    void update(){
    }
    
    //methodes:
    void invpi(){
     force=invp(force);setai();
     }
    
 void inv(){
     vx*=-1;vy*=-1;setvi();
     }
     void mult(float fi){
     vx*=fi;vy*=fi;setvi();
     }
void setm(){//attention xt2
// sera dans le move de enti l'inclure dans le constructeur enti
        vx=xyt1.x-xyt2.x;
        vy=xyt1.y-xyt2.y;
        setv(tuplv(vx,vy));      
    }
void setvi(){setv(tuplv(vx,vy));}
void setv(tuplv metvxy){//convert all with one
    forcet=vlong(metvxy.vx,metvxy.vy);
    vxt=vtan(metvxy.vx,metvxy.vy);
    vyt=vtan(metvxy.vy,metvxy.vx);
    angle=conv360(vxt,vyt);
    force=forcet;
    xyt1.x-=vx;
    xyt1.y-=vy;
    
    }
void setai(){seta(anglef(angle,force));}
void seta(anglef meta){
    angle=meta.angle;
    force=meta.force;
    vxt=cos(meta.angle*rt);
    vyt=sin(meta.angle*rt);
    vx=vxt*meta.force;
    vy=vyt*meta.force;
    xyt1.x-=vx;
    xyt1.y-=vy;
    }
    
//void sett(tuplv mett){ dont need cause missrealtan
    
  //  }

};   

class tuplvz:public tuplv
{  
   public:
   float vz;
  public:
  tuplvz(){vz=0;vx=0;vy=0;}
  tuplvz(float vxi,float vyi,float vzi)
   :tuplv(vxi,vyi)
   { vz=vzi;}
    };   
class tuplwh
{
   public:
   float w;   float h; float miw; float mih;
   std::string forme;
  public: 
  tuplwh(){}
   tuplwh(float wi,float hi)
   {forme="r";mih=hi/2;miw=wi/2;
   w=wi;h=hi; }
   tuplwh(float wi,float hi,string forme)
   {mih=hi/2;miw=wi/2;w=wi;h=hi; }
};
class tuplwhp: public tuplwh
{
   public:
   float p;  float mip;
  public: 
  tuplwhp(){h=0;w=0;}
  tuplwhp (float wi,float hi,float pii)
  :tuplwh(wi,hi)
   {forme="r";mih=hi/2;miw=wi/2;
   w=wi;h=hi; p=pii;mip=pii/2;}
   tuplwhp(float wi,float hi,float pii,string formei)
   :tuplwh(wi,hi)
   {mih=hi/2;miw=wi/2;mip=pii/2;p=pii; forme=formei;}
};   

tuplv trig(tuplv vxy){
    vxy.vx++;vxy.vy++;
    return vxy;
}
       
tupl plus1(tupl xy){
    xy.x++;xy.y++;
    return xy;
}

class entitest
{
   public:
   tupl xy;
   vecteurs v;
   
   
  public: 
  entitest(tupl xy,vecteurs v){
      
      
      
  }
 };   
   
class frm
{
    public:
    float fcurs,incr;
    //incr par updates steps = taille des sequ
    int curs;int max;int sens;int steps;//sens 0 1 -1
    int type;//0 cycle //1 vas et v
                 //2 steps  raz// 3 steps vv //4 neutre
     frm(){
                     curs=0;
                     fcurs=0.01;
                     type=1;
                     max=100;
                     sens=1;
                     incr=0.01;
                     
                    
                     }
    frm(int icurs,int imax,int isens,int isteps,int iincr,int itype){
        curs=icurs;
        max=imax;
        sens=isens;
        incr=iincr;steps=isteps;
        type=itype;
        }
        void set360(float iincr,int isens){
            incr=iincr;sens=isens;
            max=360;
            
            }
            void go(float igo){
                    fcurs=igo+0.001;curs=int(igo);sens=1;
                        }
         void update(){
             if (type!=4){
                 fcurs+=incr*sens;
                  if (fcurs!=curs and fcurs==int(fcurs))
                     curs=int(fcurs);
                 switch(type) {
                     case 0: {
                          if (curs>max)
                          {curs=0;fcurs=0;}
                          break;
                         }
                     case 1:{                                   
                         if (curs!=fcurs and (curs==0 or curs==max))
                         sens*=-1;
                         break;
                          }
                     case 2:{ 
                         if (curs>steps and curs/steps==int(curs/steps))
                                   {}
                                   else                       
                         if (curs!=fcurs and (curs==0 or curs==max))
                             sens*=-1;
                             break;
                              }
                          
                 }}}//void if switch dif 4
    }; 
class spr
{
    public:
    tupl xy; tuplwh wh;frm frames;
    float angle;
    std::string sprid;
    std::string type;
    //start with r1-9 for rotation changing
    // l1-9 for layer1-9  not in array
    //s1-9 fo size changing
    // a1-9 anime steps va et viens
    spr(){
       xy.y=0; xy.x=0;
       
       }
    spr(tupl ixy,tuplwh iwh, frm iframes,float iangle,std::string isprid)
{
    xy=ixy; wh=iwh;frames=iframes;
    angle=iangle;
    sprid=isprid;
}

    
};
class enti
{

public:
float frot=0.990;
std::string name;
std::string info;
tupl xy;
vecteurs v;
tuplwh wh;
tupl midxy;//xy-mid
bool bouncex=false;
bool bouncey=false;

float pv;

bool ctrl;
enti()
{xy.x=0;xy.y=0;v.vx=0;v.vy=0;
wh.w=0;wh.h=0;pv=1;}
enti(float xi, float yi, float vxi, float vyi,
float wi,float hi,float pvi)
{xy.x=xi;xy.y=yi;v.vx=vxi;v.vy=vyi;
wh.w=wi;wh.h=hi;pv=pvi;}
enti(float xi,float yi) {xy.x=xi;xy.y=yi;pv=1;}
enti(tupl xyi, vecteurs vxyi, tuplwh whi,float pvi)
{xy=xyi;v=vxyi;
wh=whi;pv=pvi;}
void move(tupl desti){
    v.xyt1=v.xyt2;v.xyt2=desti;xy=desti;
    v.setm();
    }
void go(tupl desti){xy=desti;}
void azim(float xi,float yi){v.vx=xi-xy.x;
v.vy=yi-xy.y;
v.setvi();}
void azimm(float xi,float yi,float fi){
    v.vx=fi*(xi-xy.x);
    v.vy=fi*(yi-xy.y);
;v.setvi();}
void azimmix(float xi,float yi,float fi){
    v.vx+=(fi*(xi-xy.x))/2;
    v.vy+=(fi*(yi-xy.y))/2;
;v.setvi();}
void dep(){
    xy.x+=v.vxt*v.force;xy.y+=v.vyt*v.force;
    move(xy);
    
}
void update(){
    float verifx;
    float verify;
    //allcase start
    
    //allcase end
    if (not ctrl){
        	
        xy.x+=v.vxt*0.1*v.force*0.1;
		xy.y+=v.vyt*0.1*v.force*0.1;
		
		verify=xy.y;verifx=xy.x;
		
		xy.setl();
		if (not bouncex and verifx!=xy.x){
		   	bouncex = true;
		   	v.vx*=-1;
	    	}
	    	if (bouncex and not (verifx!=xy.x)) bouncex = false;

		if (not bouncey and verify!=xy.y){
		   	bouncey = true;
		   	v.vy*=-1;
	    	}
	    	if (bouncey and not (verify!=xy.y)) bouncey = false;
		
		if (bouncey or bouncex)v.setvi();
		v.force*=frot;v.setai();
	
		//xy.setl(0,0,700,1600);
        }//endunctrl
    else{}//end ctrl
 }//end of void update
 
 void seta(){v.seta(anglef(v.angle,v.force));}
};
anglefz conv360p(float cx,float cy,float cz){
    //trigo en entrée
    float rep=0;
    float rep2=0;
    float vx=vtan(cx,cy);
    float vy=vtan(cy,cx);
    //float vz=cz;
    //
    float vxy=vlong(cx,cy);//non signé mais le sera après.
    float txy=vtan(vxy,cz);
    float pz=vtan(cz,vxy);
    float ap= asin(pz)*180/ppi;//phi puis theta sera construit après le signe de vxy
    
    float ax= acos(vx)*180/ppi;
	float ay=asin(vy)*180/ppi;
    if (vx>=0 and vy>=0)
    rep=ay;
    if (vx<0 and vy>=0)
    rep=ax;
    if (vx<0 and vy<0)
    rep=(ay*-1)+180;
    if (vx>=0 and vy<0)
    rep=90+(ay*1)+180+90;
    //signer txy car n'en avait pas comme vxy
    if ((rep<270)and(rep>90)) txy*=-1;
    float at= acos(txy)*180/ppi;//theta après le signe
    //phi start 
       if (txy>=0 and pz>=0)
    rep2=ap;
    if (txy<0 and pz>=0)
    rep2=at;
    if (txy<0 and pz<0)
    rep2=(ap*-1)+180;
    if (txy>=0 and pz<0)
    rep2=90+(ap*1)+180+90;
    //phi end
    anglefz rep3=anglefz(rep,1,rep2);
    
    return rep3;
}
tuplv creatuplv(tupl a,tupl b){
    tuplv r=tuplv(a.x-b.x,a.y-b.y);
    return r;
}