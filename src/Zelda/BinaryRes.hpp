#pragma once

#include <memory>
#include "Utils/Be.hpp"
#include "Utils/Signature.hpp"
#include "Utils/DataReader.hpp"

namespace Zelda
{
    struct BinaryResHeader
    {
        /* 0x00 */ Utils::Signature<8> magic;
        /* 0x08 */ u32 fileSize;
        /* 0x0C */ u32 blockCount;

        void bomSwap()
        {
            BOM_SWAP(fileSize);
            BOM_SWAP(blockCount);
        }
    };
    static_assert(sizeof(BinaryResHeader) == 0x10);

    struct BlockHeader
    {
        /* 0x00 */ Utils::Signature<4> magic;
        /* 0x04 */ u32 size;

        void bomSwap()
        {
            BOM_SWAP(size);
        }
    };
    static_assert(sizeof(BlockHeader) == 0x08);

    template<typename HeaderType>
    class BinaryResReader
    {
    public:

    protected:
        virtual void handleBlock(BlockHeader* hdr) = 0;
        void parse()
        {
            m_Reader->readStrucBe(&m_Hdr);
            //assert(m_Hdr.valid());

            
            BlockHeader block;
            for (size_t i = 0; i < m_Hdr.blockCount; i++)
            {
                size_t pos = m_Reader->getPos();

                m_Reader->readStrucBe(&block);
                m_Reader->seek(pos);

                handleBlock(&block);

                m_Reader->seek(pos + block.size);
            }
        }

        
    protected:
        HeaderType m_Hdr;
        std::unique_ptr<Utils::DataReader> m_Reader;
    };
    
}