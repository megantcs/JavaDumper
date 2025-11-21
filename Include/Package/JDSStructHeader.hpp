#pragma once
#include <cstdint>

enum PackageAction {
    None = 0,
    ReadStepBytes = 1,
    ReadStepString = 2,
};

#pragma pack(push, 1)
struct PHeader
{
    char name[64];
    char version[16];
    uint32_t cid;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PackageHeader
{
    uint32_t size;
    uint8_t action;
};
#pragma pack(pop)





