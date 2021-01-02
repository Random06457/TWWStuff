#include "BtiReader.hpp"
#include "Gc/GxTexture.hpp"

namespace Zelda::Bti
{
    Reader::Reader(std::unique_ptr<Utils::DataReader> reader)
        : m_Reader(std::move(reader))
    {
        parse();
    }

    void Reader::parse()
    {
        m_Reader->readStrucBe(&m_Hdr);
    }

    void Reader::print() const
    {
        printf("Format : %s\n", Gc::Gx::getTexFmtName(m_Hdr.fmt));
        printf("Transparency : 0x%X\n", m_Hdr.transparency);
        printf("Resolution : %dx%d\n", m_Hdr.width, m_Hdr.height);
        printf("Wrap S : 0x%X; Wrap T : 0x%X\n", m_Hdr.wrapS, m_Hdr.wrapT);

        if (m_Hdr.tlutEntries > 0)
        {
            printf("TLUT : %s (0x%X entries)\n",
                Gc::Gx::getTlutFmtName(m_Hdr.tlutFmt), m_Hdr.tlutEntries);
        }
    }

    std::vector<u8> Reader::readTexture() const
    {
        m_Reader->seek(m_Hdr.texOff);
        return m_Reader->readData(Gc::Gx::Texture::getTexSize(m_Hdr.fmt, m_Hdr.width, m_Hdr.height));
    }

    std::vector<u8> Reader::readTlut() const
    {
        assert(containsTlut());
        m_Reader->seek(m_Hdr.tlutOff);
        return m_Reader->readData(2* m_Hdr.tlutEntries);
    }

    std::vector<u8> Reader::toRGBA8() const
    {
        auto data = readTexture();
        if (containsTlut())
        {
            auto tlut = readTlut();;
            return Gc::Gx::Texture::convertToRGBA8(data.data(), m_Hdr.width, m_Hdr.height, m_Hdr.fmt, tlut.data(), m_Hdr.tlutFmt);
        }
        
        return Gc::Gx::Texture::convertToRGBA8(data.data(), m_Hdr.width, m_Hdr.height, m_Hdr.fmt);
    }
}