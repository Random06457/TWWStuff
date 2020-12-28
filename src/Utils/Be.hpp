#pragma once

#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <vector>
#include "../Types.h"


#define SWAP_TBL static const Utils::Be::BeSwapTblEntry beSwapTbl[]
#define DEFINE_SWAP_TBL(type, ...) \
    std::vector<Utils::Be::BeSwapTblEntry> g_##type##BeSwapTbl = \
            { \
                __VA_ARGS__ \
            };
#define SWAP_TBL_ENTRY(type, field) { offsetof(type, field), sizeof(type::field) }

#define BOM_SWAP(field) Utils::Be::bomSwap(&field, sizeof(field))

#define BS16(x) static_cast<u16>((((x) & 0xFF) << 8) | (((x) & 0xFF00) >> 8))
#define BS32(x) static_cast<u32>((((x) & 0xFF) << 24) | (((x) & 0xFF000000) >> 24) | (((x) & 0xFF0000) >> 8) | (((x) & 0xFF00) << 8))

namespace Utils
{
    class Be
    {
    public:
        struct BeSwapTblEntry
        {
            u32 offset;
            u32 size;
        };

        static void bomSwapStruct(void* data, std::vector<BeSwapTblEntry> entries)
        {
            for (size_t i = 0; i < entries.size(); i++)
                Be::bomSwap(reinterpret_cast<char*>(data) + entries[i].offset, entries[i].size);
        }

        static void bomSwap(void* ptr, size_t count)
        {
            u8* in = reinterpret_cast<u8*>(ptr);
            for (size_t i = 0; i < count/2; i++)
                std::swap(in[i], in[count-1-i]);
        }
        

        template<typename T>
        static T bomSwap(T t)
        {
            T ret = t;
            u8* in = reinterpret_cast<u8*>(&t);
            u8* out = reinterpret_cast<u8*>(&ret);
            for (size_t i = 0; i < sizeof(T); i++)
                out[i] = in[sizeof(T)-1-i];
            
            return ret;
        }
    };
}