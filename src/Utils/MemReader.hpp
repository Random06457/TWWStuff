#pragma once
#include "DataReader.hpp"

namespace Utils
{
    class MemReader : public DataReader
    {
    public:
        MemReader(void* buffer, size_t size, bool freeInDtor);
        virtual ~MemReader();
    
    private:
        virtual size_t getPosImpl() const;
        virtual void seekImpl(size_t off);
        virtual void readDataImpl(void* buffer, size_t size);

    protected:
        u8* m_Buffer;
        size_t m_Pos;
        bool m_FreeInDtor;
    };
    
    
}