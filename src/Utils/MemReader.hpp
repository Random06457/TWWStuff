#pragma once
#include "DataReader.hpp"

namespace Utils
{
    class MemReader : public DataReader
    {
    public:
        MemReader(const MemReader* parent, size_t baseAddr, size_t size) :
            DataReader(parent, baseAddr, size),
            m_Buffer(parent->m_Buffer+baseAddr),
            m_FreeInDtor(false)
        { }
        MemReader(void* buffer, size_t size, bool freeInDtor);
        virtual ~MemReader();
        
        std::unique_ptr<DataReader> createSubReader(size_t base, size_t size) const;
    
    private:
        virtual void readDataImpl(void* buffer, size_t size);

    protected:
        u8* m_Buffer;
        bool m_FreeInDtor;
    };
    
    
}