#pragma once
#include <vector>
#include "Types.h"
#include <assert.h>
#include <memory>

namespace Utils
{
    class DataReader
    {
    public:
        class ScopedSeek
        {
        public:
            ScopedSeek(DataReader* reader, size_t pos)
                : m_Reader(reader),
                m_TempPos(reader->getPos())
            {
                m_Reader->seek(pos);
            }
            ~ScopedSeek()
            {
                m_Reader->seek(m_TempPos);
            }

        private:
            DataReader* m_Reader;
            size_t m_TempPos;
        };



    public:
        DataReader();
        DataReader(const DataReader* parent, size_t baseAddr, size_t size);
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

        inline bool isSubReader() const
        {
            return !!m_Parent;
        }
        inline size_t getPos() const
        {
            return m_Pos;
        }
        inline void seek(size_t pos)
        {
            assert(pos < m_Size);
            m_Pos = pos;
        }
        inline size_t getSize() const
        {
            return m_Size;
        }
        inline ScopedSeek scopedSeek(size_t pos) const
        {
            return ScopedSeek(const_cast<DataReader*>(this), pos);
        }


        size_t getAbsolutePos() const;

        virtual std::unique_ptr<DataReader> createSubReader(size_t base, size_t size) const = 0;

    protected:
        virtual void readDataImpl(void* buffer, size_t size) = 0;

    protected:
        const DataReader* m_Parent;
        size_t m_BaseAddr;
        size_t m_Size;
        size_t m_Pos;
    };
}