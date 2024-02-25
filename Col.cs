#include <cmath>
extern bool runed;
//using namespace sf;
class tests {
    public:
    bool rep;
    float x,y,z,xc,yc,zc;
    int da,db;
    float r1,r2,r3,r4,r5,r6,r7,r8,tan;
    int signx,signy;
    
    
    tests (){
        
    }
    public:
    int insqr(int a,int b,int c, int d){
        x=a;y=b;xc=c,yc=d;
        if ((x<xc and x>0) or (y<yc and y>0)) return true;
        else return false;
        
    }//sqr
    int inlng(int a,int b,int c){// buguée
        x=a;xc=b;da=c;
       r1=x-xc;
        r2=sqrt(r1*r1);  
        if (r2<da) return true;
        else return false;
    }//inlng
    
    int outlng(int a,int b,int c){
        x=a;xc=b;da=c;
        r1=x-xc;
        r2=sqrt(r1*r1);
        if (x-xc<0+da*0.5 and x-xc>0-da*0.5) return true;
        else return false;
        
    }//outlng
    
 bool intan(float a,float b,float c,float d,float ntan){
        x=a;y=b;xc=c;yc=d;
        r1=x-xc;
        r2=y-yc;
        if (r1>0) signx=1;
        else if (r1<0) signx=-1;
        if (r2>0) signy=1;
        else if (r2<0) signy=-1;
        r3=sqrt(r1*r1);
        r4=sqrt(r2*r2);
        r5=sqrt((r1+r2)*(r1+r2));
        r6=r3/r5*signx;
        r7=r4/r5*signy;
        
        return (r5<ntan);
    }
    
    
    int v2dtan(float a,float b,float c,float d){
        x=a;y=b;xc=c;yc=d;
        r1=x-xc;
        r2=y-yc;
        if (r1>0) signx=1;
        else if (r1<0) signx=-1;
        if (r2>0) signy=1;
        else if (r2<0) signy=-1;
        r3=sqrt(r1*r1);
        r4=sqrt(r2*r2);
        r5=sqrt((r1+r2)*(r1+r2));
        r6=r5/r3*signx;
        r7=r5/r4*signy;
        
        return r5;
    }
    int v2dx(float a,float b,float c,float d){
        x=a;y=b;xc=c;yc=d;
        r1=x-xc;
        r2=y-yc;
        if (r1>0) signx=1;
        else if (r1<0) signx=-1;
        if (r2>0) signy=1;
        else if (r2<0) signy=-1;
        r3=sqrt(r1*r1);
        r4=sqrt(r2*r2);
        r5=sqrt((r1+r2)*(r1+r2));
        r6=(r5-r1)/r5*signx;
        r7=(r5-r2)/r5*signy;
        
        return r6;          
    }//v2d
    int v2dy(float a,float b,float c,float d){
        x=a;y=b;xc=c;yc=d;
        r1=x-xc;
        r2=y-yc;
        if (r1>0) signx=1;
        else if (r1<0) signx=-1;
        if (r2>0) signy=1;
        else if (r2<0) signy=-1;
        r3=sqrt(r1*r1);
        r4=sqrt(r2*r2);
        r5=sqrt((r1+r2)*(r1+r2));
        r6=(r5-r1)/r5*signx;
        r7=(r5-r1)/r5*signy;
        
        return r7;
        }          
    
};
// notes
     //if (x>WIDTH-100 or x<0+100) outx=true;
     //if (y>HEIGHT-100 or y<0+100) outy=true;
        
