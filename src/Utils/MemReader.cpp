#include "MemReader.hpp"
#include <cstring>

namespace Utils
{
    MemReader::MemReader(void* buffer, size_t size, bool freeInDtor) :
        DataReader(),
        m_Buffer(reinterpret_cast<u8*>(buffer)),
        m_Pos(0),
        m_FreeInDtor(freeInDtor)
    {
        m_Size = size;
    }

    MemReader::~MemReader()
    {
        if (m_FreeInDtor && !isSubReader())
            delete[] m_Buffer;
    }

    
    size_t MemReader::getPosImpl() const
    {
        return m_Pos;
    }
    void MemReader::seekImpl(size_t off)
    {
        m_Pos = off;
    }
    void MemReader::readDataImpl(void* buffer, size_t size)
    {
        memcpy(buffer, m_Buffer + m_Pos, size);
        m_Pos += size;
    }
}
