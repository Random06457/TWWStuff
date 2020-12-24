#include "Yaz0.hpp"

namespace Zelda
{
    void Yaz0::decompress(const void* input, void* output)
    {
        auto hdr = reinterpret_cast<const Yaz0Header*>(input);
        auto inPtr = reinterpret_cast<const u8*>(hdr->data);
        auto outPtr = reinterpret_cast<u8*>(output);

        assert(hdr->valid());

        size_t decSize = Utils::Be::bomSwap(hdr->decSize);

        size_t bitIdx = 7;
        u8 hdrByte;

        size_t inIdx = 0;
        size_t outIdx = 0;

        while (outIdx < decSize)
        {
            if (++bitIdx >= 8)
            {
                hdrByte = inPtr[inIdx++];
                bitIdx = 0;
            }

            // decompressed
            if (hdrByte & 0x80)
                outPtr[outIdx++] = inPtr[inIdx++];
            // compressed
            else
            {
                // N = size; B = back
                // 3 bytes: 0B BB NN or
                // 2 bytes: NB BB

                u8 b0 = inPtr[inIdx++];
                u8 b1 = inPtr[inIdx++];

                size_t backIdx = outIdx - (((b0 & 0xF) << 8) | b1) - 1;
                size_t nBytes = (b0 >> 4) + 2;
                if (nBytes == 2) // 3 bytes
                    nBytes = inPtr[inIdx++] + 0x12;

                while (nBytes-- > 0)
                    outPtr[outIdx++] = outPtr[backIdx++];
            }

            hdrByte <<= 1;
        }
        
    }
    std::vector<u8> Yaz0::decompress(std::vector<u8> data)
    {
        Yaz0Header* hdr = reinterpret_cast<Yaz0Header*>(data.data());
        std::vector<u8> ret(Utils::Be::bomSwap(hdr->decSize));
        decompress(data.data(), ret.data());
        return ret;
    }
}