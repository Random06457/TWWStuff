#pragma once
#include "../Types.h"

namespace Zelda
{
    struct NameHash
    {
        static bool validHash(const char* name, u16 hash)
        {
            return computeHash(name) == hash;
        }
        static u16 computeHash(const char* name)
        {
            u16 ret = 0;
            while (*name)
                ret = ret*3 + *name++;
            return ret;
        }
    };
}