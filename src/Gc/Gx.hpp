#pragma once

#include "../Types.h"

// vtxattr
#define GX_NONE             0           /*!< Input data is not used */
#define GX_DIRECT           1           /*!< Input data is set direct */
#define GX_INDEX8           2           /*!< Input data is set by a 8bit index */
#define GX_INDEX16          3           /*!< Input data is set by a 16bit index */

// comptype
#define GX_POS_XY           0           /*!< X,Y position */
#define GX_POS_XYZ          1           /*!< X,Y,Z position */
#define GX_NRM_XYZ          0           /*!< X,Y,Z normal */
#define GX_NRM_NBT          1
#define GX_NRM_NBT3         2
#define GX_CLR_RGB          0           /*!< RGB color */
#define GX_CLR_RGBA         1           /*!< RGBA color */
#define GX_TEX_S            0           /*!< One texture dimension */
#define GX_TEX_ST           1           /*!< Two texture dimensions */

// compsize
#define GX_U8               0           /*!< Unsigned 8-bit integer */
#define GX_S8               1           /*!< Signed 8-bit integer */
#define GX_U16              2           /*!< Unsigned 16-bit integer */
#define GX_S16              3           /*!< Signed 16-bit integer */
#define GX_F32              4           /*!< 32-bit floating-point */
#define GX_RGB565           0           /*!< 16-bit RGB */
#define GX_RGB8             1           /*!< 24-bit RGB */
#define GX_RGBX8            2           /*!< 32-bit RGBX */
#define GX_RGBA4            3           /*!< 16-bit RGBA */
#define GX_RGBA6            4           /*!< 24-bit RGBA */
#define GX_RGBA8            5           /*!< 32-bit RGBA */

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

namespace Gc::Gx
{
    const char* getTexFmtName(u8 fmt);
    const char* getTlutFmtName(u8 fmt);

    struct GXVtxAttrFmt
    {
        /* 0x00 */ u32 vtxattr;
        /* 0x04 */ u32 comptype;
        /* 0x08 */ u32 compsize;
        /* 0x0C */ u32 frac;
    };
}