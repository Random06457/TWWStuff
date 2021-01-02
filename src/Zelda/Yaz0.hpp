#pragma once

#include <vector>
#include "Types.h"
#include "Utils/Be.hpp"
#include "Utils/Signature.hpp"
#include "Utils/MemReader.hpp"

namespace Zelda::Yaz0
{   
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

    void decompress(const void* input, void* output);
    std::vector<u8> decompress(std::vector<u8> data);
    std::unique_ptr<Utils::DataReader> createReader(std::unique_ptr<Utils::DataReader> oldReader);
}