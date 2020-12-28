#include <assert.h>
#include "GxTexture.hpp"

#define IDX(x, y, w) ((y)*(w)+(x))
#define PX(buff, pxType, x, y, w) = (reinterpret_cast<pxType*>)[IDX(x, y, w)]
#define RGBA8(r, g, b, a) (((a&0xFF)<<24) | ((b&0xFF)<<16) | ((g&0xFF)<<8) | (b & 0xFF))

namespace Gc::Gx::Texture
{   
    
    struct ColorRGBA8
    {
        ColorRGBA8()
        {
            abgr = 0;
        }
        ColorRGBA8(u8 r, u8 g, u8 b, u8 a)
        {
            this->r = r;
            this->g = g;
            this->b = b;
            this->a = a;
        }
        ColorRGBA8(u32 abgr)
        {
            this->abgr = abgr;
        }
        union
        {
            struct { u8 r, g, b, a; };
            u32 abgr;
        };
    };
    
    struct ColorIA8
    {
        u8 i;
        u8 a;

        ColorRGBA8 toRGBA8() const { return ColorRGBA8((0x010101u * i) | (a << 24)); }
    };

    struct ColorI8
    {
        u8 i;

        ColorRGBA8 toRGBA8() const { return ColorRGBA8((0x010101 * i) | 0xFF000000); }
    };

    struct ColorRGB565
    {
        union
        {
            u16 rgb;
            struct
            {
                u16 b : 5;
                u16 g : 6;
                u16 r : 5;
            };
        };

        ColorRGBA8 toRGBA8() const
        {
            ColorRGB565 bePx = { BS16(rgb) };
            return ColorRGBA8(bePx.r * 8, bePx.g * 4, bePx.b * 8, 0xFF);
        }
    };

    struct ColorIA4
    {
        u8 i : 4;
        u8 a : 4;

        ColorRGBA8 toRGBA8() const { return ColorRGBA8((0x111111u * i) | ((a * 0x11) << 24)); }
    };

    struct ColorRGB5A3
    {
        union
        {
            u16 rgba;
            struct
            {
                u16 b : 5;
                u16 g : 5;
                u16 r : 5;
                u16 isOpaque : 1;
            } opaque;
            struct
            {
                u16 b : 4;
                u16 g : 4;
                u16 r : 4;
                u16 a : 3;
                u16 isOpaque : 1;
            };
        };

        ColorRGBA8 toRGBA8() const
        {
            ColorRGB5A3 bePx = { BS16(rgba) };

            return (bePx.isOpaque)
                ? ColorRGBA8(bePx.opaque.r * 8, bePx.opaque.g * 8, bePx.opaque.b * 8, 0xFF)
                : ColorRGBA8(bePx.r * 0x11, bePx.g * 0x11, bePx.b * 0x11, bePx.a * 0x20);
        }
    };


    struct BlockI4
    {
        struct EntryI4
        {
            u8 i1:4;
            u8 i2:4;
        };

        static void decodeBlock(const void* block, void* outPtr, size_t x, size_t y, size_t bw, size_t bh, size_t w, size_t h, const void*)
        {
            auto in = reinterpret_cast<const EntryI4*>(block);
            auto out = reinterpret_cast<u32*>(outPtr);

            for (size_t by = 0; by < bh && y+by < h; by++)
            for (size_t bx = 0; bx < bw && x+bx < w; bx += 2)
            {
                out[IDX(x+bx, y+by, w)+0] = 0xFF000000 | (in[IDX(bx, by, bw)].i1 * 0x111111);
                out[IDX(x+bx, y+by, w)+1] = 0xFF000000 | (in[IDX(bx, by, bw)].i2 * 0x111111);
            }
        }
    };

    struct BlockRGBA8
    {
        struct
        {
            u8 a, r;
        } ar[16];
        struct
        {
            u8 g, b;
        } gb[16];

        static void decodeBlock(const void* block, void* outPtr, size_t x, size_t y, size_t bw, size_t bh, size_t w, size_t h, const void*)
        {
            auto in = reinterpret_cast<const BlockRGBA8*>(block);
            auto out = reinterpret_cast<ColorRGBA8*>(outPtr);

            for (size_t by = 0; by < bh && y+by < h; by++)
            for (size_t bx = 0; bx < bw && x+bx < w; bx++)
            {
                out[IDX(x+bx, y+by, w)] =
                {
                    in->ar[IDX(bx, by, bw)].r,
                    in->gb[IDX(bx, by, bw)].g,
                    in->gb[IDX(bx, by, bw)].b,
                    in->ar[IDX(bx, by, bw)].a,
                };
            }
        }
    };
    
    // https://docs.microsoft.com/en-us/windows/win32/direct3d10/d3d10-graphics-programming-guide-resources-block-compression#bc1
    struct BlockBC1
    {
        
        ColorRGB565 c0;
        ColorRGB565 c1;
        struct
        {
            u8 i3 : 2;
            u8 i2 : 2;
            u8 i1 : 2;
            u8 i0 : 2;
        } i[4];

        static void decodeBlock(const void* block, void* outPtr, size_t x, size_t y, size_t bw, size_t bh, size_t w, size_t h, const void*)
        {
            auto in = reinterpret_cast<const BlockBC1*>(block);
            auto out = reinterpret_cast<ColorRGBA8*>(outPtr);

            for (size_t by = 0; by < bh && y+by < h; by += 4)
            for (size_t bx = 0; bx < bw && x+bx < w; bx += 4)
            {

                ColorRGBA8 c[4];
                c[0] = in->c0.toRGBA8();
                c[1] = in->c1.toRGBA8();
                
                u16 c0 = BS16(in->c0.rgb);
                u16 c1 = BS16(in->c1.rgb);
                if (c0 > c1)
                {
                    c[2].r = 2 * c[0].r / 3 + c[1].r / 3;
                    c[2].g = 2 * c[0].g / 3 + c[1].g / 3;
                    c[2].b = 2 * c[0].b / 3 + c[1].b / 3;
                    c[2].a = 0xFF;

                    c[3].r = c[0].r / 3 + 2 * c[1].r / 3;
                    c[3].g = c[0].g / 3 + 2 * c[1].g / 3;
                    c[3].b = c[0].b / 3 + 2 * c[1].b / 3;
                    c[3].a = 0xFF;
                }
                else
                {
                    c[2].r = c[0].r / 2 + c[1].r / 2;
                    c[2].g = c[0].g / 2 + c[1].g / 2;
                    c[2].b = c[0].b / 2 + c[1].b / 2;
                    c[2].a = 0xFF;
                    
                    c[3] = { 0 };
                }

                for (size_t ty = 0; ty < 4; ty++)
                {
                    out[IDX(x+bx+0, y+by+ty, w)] = c[in->i[ty].i0];
                    out[IDX(x+bx+1, y+by+ty, w)] = c[in->i[ty].i1];
                    out[IDX(x+bx+2, y+by+ty, w)] = c[in->i[ty].i2];
                    out[IDX(x+bx+3, y+by+ty, w)] = c[in->i[ty].i3];
                }
                
                in++;
            }
        }
    };

    template<typename Color>
    struct BlockSimple
    {
        static void decodeBlock(const void* block, void* outPtr, size_t x, size_t y, size_t bw, size_t bh, size_t w, size_t h, const void*)
        {
            auto in = reinterpret_cast<const Color*>(block);
            auto out = reinterpret_cast<ColorRGBA8*>(outPtr);

            for (size_t by = 0; by < bh && y+by < h; by++)
            for (size_t bx = 0; bx < bw && x+bx < w; bx++)
                out[IDX(x+bx, y+by, w)] = in[IDX(bx, by, bw)].toRGBA8();
        }
    };

    template<typename Color>
    struct BlockCI4
    {
        struct EntryCI4
        {
            u8 i1:4;
            u8 i2:4;
        };

        static void decodeBlock(const void* block, void* outPtr, size_t x, size_t y, size_t bw, size_t bh, size_t w, size_t h, const void* tlutPtr)
        {
            auto in = reinterpret_cast<const EntryCI4*>(block);
            auto tlut = reinterpret_cast<const Color*>(tlutPtr);
            auto out = reinterpret_cast<ColorRGBA8*>(outPtr);

            for (size_t by = 0; by < bh && y+by < h; by++)
            for (size_t bx = 0; bx < bw && x+bx < w; bx += 2)
            {
                out[IDX(x+bx, y+by, w)+0] = tlut[in[IDX(bx, by, bw)].i1].toRGBA8();
                out[IDX(x+bx, y+by, w)+1] = tlut[in[IDX(bx, by, bw)].i2].toRGBA8();
            }
        }
    };

    template<typename Color>
    struct BlockCI8
    {
        static void decodeBlock(const void* block, void* outPtr, size_t x, size_t y, size_t bw, size_t bh, size_t w, size_t h, const void* tlutPtr)
        {
            auto in = reinterpret_cast<const u8*>(block);
            auto tlut = reinterpret_cast<const Color*>(tlutPtr);
            auto out = reinterpret_cast<ColorRGBA8*>(outPtr);

            for (size_t by = 0; by < bh && y+by < h; by++)
            for (size_t bx = 0; bx < bw && x+bx < w; bx++)
                out[IDX(x+bx, y+by, w)] = tlut[in[IDX(bx, by, bw)]].toRGBA8();
        }
    };

    template<typename Color>
    struct BlockCI14
    {
        static void decodeBlock(const void* block, void* outPtr, size_t x, size_t y, size_t bw, size_t bh, size_t w, size_t h, const void* tlutPtr)
        {
            auto in = reinterpret_cast<const u16*>(block);
            auto tlut = reinterpret_cast<const Color*>(tlutPtr);
            auto out = reinterpret_cast<ColorRGBA8*>(outPtr);

            for (size_t by = 0; by < bh && y+by < h; by++)
            for (size_t bx = 0; bx < bw && x+bx < w; bx++)
                out[IDX(x+bx, y+by, w)] = tlut[in[IDX(bx, by, bw)] & 0x3FFF].toRGBA8();
        }
    };


    template<typename Px>
    std::vector<u8> decode(const void* data, size_t width, size_t height, size_t blockW, size_t blockH, size_t blockSize, const void* tlut = nullptr)
    {
        std::vector<u8> ret(4*width*height);

        auto inPtr = reinterpret_cast<const u8*>(data);

        size_t blockIdx = 0;
        for (size_t y = 0; y < height; y += blockH)
        for (size_t x = 0; x < width; x += blockW, blockIdx++)
            Px::decodeBlock(inPtr + blockIdx * blockSize, ret.data(), x, y, blockW, blockH, width, height, tlut);
        
        return ret;
    }


    std::vector<u8> convertToRGBA8(const void* data, size_t width, size_t height, u8 fmt, const void* tlut, u8 tlutFmt)
    {
        switch (fmt)
        {
            case GX_TF_CI4:
                switch (tlutFmt)
                {
                    case GX_TL_IA8: return decode<BlockCI4<ColorIA8>>(data, width, height, 8, 8, 0x20);
                    case GX_TL_RGB565: return decode<BlockCI4<ColorRGB565>>(data, width, height, 8, 8, 0x20);
                    case GX_TL_RGB5A3: return decode<BlockCI4<ColorRGB5A3>>(data, width, height, 8, 8, 0x20);
                    default: assert(0 && "invalid TLUT format");
                }
                
            case GX_TF_CI8:
                switch (tlutFmt)
                {
                    case GX_TL_IA8: return decode<BlockCI8<ColorIA8>>(data, width, height, 8, 4, 0x20, tlut);
                    case GX_TL_RGB565: return decode<BlockCI8<ColorRGB565>>(data, width, height, 8, 4, 0x20, tlut);
                    case GX_TL_RGB5A3: return decode<BlockCI8<ColorRGB5A3>>(data, width, height, 8, 4, 0x20, tlut);
                    default: assert(0 && "invalid TLUT format");
                }
            case GX_TF_CI14:
                switch (tlutFmt)
                {
                    case GX_TL_IA8: return decode<BlockCI8<ColorIA8>>(data, width, height, 8, 4, 0x20, tlut);
                    case GX_TL_RGB565: return decode<BlockCI8<ColorRGB565>>(data, width, height, 8, 4, 0x20, tlut);
                    case GX_TL_RGB5A3: return decode<BlockCI8<ColorRGB5A3>>(data, width, height, 8, 4, 0x20, tlut);
                    default: assert(0 && "invalid TLUT format");
                }

            case GX_TF_CMPR: return decode<BlockBC1>(data, width, height, 8, 8, 0x20);
            case GX_TF_RGBA8: return decode<BlockRGBA8>(data, width, height, 4, 4, 0x40);
            case GX_TF_I4: return decode<BlockI4>(data, width, height, 8, 4, 0x20);
            case GX_TF_IA4: return decode<BlockSimple<ColorIA4>>(data, width, height, 8, 4, 0x20);
            case GX_TF_IA8: return decode<BlockSimple<ColorIA8>>(data, width, height, 4, 4, 0x20);
            case GX_TF_RGB565: return decode<BlockSimple<ColorRGB565>>(data, width, height, 4, 4, 0x20);
            case GX_TF_RGB5A3: return decode<BlockSimple<ColorRGB5A3>>(data, width, height, 4, 4, 0x20);
            case GX_TF_I8: return decode<BlockSimple<ColorRGB565>>(data, width, height, 8, 4, 0x20);

            default: assert(0 && "invalid texture format");
        }
    }

    size_t getTexSize(u8 fmt, u16 width, u16 height)
    {
        size_t bitSize = 0;
        size_t bw = 0, bh = 0;

        switch (fmt)
        {
            case GX_TF_I4:
            case GX_TF_CMPR:
            case GX_TF_CI4:
                bitSize = 4;
                bw = 8; bh = 8;
                break;

            case GX_TF_I8:
            case GX_TF_IA4:
            case GX_TF_CI8:
                bitSize = 8;
                bw = 8; bh = 4;
                break;
            
            case GX_TF_IA8:
            case GX_TF_RGB565:
            case GX_TF_RGB5A3:
            case GX_TF_CI14:
                bitSize = 16;
                bw = 4;
                bh = 4;
                break;
            
            case GX_TF_RGBA8:
                bitSize = 32;
                bw = 4; bh = 4;
                break;

            default:
                assert(0 && "not supported");
        }

        width += (bw - (width % bw)) % bw;
        height += (bh - (height % bh)) % bh;

        return width*height*bitSize/8;
    }
}