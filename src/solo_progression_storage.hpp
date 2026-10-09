#pragma once
// SOLO progression disk persistence. Explicitly scoped to SOLO; never overwrites
// historical campaign saves. Atomic replacement avoids partially written saves.
#include "solo_progression.hpp"
#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>

namespace sfsolo {
inline bool loadProgression(const std::string &path,Progression &out){
    if(path.empty())return false;
    std::ifstream input(path,std::ios::binary);
    if(!input)return false;
    std::string serialized((std::istreambuf_iterator<char>(input)),
                            std::istreambuf_iterator<char>());
    if(input.bad() || serialized.size()>256)return false;
    if(!serialized.empty() && serialized.back()=='\n')serialized.pop_back();
    Progression candidate;
    if(!candidate.deserialize(serialized))return false;
    out=candidate;
    return true;
}
inline bool saveProgression(const std::string &path,const Progression &value){
    if(path.empty())return false;
    const std::string temporary=path+".tmp";
    {
        std::ofstream output(temporary,std::ios::binary|std::ios::trunc);
        if(!output)return false;
        const std::string encoded=value.serialize()+"\n";
        output.write(encoded.data(),std::streamsize(encoded.size()));
        output.flush();
        if(!output.good()){
            output.close();
            std::remove(temporary.c_str());
            return false;
        }
        output.close();
        if(output.fail()){std::remove(temporary.c_str());return false;}
    }
    if(std::rename(temporary.c_str(),path.c_str())!=0){
        std::remove(temporary.c_str());
        return false;
    }
    return true;
}
} // namespace sfsolo
