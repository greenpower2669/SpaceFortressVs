#pragma once

#include <cstdint>
#include <functional>
#include <string>

struct SfHallUploadRequest;

struct SfHallTransport {
    std::function<void(const SfHallUploadRequest&)> submit;
    std::function<void(uint64_t,const std::string&,int)> syncPage;
};
