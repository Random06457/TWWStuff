#include "BfnReader.hpp"
#include "Gc/GxTexture.hpp"

namespace Zelda::Bfn
{
    void Reader::handleBlock(BlockHeader* hdr)
    {
        size_t off = m_Reader->getPos();

        if (hdr->magic.matches(INF1_MAGIC))
        {
            m_Reader->readStrucBe(&m_Info);
        }
        else if (hdr->magic.matches(GLY1_MAGIC))
        {
            GlyphBlock block;
            m_Reader->readStrucBe(&block);
            m_Gly.emplace(off, block);
        }
        else if (hdr->magic.matches(MAP1_MAGIC))
        {
            MapBlock block;
            m_Reader->readStrucBe(&block);
            m_Map.emplace(off, block);
        }
        else if (hdr->magic.matches(WID1_MAGIC))
        {
            WidthBlock block;
            m_Reader->readStrucBe(&block);
            m_Width.emplace(off, block);
        }
        else
        {
            assert(0 && "Unhandled header\n");
        }
    }
    
    std::vector<u8> Reader::getTexture(size_t* outWidth, size_t* outHeight)
    {
        for (auto& gly : m_Gly)
        {
            m_Reader->seek(gly.first + sizeof(GlyphBlock));
            auto buff = m_Reader->readData(gly.second.blockSize);
            if (outWidth)
                *outWidth = gly.second.width;
            if (outHeight)
                *outHeight = gly.second.height;
            
            return Gc::Gx::Texture::convertToRGBA8(buff.data(), gly.second.width, gly.second.height, gly.second.fmt);
        }
        assert(0 && "no glyph header");
    }

    void Reader::print()
    {
        printf("Information Header:\n");
        printf("Ascent : %d\n", m_Info.ascent);
        printf("Descent : %d\n", m_Info.descent);
        printf("Leading : %d\n", m_Info.leading);
        printf("\n");

        for (auto& gly : m_Gly)
        {
            printf("Glyph Header:\n");
            printf("Code Range : 0x%04X-0x%04X\n", gly.second.firstCode, gly.second.lastCode);
            printf("Block Size : 0x%X\n", gly.second.blockSize);
            printf("Format : %s\n",  Gc::Gx::getTexFmtName(gly.second.fmt));
            printf("Cell Dimensions: %dx%d\n", gly.second.cellWidth, gly.second.cellHeight),
            printf("UNK Dimension : %dx%d\n", gly.second.unkWidth, gly.second.unkHeight);
            printf("Texture Dimensions : %dx%d\n", gly.second.width, gly.second.height);
            printf("\n");
        }

        for (auto& map : m_Width)
        {
            printf("Width Header:\n");
            printf("Code Range : 0x%04X-0x%04X\n", map.second.firstCode, map.second.lastCode);
            printf("\n");
        }
        
        printf("Map Headers:\n");
        for (auto& map : m_Map)
        {
            printf("Type : %d; Char Range : 0x%04X-0x%04X; Code Count : %d\n",
                map.second.type, map.second.firstChr, map.second.lastChr, map.second.codeCount);
        }
        printf("\n");


    }
}

