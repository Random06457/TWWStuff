#pragma once

#include "../Types.h"
#include "../Utils/Be.hpp"

#define TEXT_SECTION_COUNT  7
#define DATA_SECTION_COUNT  11
#define SECTION_COUNT       (TEXT_SECTION_COUNT+DATA_SECTION_COUNT)

namespace Gc::Dol
{
    struct DolHeader
    {
        /* 0x00 */ u32 sectionOff[SECTION_COUNT];
        /* 0x48 */ u32 sectionAddr[SECTION_COUNT];
        /* 0x90 */ u32 sectionSize[SECTION_COUNT];
        /* 0xD8 */ u32 bssAddr;
        /* 0xDC */ u32 bssSize;
        /* 0xE0 */ u32 entrypoint;
        /* 0xE4 */ char pad_E4[0x1C];

        void read(FILE* f)
        {
            fread(this, 1, sizeof(DolHeader), f);
            bomSwap();
        }

        void bomSwap()
        {
            for (size_t i = 0; i < SECTION_COUNT; i++)
            {
                BOM_SWAP(sectionOff[i]);
                BOM_SWAP(sectionAddr[i]);
                BOM_SWAP(sectionSize[i]);
            }
            BOM_SWAP(bssAddr);
            BOM_SWAP(bssSize);
            BOM_SWAP(entrypoint);
        }

        size_t getDolSize() const;


        void print();
    };
    static_assert(sizeof(DolHeader) == 0x100);
}