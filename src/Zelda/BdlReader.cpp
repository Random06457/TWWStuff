#include "BdlReader.hpp"
#include "Bdl.hpp"

namespace Zelda::Bdl
{
    Reader::Reader(std::unique_ptr<Utils::DataReader> reader) :
        m_Reader(std::move(reader))
    {
        parse();
    }

    void Reader::parse()
    {
        m_Reader->readStrucBe(&m_Hdr);
        
        DataBlock res;
        for (size_t i = 0; i < m_Hdr.resCount; i++)
        {
            size_t resHdrPos = m_Reader->getPos();

            m_Reader->readStrucBe(&res);
            printf("%.*s\n", 4, res.magic.value);

            m_Reader->seek(resHdrPos + res.size);
        }
        
    }
}