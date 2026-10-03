#!/usr/bin/env python3
from pathlib import Path
p=Path('tests/difficulty_regressions.hpp')
text=p.read_text()
old='''        if(difficulty==0) assert(original==decorated); // No aura or reflection at level 1.\n        else {\n            size_t outside=0,unchanged=0,body=0;\n            for(int y=0;y<400;++y) for(int x=0;x<400;++x) {\n                const size_t p=y*400+x;\n                if(std::hypot(x-200,y-200)>110 && original[p]!=decorated[p]) ++outside;\n                if(std::hypot(x-200,y-200)<60) {++body;if(original[p]==decorated[p]) ++unchanged;}\n            }\n            assert(outside>100 && unchanged>body/3); // Visible signature, original central colors retained.\n        }\n'''
new='''        size_t outside=0,changedBody=0,body=0;\n        for(int y=0;y<400;++y) for(int x=0;x<400;++x) {\n            const size_t p=y*400+x;\n            if(std::hypot(x-200,y-200)>110 && original[p]!=decorated[p]) ++outside;\n            if(std::hypot(x-200,y-200)<60) {++body;if(original[p]!=decorated[p]) ++changedBody;}\n        }\n        const auto look=sfBossAppearance(encounter);\n        assert(look.bodyTint.r>=215 && look.bodyTint.g>=215 && look.bodyTint.b>=215); // RGB correction stays deliberately light.\n        if(difficulty==0) {\n            assert(look.tentacles==0 && changedBody>body/50); // Tint only; animated square mesh can extend beyond radius 110.\n        } else {\n            assert(outside>100 && changedBody>body/50); // Aura/tentacles outside + subtle body correction.\n        }\n'''
if old not in text:
    raise SystemExit('difficulty rendering assertion anchor not found')
text=text.replace(old,new,1)
text=text.replace('PASS: 200 renders, original level-1 pixels, 0/4/8/20 smooth bounded tentacles, visible aura and localized reflections',
                  'PASS: 200 renders, subtle progressive boss tint, 0/4/8/20 smooth bounded tentacles, visible aura and localized reflections',1)
p.write_text(text)
