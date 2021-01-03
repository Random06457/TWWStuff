#include "MemReader.hpp"
#include <cstring>

namespace Utils
{
    MemReader::MemReader(void* buffer, size_t size, bool freeInDtor) :
        DataReader(),
        m_Buffer(reinterpret_cast<u8*>(buffer)),
        m_FreeInDtor(freeInDtor)
    {
        m_Size = size;
    }

    MemReader::~MemReader()
    {
        if (m_FreeInDtor && !isSubReader())
            delete[] m_Buffer;
    }

    void MemReader::readDataImpl(void* buffer, size_t size)
    {
        memcpy(buffer, m_Buffer + m_Pos, size);
    }
    
    std::unique_ptr<DataReader> MemReader::createSubReader(size_t base, size_t size) const
    {
        return std::make_unique<MemReader>(this, base, size);
    }
}
