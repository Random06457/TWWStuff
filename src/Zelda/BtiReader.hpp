#pragma once

#include <memory>
#include "Utils/DataReader.hpp"
#include "Bti.hpp"

namespace Zelda::Bti
{
    class Reader
    {
    public:
        Reader(std::unique_ptr<Utils::DataReader> reader);

        void print() const;
        std::vector<u8> readTexture() const;
        std::vector<u8> readTlut() const;

        std::vector<u8> toRGBA8() const;

        inline bool containsTlut() const
        {
            return m_Hdr.tlutEntries != 0;
        }
    private:
        void parse();

    public:
        std::unique_ptr<Utils::DataReader> m_Reader;
        BtiHeader m_Hdr;
    };
    
}