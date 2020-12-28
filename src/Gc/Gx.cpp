#include <assert.h>
#include "Gx.hpp"

#define GX_TF_I4            0x0
#define GX_TF_I8            0x1
#define GX_TF_IA4           0x2
#define GX_TF_IA8           0x3
#define GX_TF_RGB565        0x4
#define GX_TF_RGB5A3        0x5
#define GX_TF_RGBA8         0x6
#define GX_TF_CI4           0x8
#define GX_TF_CI8           0x9
#define GX_TF_CI14          0xa
#define GX_TF_CMPR          0xE         /*!< Compressed */

#define GX_TL_IA8           0x00
#define GX_TL_RGB565        0x01
#define GX_TL_RGB5A3        0x02

#define CASE_TF(fmt) case GX_TF_##fmt: return #fmt;
#define CASE_TL(fmt) case GX_TL_##fmt: return #fmt;

namespace Gc::Gx
{
    const char* getTexFmtName(u8 fmt)
    {
        switch (fmt)
        {
            CASE_TF(I4);
            CASE_TF(I8);
            CASE_TF(IA4);
            CASE_TF(IA8);
            CASE_TF(RGB565);
            CASE_TF(RGB5A3);
            CASE_TF(RGBA8);
            CASE_TF(CI4);
            CASE_TF(CI8);
            CASE_TF(CI14);
            CASE_TF(CMPR);
        
            default: return nullptr;
        }
    }

    const char* getTlutFmtName(u8 fmt)
    {
        switch (fmt)
        {
            CASE_TL(IA8);
            CASE_TL(RGB565);
            CASE_TL(RGB5A3);
            default: return nullptr;
        }
    }
}