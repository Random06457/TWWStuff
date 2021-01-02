#pragma once

#include "BinaryRes.hpp"
#include "Bfn.hpp"
#include <map>

namespace Zelda::Bfn
{
    class Reader : public BinaryResReader<BfnHeader>
    {

    public:
        Reader(std::unique_ptr<Utils::DataReader> reader)
        {
            m_Reader = std::move(reader);
            parse();
        }


        std::vector<u8> getTexture(size_t* outWith, size_t* outHeight);
        void print();

    private:
        virtual void handleBlock(BlockHeader* hdr);


    private:
        InfoBlock m_Info;
        std::map<size_t, GlyphBlock> m_Gly;
        std::map<size_t, MapBlock> m_Map;
        std::map<size_t, WidthBlock> m_Width;
    };
    
    
}