#pragma once
#include "DataReader.hpp"

namespace Utils
{
    class MemReader : public DataReader
    {
    public:
        MemReader(void* buffer, size_t size);
    
    private:
        virtual size_t getPosImpl() const = 0;
        virtual void seekImpl(size_t off) = 0;
        virtual void readDataImpl(size_t off, void* buffer, size_t size) = 0;

    protected:
        u8* m_Buffer;
        size_t m_Pos;
    };
    
    
}