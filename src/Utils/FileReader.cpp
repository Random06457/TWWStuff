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
    
    size_t FileReader::getPosImpl() const
    {
        return ftell(m_File);
    }

    void FileReader::seekImpl(size_t off)
    {
        fseek(m_File, off, SEEK_SET);
    }

    void FileReader::readDataImpl(void* buffer, size_t size)
    {
        size_t read = 0;
        while (read < size)
            read += fread(reinterpret_cast<u8*>(buffer)+read, 1, size-read, m_File);
    }
}