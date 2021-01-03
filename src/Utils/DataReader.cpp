#include "DataReader.hpp"

namespace Utils
{
    
    DataReader::DataReader() :
        m_Parent(nullptr),
        m_BaseAddr(0),
        m_Size(0),
        m_Pos(0)
    {

    }
    DataReader::DataReader(const DataReader* parent, size_t baseAddr, size_t size) :
        m_Parent(parent),
        m_BaseAddr(baseAddr),
        m_Size(size),
        m_Pos(0)
    {

    }

    DataReader::~DataReader()
    {

    }


    void DataReader::readData(void* buffer, size_t size)
    {
        assert(getPos()+size <= m_Size);
        readDataImpl(buffer, size);
        m_Pos += size;
    }

    std::vector<u8> DataReader::readData(size_t size)
    {
        std::vector<u8> ret(size);
        readData(ret.data(), size);
        return ret;
    }

    size_t DataReader::getAbsolutePos() const
    {
        size_t pos = m_Pos;
        const DataReader* reader = this;
        while (reader->isSubReader())
        {
            pos += reader->m_BaseAddr;
            reader = reader->m_Parent;
        }
        return pos;
    }
}