#pragma once
#include <stdio.h>
#include <string>
#include "DataReader.hpp"

namespace Utils
{
    class FileReader : public DataReader
    {
    public:
        FileReader(const FileReader* parent, size_t baseAddr, size_t size) :
            DataReader(parent, baseAddr, size),
            m_File(parent->m_File)
        { }
        FileReader(std::string path);
        virtual ~FileReader();
        
        virtual std::unique_ptr<DataReader> createSubReader(size_t base, size_t size) const;
        
    private:
        virtual void readDataImpl(void* buffer, size_t size);

    protected:
        FILE* m_File;
    };
    
}