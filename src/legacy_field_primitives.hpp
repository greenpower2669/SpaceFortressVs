#pragma once
// Fab's historical field primitives, shared by duel and campaign.
// Visibility uses arena W rather than the last loaded texture width.

bool colee(sprite *a,sprite *b)
{
  float r1=2*0.85;
  return ((b->x - a->x)*(b->x - a->x)+
         (b->y - a->y)*(b->y - a->y))<
         ((a->sh + b->sh)/r1*(a->sh + b->sh)/r1)/r1;

  //return ((b->xm - a->xm)*(b->xm - a->xm)+
        // (b->ym - a->ym)*(b->ym - a->ym))<
       //  ((a->sh + b->sh)/r1*(a->sh + b->sh)/r1)/r1;
}

bool inxy(sprite *a)
{
  float r1=2*1.01;
  return (a->x>0-W*0.2 and a->x<W*1.2) and
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
