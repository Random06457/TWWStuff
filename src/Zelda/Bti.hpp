#pragma once

#include "Types.h"
#include "Utils/Be.hpp"

namespace Zelda::Bti
{
    struct BtiHeader
    {
        /* 0x00 */ u8 fmt;
        /* 0x01 */ u8 transparency;
        /* 0x02 */ u16 width;
        /* 0x04 */ u16 height;
        /* 0x06 */ u8 wrapS;
        /* 0x07 */ u8 wrapT;
        /* 0x08 */ u8 unk_8;
        /* 0x09 */ u8 tlutFmt;
        /* 0x0A */ u16 tlutEntries;
        /* 0x0C */ u32 tlutOff;
        /* 0x10 */ char unk_10[8];
        /* 0x18 */ u8 mipmaps; // ?
        /* 0x1A */ u16 unk_1A;
        /* 0x1C */ u32 texOff;

        void bomSwap()
        {
            BOM_SWAP(width);
            BOM_SWAP(height);
            BOM_SWAP(tlutEntries);
            BOM_SWAP(tlutOff);
            BOM_SWAP(unk_1A);
            BOM_SWAP(texOff);
        }
    };
    static_assert(sizeof(BtiHeader) == 0x20);
    
}