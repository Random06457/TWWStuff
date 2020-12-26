#pragma once
#include "../Types.h"
#include "../Utils/Be.hpp"

namespace Gc::Rel
{
    struct SectionEntry
    {
        /* 0x00 */ u32 off;
        /* 0x04 */ u32 size;

        void bomSwap()
        {
            BOM_SWAP(off);
            BOM_SWAP(size);
        }

        inline u32 getOff() const
        {
            return off & ~1;
        }
        inline bool isExec() const
        {
            return off & 1;
        }
    };
    static_assert(sizeof(SectionEntry) == 0x8);

    struct ImportEntry
    {
        /* 0x00 */ u32 id;
        /* 0x04 */ u32 off;

        void bomSwap()
        {
            BOM_SWAP(id);
            BOM_SWAP(off);
        }
    };
    static_assert(sizeof(ImportEntry) == 0x8);

    struct RelocEntry
    {
        /* 0x00 */ u16 off;
        /* 0x02 */ u8 type;
        /* 0x03 */ u8 section;
        /* 0x04 */ u32 addend;

        void bomSwap()
        {
            BOM_SWAP(off);
            BOM_SWAP(addend);
        }
    };
    static_assert(sizeof(RelocEntry) == 0x8);

    struct RelHeader
    {
        /* 0x00 */ u32 id;
        /* 0x04 */ char link[8];
        /* 0x0C */ u32 sectionCount;
        /* 0x10 */ u32 sectionsOff;
        /* 0x14 */ u32 nameOff;
        /* 0x18 */ u32 nameSize;
        /* 0x1C */ u32 version;
        /* 0x20 */ u32 bssSize;
        /* 0x24 */ u32 relOff;
        /* 0x28 */ u32 importOff;
        /* 0x2C */ u32 importSize;
        /* 0x30 */ u8 initArraySection;
        /* 0x31 */ u8 finiArraySection;
        /* 0x32 */ u8 unresolvedSection;
        /* 0x33 */ u8 bssSection;
        /* 0x34 */ u32 initFunc;
        /* 0x38 */ u32 finiFunc;
        /* 0x3C */ u32 unresolved;
        /* 0x40 */ u32 align;
        /* 0x44 */ u32 bssAlign;
        /* 0x48 */ u32 fixSize;

        void bomSwap()
        {
            BOM_SWAP(id);
            BOM_SWAP(sectionCount);
            BOM_SWAP(sectionsOff);
            BOM_SWAP(nameOff);
            BOM_SWAP(nameSize);
            BOM_SWAP(version);

            BOM_SWAP(bssSize);
            BOM_SWAP(relOff);
            BOM_SWAP(importOff);
            BOM_SWAP(importSize);

            BOM_SWAP(initFunc);
            BOM_SWAP(finiFunc);
            BOM_SWAP(unresolved);
            BOM_SWAP(align);
            BOM_SWAP(bssAlign);
            BOM_SWAP(fixSize);
        }
    };
    static_assert(sizeof(RelHeader) == 0x4C);
}