#pragma once
#include "Types.h"
#include "Utils/Be.hpp"
#include "Utils/Signature.hpp"
#include "NameHash.hpp"
#include "BinaryRes.hpp"

#define MAGIC_BDL3  "J3D2bdl3"
#define MAGIC_BDL4  "J3D2bdl4"

namespace Zelda::Bdl
{
    struct BdlHeader
    {
        /* 0x00 */ Utils::Signature<8> magic;
        /* 0x08 */ u32 fileSize;
        /* 0x0C */ u32 resCount;
        /* 0x10 */ Utils::Signature<4> unkMagic;
        /* 0x14 */ char pad_14[0xC];

        void bomSwap()
        {
            BOM_SWAP(fileSize);
            BOM_SWAP(resCount);
        }

        inline bool valid() const
        {
            return magic.matches(MAGIC_BDL3) || magic.matches(MAGIC_BDL4);
        }
    };
    static_assert(sizeof(BdlHeader) == 0x20);

    struct ResNameTable
    {
        /* 0x00 */ u16 count;
        /* 0x02 */ u16 pad_02;
        /* 0x04 */ struct
        {
            /* 0x00 */ u16 hash;
            /* 0x02 */ u16 off;
        } entries[];

        void bomSwap()
        {
            BOM_SWAP(count);
            for (size_t i = 0; i < count; i++)
            {
                BOM_SWAP(entries[i].hash);
                BOM_SWAP(entries[i].off);
            }
        }

        inline const char* getStrTab() const
        {
            return reinterpret_cast<const char*>(entries + count);
        }
        
        inline const char* getStr(size_t idx)
        {
            return getStrTab() + entries[idx].off;
        }

        inline bool validStr(size_t idx)
        {
            return NameHash::validHash(getStr(idx), entries[idx].hash);
        }
    };
    static_assert(sizeof(ResNameTable) == 0x4);
    

    /*
        0x00 : end
        0x01 : start hierarchy
        0x02 : end hierarchy
        0x10 : ? J3DJoinTree+0x1C
        0x11 : mat
        0x12 : shape
    */
    struct HierarchyEntry
    {
        /* 0x00 */ u16 type;
        /* 0x02 */ u16 idx;
    };
    struct InfoHeader : BlockHeader
    {
        /* 0x08 */ u16 flag; // ??
        /* 0x0C */ u32 unk_0C;
        /* 0x10 */ u32 unk_10;
        /* 0x14 */ u32 hierarchyOff;
        /* 0x18 */ HierarchyEntry hierarchy[];
    };
}