#!/usr/bin/env python3
"""SOLO-023: deterministic sprite atlas, 16 connected N/E/S/W shapes x 3 variants.
Each pixel's alpha is 0 or 255 for stable SDL foreground redraw.
No pip dependencies, no edits to any existing image.
"""
import argparse, math, pathlib, struct, zlib

S=128; COLS=12; ROWS=4; WIDTH=COLS*S; HEIGHT=ROWS*S
N,E,SO,W=1,2,4,8

def hash01(x,y,seed):
    v=(x*374761393+y*668265263+seed*1442695041)&0xffffffff
    v^=v>>13;v=(v*1274126177)&0xffffffff;v^=v>>16
    return v/4294967295.0

def noise(x,y,scale,seed):
    a=x/scale;b=y/scale;i=math.floor(a);j=math.floor(b)
    u=a-i;v=b-j;u=u*u*(3-2*u);v=v*v*(3-2*v)
    x0=hash01(i,j,seed);x1=hash01(i+1,j,seed)
    y0=hash01(i,j+1,seed);y1=hash01(i+1,j+1,seed)
    return (x0+(x1-x0)*u)*(1-v)+(y0+(y1-y0)*u)*v

def pixel(mask,variant,x,y):
    xx=(x+.5)/S;yy=(y+.5)/S
    wx=math.sin(2*math.pi*2*xx+variant*.47)+.42*math.sin(2*math.pi*5*xx-variant*.17)
    wy=math.sin(2*math.pi*2*yy+variant*.43)+.42*math.sin(2*math.pi*5*yy+variant*.13)
    f=(noise(x,y,14,variant+31)-.5)*5
    dn=y-(8.5+3.9*wx+f);de=S-1-x-(8.5+3.9*wy+f)
    ds=S-1-y-(8.5+3.5*wx-f);dw=x-(8.5+3.5*wy-f)
    exposed=[]
    for bit,d,sign in ((N,dn,1),(E,de,-1),(SO,ds,-1),(W,dw,1)):
        if not(mask&bit):exposed.append((d,sign))
    if exposed and min(d for d,_ in exposed)<0:return (0,0,0,0)
    radius=21+2*variant
    for (px,py),bits in (((x,y),N|W),((S-1-x,y),N|E),
                          ((x,S-1-y),SO|W),((S-1-x,S-1-y),SO|E)):
        if not (mask&bits) and px<radius and py<radius:
            limit=radius+2.5*noise(x,y,9,variant+43)-1.25
            if (px-radius)**2+(py-radius)**2>=limit*limit:return (0,0,0,0)
    broad=noise(x+variant*31,y+variant*17,34,17)-.5
    fine=noise(x+variant*41,y+variant*13,8,23)-.5
    grain=hash01(x//2,y//2,variant+9)-.5
    light=145+47*broad+31*fine+15*grain
    if exposed:
        d,side=min(exposed)
        if d<17:
            light+=math.exp(-max(0,d)/7)*(49 if side>0 else -34)
            light+=(noise(x,y,6,39+variant)-.5)*22
    if hash01(x//3,y//3,variant+55)>.94:light+=16
    if hash01(x//4,y//4,variant+61)>.965:light-=24
    c=max(65,min(233,int(light)))
    return c,max(0,c-2),min(255,c+5),255

def chunk(name,data):
    return struct.pack('!I',len(data))+name+data+struct.pack('!I',zlib.crc32(name+data)&0xffffffff)

def main(output):
    pixels=bytearray(WIDTH*HEIGHT*4)
    for variant in range(3):
        for mask in range(16):
            col=variant*4+(mask%4);row=mask//4
            for y in range(S):
                for x in range(S):
                    offset=((row*S+y)*WIDTH+col*S+x)*4
                    pixels[offset:offset+4]=bytes(pixel(mask,variant,x,y))
    raw=b''.join(b'\x00'+pixels[y*WIDTH*4:(y+1)*WIDTH*4] for y in range(HEIGHT))
    data=(b'\x89PNG\r\n\x1a\n'+chunk(b'IHDR',struct.pack('!2I5B',WIDTH,HEIGHT,8,6,0,0,0))
          +chunk(b'IDAT',zlib.compress(raw,9))+chunk(b'IEND',b''))
    out=pathlib.Path(output);out.parent.mkdir(parents=True,exist_ok=True)
    out.write_bytes(data)
    print(f'SOLO-023 edge atlas: {out} ({WIDTH}x{HEIGHT}, 48 sprites, {len(data)} bytes)')

if __name__=='__main__':
    parser=argparse.ArgumentParser()
    parser.add_argument('--output',default='assets/pict/solo/HabillageRebordsModeSolo.png')
    main(parser.parse_args().output)
