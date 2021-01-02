#pragma once
#include <memory>
#include "Utils/DataReader.hpp"
#include "Bdl.hpp"

namespace Zelda::Bdl
{
    class Reader
    {
    public:
        Reader(std::unique_ptr<Utils::DataReader> reader);
    
    private:
        void parse();

    public:
        std::unique_ptr<Utils::DataReader> m_Reader;

    private:
        BdlHeader m_Hdr;
    };
    
}