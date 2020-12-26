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

        void readData(void* buffer, size_t size);
        std::vector<u8> readData(size_t size);
        template<class T>
        void readStrucBe(T* t)
        {
            readData(t, sizeof(T));
            t->bomSwap();
        }
        template<class T>
        T readStrucBe()
        {
            T t;
            readStrucBe(&t);
            return t;
        }

        bool isSubReader() const;

        size_t getPos() const;
        void seek(size_t off);

        inline size_t getSize() const
        {
            return m_Size;
        }

    protected:

        // assumes m_Parent == nullptr
        virtual size_t getPosImpl() const = 0;
        virtual void seekImpl(size_t off) = 0;
        virtual void readDataImpl(void* buffer, size_t size) = 0;

    protected:
        DataReader* m_Parent;
        size_t m_BaseAddr;
        size_t m_Size;
    };
}