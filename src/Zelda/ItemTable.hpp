#pragma once
#include "Utils/Be.hpp"
#include "Utils/Signature.hpp"

namespace Zelda::ItemTable
{
    struct ItemTableHeader
    {
        /* 0x00 */ Utils::Signature<10> magic;
        /* 0x0A */ u16 count;
        /* 0x0C */ char pad_0C[4];
        /* 0x10 */ u8 entries[][0x10];

        void bomSwap()
        {
            BOM_SWAP(count);
        }

        inline bool valid() const
        {
            return magic.matches("ITEM_TABLE");
        }
    };
    static_assert(sizeof(ItemTableHeader) == 0x10);
      
};