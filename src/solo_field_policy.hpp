#pragma once
// Bound only while the SOLO engine is ticking. Historical modes use defaults.
struct SfSoloFieldPolicy {
    bool enabled=false,maze=false;
    void *context=nullptr;
    tupl (*resolve)(void*,tupl,tupl,float)=nullptr;
    bool (*occupy)(void*,tupl,float)=nullptr;
    float (*sweep)(void*,tupl,tupl,float)=nullptr;
    bool (*audible)(void*,tupl)=nullptr;
};
inline SfSoloFieldPolicy sfSoloField;
struct SfSoloFieldScope {
    SfSoloFieldPolicy previous;
    explicit SfSoloFieldScope(SfSoloFieldPolicy next):previous(sfSoloField){sfSoloField=next;}
    ~SfSoloFieldScope(){sfSoloField=previous;}
};
static bool sfFieldAudible(float x,float y){
    return !sfSoloField.enabled || !sfSoloField.audible || sfSoloField.audible(sfSoloField.context,tupl(x,y));
}
