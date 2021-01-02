#pragma once

#include <vector>
#include <functional>
#include <string>
#include "Types.h"
#include "Utils/Be.hpp"
#include "Dol.hpp"

namespace Gc
{
    namespace Gcm
    {   
        struct GcmHeader
        {
            /* 0x000 */ char gameCode[6];
            /* 0x006 */ u8 diskID;
            /* 0x007 */ u8 version;
            /* 0x008 */ u8 streaming;
            /* 0x009 */ u8 streamBufSize;
            /* 0x00A */ char pad_00A[0x12];
            /* 0x01C */ u32 unk_1C;
            /* 0x020 */ char gameName[0x400-0x20];
            /* 0x400 */ u32 debugMonitorOff;
            /* 0x404 */ u32 debugMonitorAddr;
            /* 0x408 */ char pad_408[0x18];
            /* 0x420 */ u32 dolOff;
            /* 0x424 */ u32 fstOff;
            /* 0x428 */ u32 fstSize;
            /* 0x42C */ u32 fstMaxSize;
            /* 0x430 */ u32 userPosition;
            /* 0x434 */ u32 userLength;
            /* 0x438 */ char unk_438[8];

            void read(FILE* f, bool be = true);
            void bomSwap();
            void print();

        }; static_assert(sizeof(GcmHeader) == 0x440);

        struct AppLoaderHeader
        {
            /* 0x00 */ char date[0x10];
            /* 0x10 */ u32 entrypoint;
            /* 0x14 */ u32 size;
            /* 0x18 */ u32 unk_18;
            /* 0x1C */ char pad_1C[4];

            void read(FILE* f, bool be = true);
            void bomSwap();
            void print();
        };
        static_assert(sizeof(AppLoaderHeader) == 0x20);
        
        
        struct FileEntry
        {
            /* 0x00 */ u32 isDir : 8;
            /* 0x01 */ u32 nameOff : 24;
            union
            {
                struct
                {
                    /* 0x04 */ u32 parentOff;
                    /* 0x08 */ u32 nextOff;
                } dir;
                struct 
                {
                    /* 0x04 */ u32 fileOff;
                    /* 0x08 */ u32 fileSize;
                } file;
            };

            void read(FILE* f, bool be = true);
            void bomSwap();
        };
        static_assert(sizeof(FileEntry) == 0xC);
    };
}