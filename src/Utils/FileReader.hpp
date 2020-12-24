#pragma once
#include <stdio.h>
#include <string>
#include "DataReader.hpp"

namespace Utils
{
    class FileReader : public DataReader
    {
    public:
        FileReader(std::string path);
        virtual ~FileReader();
        
    private:
        virtual size_t getPosImpl() const = 0;
        virtual void seekImpl(size_t off) = 0;
        virtual void readDataImpl(size_t off, void* buffer, size_t size) = 0;

    protected:
        FILE* m_File;
    };
    
}