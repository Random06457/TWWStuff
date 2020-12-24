#pragma once
#include <string>
#include "../Types.h"
#include "../Utils/Be.hpp"

#define RARC_MAGIC  (0x42515243)


namespace Zelda
{
    struct ArcName
    {
        bool validateHash(std::string name, u16 hash) const
        {
            return computeHash(name) == hash;
        }
        u16 computeHash(std::string name) const
        {
            u16 ret = 0;
            for (auto c : name)
                ret = ret*3 + c;
            return ret;
        }
    };

    struct ArcHeader
    {
        /* 0x00 */ u32 magic;
        /* 0x04 */ u32 fileSize;
        /* 0x08 */ u32 headerSize;
        /* 0x0C */ u32 unk_0C;

        void bomSwap()
        {
            BOM_SWAP(magic);
            BOM_SWAP(fileSize);
            BOM_SWAP(headerSize);
            BOM_SWAP(unk_0C);
        }

        bool validate() const
        {
            return magic == RARC_MAGIC;
        }
    };
    static_assert(sizeof(ArcHeader) == 0x10);
    

    struct ArcHeader2
    {
        /* 0x00 */ u32 resCount;
        /* 0x04 */ u32 resOff;
        /* 0x08 */ u32 fileCount;
        /* 0x0C */ u32 fileOff;
        /* 0x10 */ u32 strTabSize;
        /* 0x14 */ u32 strTabOff;

        void bomSwap()
        {
            BOM_SWAP(resCount);
            BOM_SWAP(resOff);
            BOM_SWAP(fileCount);
            BOM_SWAP(fileOff);
            BOM_SWAP(strTabSize);
            BOM_SWAP(strTabOff);
        }
    };

    struct ArcResEntry
    {
        /* 0x00 */ char resType[4];//u32 resType;
        /* 0x04 */ u32 name;
        /* 0x08 */ u16 nameHash;
        /* 0x0A */ u16 fileCount;
        /* 0x0C */ u32 firstFileIdx;
        
        void bomSwap()
        {
            //BOM_SWAP(resType);
            BOM_SWAP(name);
            BOM_SWAP(nameHash);
            BOM_SWAP(fileCount);
            BOM_SWAP(firstFileIdx);
        }
    };
    static_assert(sizeof(ArcResEntry) == 0x10);

#define ATTR_DIR    (1<<1)

    struct ArcFileEntry
    {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 nameHash;
        /* 0x04 */ u32 attr : 8;
        /* 0x05 */ u32 name : 24;
        /* 0x08 */ union
        {
            s32 parent;
            u32 fileOff;
        };
        /* 0x0C */ u32 fileSize;
        /* 0x10 */ u32 unk_10;

        void bomSwap()
        {
            BOM_SWAP(unk_00);
            BOM_SWAP(nameHash);
            Utils::Be::bomSwap(reinterpret_cast<char*>(this)+5, 3);
            BOM_SWAP(parent);
            BOM_SWAP(fileSize);
            BOM_SWAP(unk_10);
        }
    };
    static_assert(sizeof(ArcFileEntry) == 0x14);
}