#!/usr/bin/env python3
"""Check that only authorized SOLO-025 private audio gets packaged into APK."""
import argparse,hashlib,zipfile
NAMES = (
 'Meteor_explosion_rock_contre_rock1_001.wav',
 'Meteor_explosion_rock_contre_rock2_002.wav',
 'Meteor_explosion_rock_contre_rock3_003.wav',
 'Meteor_explosion_rock_contre_rock4_004.wav',
)
HASHES = (
 '98d08994f89f1f246158b74f4b1d0dc5d9edce79134d6d1a109bef3193b895d0',
 '2e4bec14e47c1cdaf657ce1b0d9a2a0faab99f89e34d2fc6d688243a7111bab4',
 'b69ce378105355f2357e29b16f86aa2b7b776bf2bab9202e142401aca058aab3',
 'aa68017c13662e71666473e243151463042d44d78d7fcdb11772524bd00b4de4',
)
def verify(apk,expect):
    with zipfile.ZipFile(apk) as z:
        media=[x for x in z.namelist() if x.startswith('assets/') and
               'resources/assets/sounds/Meteor_explosion_rock_contre_rock' in x]
        assert len(media)==expect,(len(media),expect)
        if expect:
            assert len(media)==4
            for name,digest in zip(NAMES,HASHES):
                found=[entry for entry in media if entry.endswith('/'+name)]
                assert len(found)==1,(name,found)
                assert hashlib.sha256(z.read(found[0])).hexdigest()==digest,name
    print(f'SOLO-025: APK contains {expect}/4 verified authorized meteor sounds')
if __name__=='__main__':
    p=argparse.ArgumentParser()
    p.add_argument('--apk',required=True)
    p.add_argument('--expect',required=True,type=int,choices=[0,4])
    a=p.parse_args();verify(a.apk,a.expect)
