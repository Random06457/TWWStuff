#pragma once
#include <vector>
#include "../Types.h"

namespace Utils
{
    class DataReader
    {
    public:
        DataReader();
        DataReader(DataReader* parent, size_t baseAddr, size_t size);
        virtual ~DataReader();

        void readData(size_t off, void* buffer, size_t size);
        std::vector<u8> readData(size_t off, size_t size);
        bool isSubReader() const;

    protected:
        size_t getPos() const;
        void seek(size_t off);

        // assumes m_Parent == nullptr
        virtual size_t getPosImpl() const = 0;
        virtual void seekImpl(size_t off) = 0;
        virtual void readDataImpl(size_t off, void* buffer, size_t size) = 0;

    protected:
        DataReader* m_Parent;
        size_t m_BaseAddr;
        size_t m_Size;
    };
}