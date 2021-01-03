#pragma once

#include "Types.h"
#include "Utils/Signature.hpp"
#include "Utils/Be.hpp"
#include "BinaryRes.hpp"

#define BFN_MAGIC "FONTbfn1"

namespace Zelda::Bfn
{
    struct BfnHeader
    {
        /* 0x00 */ Utils::Signature<8> magic;
        /* 0x08 */ char unk_08[4];
        /* 0x0C */ u32 blockCount;
        /* 0x10 */ char unk_10[0x10];

        void bomSwap()
        {
            BOM_SWAP(blockCount);
        }

        bool valid() const
        {
            return magic.matches(BFN_MAGIC);
        }
    };
    static_assert(sizeof(BfnHeader) == 0x20);

#define GLY1_MAGIC  "GLY1"
#define INF1_MAGIC  "INF1"
#define MAP1_MAGIC  "MAP1"
#define WID1_MAGIC  "WID1"

    struct GlyphBlock
    {
        /* 0x00 */ BlockHeader hdr;
        /* 0x08 */ u16 firstCode;
        /* 0x0A */ u16 lastCode;
        /* 0x0C */ u16 cellWidth;
        /* 0x0E */ u16 cellHeight;
        /* 0x10 */ u32 blockSize;
        /* 0x14 */ u16 fmt;
        /* 0x16 */ u16 unkWidth;
        /* 0x18 */ u16 unkHeight;
        /* 0x1A */ u16 width;
        /* 0x1C */ u16 height;
        /* 0x20 */ u8 data [];

        void bomSwap()
        {
            hdr.bomSwap();

            BOM_SWAP(firstCode);
            BOM_SWAP(lastCode);
            BOM_SWAP(cellWidth);
            BOM_SWAP(cellHeight);
            BOM_SWAP(blockSize);
            BOM_SWAP(fmt);
            BOM_SWAP(unkWidth);
            BOM_SWAP(unkHeight);
            BOM_SWAP(width);
            BOM_SWAP(height);
        }
    };
    static_assert(sizeof(GlyphBlock) == 0x20);

    struct InfoBlock
    {
        /* 0x00 */ BlockHeader hdr;
        /* 0x08 */ u16 fontType; // 0 = 1 byte; 1 = 2 byte; 2 = shiftJIS
        /* 0x0A */ u16 ascent;
        /* 0x0C */ u16 descent;
        /* 0x10 */ u16 leading;
        /* 0x12 */ char unk_12[0xE];

        void bomSwap()
        {
            hdr.bomSwap();

            BOM_SWAP(fontType);
            BOM_SWAP(ascent);
            BOM_SWAP(descent);
            BOM_SWAP(leading);
        }
    };
    static_assert(sizeof(InfoBlock) == 0x20);

    struct MapBlock
    {
        /* 0x00 */ BlockHeader hdr;
        /* 0x08 */ u16 type;
        /* 0x0A */ u16 firstChr;
        /* 0x0C */ u16 lastChr;
        /* 0x0E */ u16 codeCount;
        /* 0x10 */ u16 codes[];

        void bomSwap()
        {
            hdr.bomSwap();
            BOM_SWAP(type);
            BOM_SWAP(firstChr);
            BOM_SWAP(lastChr);
            BOM_SWAP(codeCount);
        }
    };
    static_assert(sizeof(MapBlock) == 0x10);

    /*
        Type 0 : code = c - first
        Type 1 : code = convertSjis(c, codeCount == 1 ? codes[0] : 0x31C)
        Type 2 : code = codes[c - first]
        Type 3 : ...
    */

    struct WidthBlock
    {
        /* 0x00 */ BlockHeader hdr;
        /* 0x08 */ u16 firstCode;
        /* 0x0A */ u16 lastCode;
        /* 0x0C */ u16 entries[];

        void bomSwap()
        {
            hdr.bomSwap();
            BOM_SWAP(firstCode);
            BOM_SWAP(lastCode);
        }
    };
    static_assert(sizeof(WidthBlock) == 0x0C);
}