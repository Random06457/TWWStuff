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
        virtual size_t getPosImpl() const;
        virtual void seekImpl(size_t off);
        virtual void readDataImpl(void* buffer, size_t size);

    protected:
        FILE* m_File;
    };
    
}