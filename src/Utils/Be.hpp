#pragma once

#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <vector>
#include "Types.h"

#define BOM_SWAP(field) Utils::Be::bomSwap(&field, sizeof(field))

#define BS16(x) static_cast<u16>((((x) & 0xFF) << 8) | (((x) & 0xFF00) >> 8))
#define BS32(x) static_cast<u32>((((x) & 0xFF) << 24) | (((x) & 0xFF000000) >> 24) | (((x) & 0xFF0000) >> 8) | (((x) & 0xFF00) << 8))

namespace Utils::Be
{
    inline void bomSwap(void* ptr, size_t count)
    {
        u8* in = reinterpret_cast<u8*>(ptr);
        for (size_t i = 0; i < count/2; i++)
            std::swap(in[i], in[count-1-i]);
    }
    

    template<typename T>
    T bomSwap(T t)
    {
        T ret = t;
        u8* in = reinterpret_cast<u8*>(&t);
        u8* out = reinterpret_cast<u8*>(&ret);
        for (size_t i = 0; i < sizeof(T); i++)
            out[i] = in[sizeof(T)-1-i];
        
        return ret;
    }

    template<>
    inline u16 bomSwap<u16>(u16 t) { return BS16(t); }
    template<>
    inline u32 bomSwap<u32>(u32 t) { return BS32(t); }
}