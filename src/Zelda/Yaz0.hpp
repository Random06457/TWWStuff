#pragma once

#include <vector>
#include "../Types.h"
#include "../Utils/Be.hpp"
#include "../Utils/Signature.hpp"

namespace Zelda
{
    class Yaz0
    {

    private:
    
        struct Yaz0Header
        {
            /* 0x00 */ Utils::Signature<4> magic;
            /* 0x04 */ u32 decSize;
            /* 0x08 */ char pad_08[8];
            /* 0x10 */ u8 data[];

            void bomSwap()
            {
                BOM_SWAP(decSize);
            }

            bool valid() const
            {
                return magic.matches("Yaz0");
            }
        };
        static_assert(sizeof(Yaz0Header) == 0x10);

    public:
        static void decompress(const void* input, void* output);
        static std::vector<u8> decompress(std::vector<u8> data);
    };
}