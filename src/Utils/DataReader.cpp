#include <assert.h>
#include "DataReader.hpp"

namespace Utils
{
    
    DataReader::DataReader() :
        m_Parent(nullptr),
        m_BaseAddr(0),
        m_Size(0)
    {

    }
    DataReader::DataReader(DataReader* parent, size_t baseAddr, size_t size) :
        m_Parent(parent),
        m_BaseAddr(baseAddr),
        m_Size(size)
    {

    }

    DataReader::~DataReader()
    {

    }


    void DataReader::readData(void* buffer, size_t size)
    {
        assert(getPos()+size <= m_Size);
        readDataImpl(buffer, size);
    }

    std::vector<u8> DataReader::readData(size_t size)
    {
        std::vector<u8> ret(size);
        readData(ret.data(), size);
        return ret;
    }

    bool DataReader::isSubReader() const
    {
        return !!m_Parent;
    }

    void DataReader::seek(size_t off)
    {
        if (m_Parent)
            m_Parent->seek(off + m_BaseAddr);
        else
            seekImpl(off);
    }

    size_t DataReader::getPos() const
    {
        return m_Parent
            ? m_Parent->getPos() - m_BaseAddr
            : getPosImpl();
    }
}