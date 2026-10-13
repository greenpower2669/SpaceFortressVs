#!/usr/bin/env python3
"""Build the production SOLO offensive-bonus animation atlas.

Authored sources:
- assets/pict/solo/EnvironnementsModeSolo.png (transparent motifs)
- main:/Sprites animés mode solo.png (visual timing/reference only)

Output: assets/pict/solo/SoloBonusAnimations.png
Layout: 4 rows x 8 columns, 192x192 RGBA cells.
Rows: PRISON, GLACE, LAVE, METEORE.
"""
from pathlib import Path
from PIL import Image
import hashlib
import math

ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/"assets/pict/solo/EnvironnementsModeSolo.png"
OUTPUT=ROOT/"assets/pict/solo/SoloBonusAnimations.png"
FRAME=192
COLS=8
ROWS=4
EXPECTED_SOURCE_SHA256="77d05e4c7382ac08b1001b4b33b9fd423b494fa86e068019cfd29626509eec76"


def crop_tight(source, box, pad=4):
    image=source.crop(box)
    alpha=image.getchannel("A")
    bbox=alpha.point(lambda p: 255 if p>10 else 0).getbbox()
    if bbox:
        x0,y0,x1,y1=bbox
        x0=max(0,x0-pad);y0=max(0,y0-pad)
        x1=min(image.width,x1+pad);y1=min(image.height,y1+pad)
        image=image.crop((x0,y0,x1,y1))
    return image


def scaled(image, max_side):
    scale=max_side/max(1,max(image.width,image.height))
    return image.resize((max(1,int(round(image.width*scale))),
                         max(1,int(round(image.height*scale)))),
                        Image.Resampling.LANCZOS)


def paste_center(canvas, image, cx, cy, alpha=1.0, angle=0.0):
    obj=image
    if angle:
        obj=obj.rotate(angle,resample=Image.Resampling.BICUBIC,expand=True)
    if alpha<0.999:
        obj=obj.copy()
        obj.putalpha(obj.getchannel("A").point(lambda p:int(p*alpha)))
    canvas.alpha_composite(obj,(int(round(cx-obj.width/2)),int(round(cy-obj.height/2))))


def build():
    raw=SOURCE.read_bytes()
    actual=hashlib.sha256(raw).hexdigest()
    if actual!=EXPECTED_SOURCE_SHA256:
        raise SystemExit(f"Refuse unknown SOLO source atlas: {actual}")
    source=Image.open(SOURCE).convert("RGBA")
    if source.size!=(1122,1402):
        raise SystemExit(f"Unexpected SOLO source dimensions: {source.size}")

    rocks=[
        crop_tight(source,(13,594,195,808)),
        crop_tight(source,(184,590,332,837)),
        crop_tight(source,(334,635,481,808)),
        crop_tight(source,(607,614,748,817)),
        crop_tight(source,(740,608,873,843)),
        crop_tight(source,(880,632,1000,818)),
    ]
    ice_crystal=crop_tight(source,(162,903,306,1096))
    ice_cloud=crop_tight(source,(12,857,162,1104))
    ice_shard=crop_tight(source,(866,887,1112,1100))
    lava=[
        crop_tight(source,(13,326,168,550)),
        crop_tight(source,(175,372,320,546)),
        crop_tight(source,(325,355,451,532)),
        crop_tight(source,(454,366,613,556)),
        crop_tight(source,(611,340,737,561)),
        crop_tight(source,(740,344,876,543)),
        crop_tight(source,(873,312,1001,554)),
    ]
    smoke=[
        crop_tight(source,(21,1131,192,1364)),
        crop_tight(source,(178,1155,333,1349)),
        crop_tight(source,(336,1120,478,1374)),
        crop_tight(source,(446,1176,613,1349)),
        crop_tight(source,(573,1208,774,1364)),
        crop_tight(source,(764,1144,967,1365)),
        crop_tight(source,(972,1121,1108,1384)),
    ]

    atlas=Image.new("RGBA",(COLS*FRAME,ROWS*FRAME),(0,0,0,0))

    # PRISON: fragments fly inward then close into a rocky cage around the boss.
    for frame in range(COLS):
        t=frame/(COLS-1)
        canvas=Image.new("RGBA",(FRAME,FRAME),(0,0,0,0))
        radius=82-34*min(1.0,t*1.15)
        count=4 if frame<2 else 6
        for i in range(count):
            angle=2*math.pi*i/count+.25*(1-t)
            size=(44+18*t)*(.9+.12*((i%3)-1))
            obj=scaled(rocks[i%len(rocks)],size)
            paste_center(canvas,obj,96+math.cos(angle)*radius,96+math.sin(angle)*radius,
                         min(1.0,.55+.5*t),20*math.sin(frame+i))
        if frame>=5:
            for i in range(4):
                angle=math.pi/4+i*math.pi/2
                obj=scaled(rocks[(i+2)%len(rocks)],22+6*(frame-5))
                paste_center(canvas,obj,96+math.cos(angle)*(26-4*(frame-5)),
                             96+math.sin(angle)*(26-4*(frame-5)),.75)
        atlas.alpha_composite(canvas,(frame*FRAME,0))

    # GLACE: frost cloud spreads, crystals grow and lock around the target.
    for frame in range(COLS):
        t=frame/(COLS-1)
        canvas=Image.new("RGBA",(FRAME,FRAME),(0,0,0,0))
        cloud=scaled(ice_cloud,70+80*t)
        paste_center(canvas,cloud,96,96,.35+.35*t,frame*3)
        count=2+int(t*5)
        for i in range(count):
            angle=2*math.pi*i/max(1,count)+.3
            radius=72-22*t
            obj=scaled(ice_crystal,24+28*t*(.7+.3*(i%2)))
            paste_center(canvas,obj,96+math.cos(angle)*radius,96+math.sin(angle)*radius,
                         .65+.35*t,-30+i*55)
        if frame>=5:
            shard=scaled(ice_shard,70+12*(frame-5))
            paste_center(canvas,shard,96,96,.28+.12*(frame-5),frame*10)
        atlas.alpha_composite(canvas,(frame*FRAME,FRAME))

    # LAVE: magma projectiles converge, impact, then flare around the target.
    for frame in range(COLS):
        canvas=Image.new("RGBA",(FRAME,FRAME),(0,0,0,0))
        if frame<4:
            count=3
            radius=75-14*frame
            for i in range(count):
                obj=scaled(lava[(frame+i)%len(lava)],36+8*frame)
                angle=2*math.pi*i/count+frame*.2
                paste_center(canvas,obj,96+math.cos(angle)*radius,96+math.sin(angle)*radius,
                             .75+.06*frame,frame*7+i*13)
        else:
            obj=scaled(lava[(frame+3)%len(lava)],100+14*(frame-4))
            paste_center(canvas,obj,96,96,max(.55,1-.1*(frame-6)),frame*5)
            obj2=scaled(lava[(frame+1)%len(lava)],72+10*(frame-4))
            paste_center(canvas,obj2,96,96,.55,-frame*8)
        atlas.alpha_composite(canvas,(frame*FRAME,2*FRAME))

    # METEORE: rock arrives, impacts, then expands into the authored smoke.
    meteor_rock=rocks[2]
    for frame in range(COLS):
        canvas=Image.new("RGBA",(FRAME,FRAME),(0,0,0,0))
        if frame<=3:
            progress=frame/3
            obj=scaled(meteor_rock,48+20*progress)
            paste_center(canvas,obj,35+61*progress,30+66*progress,1.0,frame*18)
            if frame>0:
                trail=scaled(smoke[min(frame-1,len(smoke)-1)],45+10*frame)
                paste_center(canvas,trail,25+45*progress,20+45*progress,.25+.1*frame)
        else:
            si=min(frame-4,len(smoke)-1)
            cloud=scaled(smoke[si],80+20*(frame-4))
            paste_center(canvas,cloud,96,96,max(.45,1-.1*(frame-4)),(frame-4)*6)
            if frame==4:
                obj=scaled(meteor_rock,76)
                paste_center(canvas,obj,96,96,.85,70)
        atlas.alpha_composite(canvas,(frame*FRAME,3*FRAME))

    OUTPUT.parent.mkdir(parents=True,exist_ok=True)
    atlas.save(OUTPUT,optimize=True)
    print(f"{OUTPUT.relative_to(ROOT)} {atlas.size[0]}x{atlas.size[1]} sha256={hashlib.sha256(OUTPUT.read_bytes()).hexdigest()}")


if __name__=="__main__":
    build()
