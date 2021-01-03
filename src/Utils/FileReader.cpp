#include <assert.h>
#include "FileReader.hpp"

namespace Utils
{

    FileReader::FileReader(std::string path) :
        m_File(fopen(path.c_str(), "rb"))
    {
        assert(m_File && "should not happen");

        fseek(m_File, 0, SEEK_END);
        m_Size = ftell(m_File);
        fseek(m_File, 0, SEEK_SET);
    }

    FileReader::~FileReader()
    {
        if (!isSubReader())
            fclose(m_File);
    }

    void FileReader::readDataImpl(void* buffer, size_t size)
    {
        fseek(m_File, getAbsolutePos(), SEEK_SET);
        size_t read = 0;
        while (read < size)
            read += fread(reinterpret_cast<u8*>(buffer)+read, 1, size-read, m_File);
    }
    
    std::unique_ptr<DataReader> FileReader::createSubReader(size_t base, size_t size) const
    {
        assert(base+size <= m_Size);
        return std::make_unique<FileReader>(this, base, size);
    }
}