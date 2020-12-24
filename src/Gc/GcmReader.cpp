#include "GcmReader.hpp"
#include "../Utils/FileReader.hpp"
#include "../Utils/MemReader.hpp"

namespace Gc::Gcm
{
    Reader::Reader(std::unique_ptr<Utils::DataReader> reader) :
        m_Reader(std::move(reader))
    {
        readHeaders();
    }

    Reader::Reader(std::string filePath) : 
        Reader(std::make_unique<Utils::FileReader>(filePath))
    {

    }
    Reader::Reader(void* buffer, size_t size) :
        Reader(std::make_unique<Utils::MemReader>(buffer, size))
    {

    }

    void Reader::readHeaders()
    {
        m_Reader->seek(0);
        m_Reader->readStrucBe(&m_GcmHdr);

        m_Reader->seek(0x2440);
        m_Reader->readStrucBe(&m_AppLoaderHdr);

        m_Reader->seek(m_GcmHdr.dolOff);
        m_Reader->readStrucBe(&m_DolHdr);
        
        m_Reader->seek(m_GcmHdr.fstOff);
        auto root = m_Reader->readStrucBe<FileEntry>();

        m_Reader->seek(m_GcmHdr.fstOff);

        m_FileEntries.reserve(root.dir.nextOff);
        for (size_t i = 0; i < root.dir.nextOff; i++)
            m_FileEntries.push_back(m_Reader->readStrucBe<FileEntry>());

        size_t strTabSize = m_GcmHdr.fstSize - root.dir.nextOff;
        m_FstStrTable.reserve(strTabSize);
        m_FstStrTable.resize(strTabSize);
        m_Reader->readData(m_FstStrTable.data(), strTabSize);
    }

    std::vector<u8> Reader::readDol() const
    {
        m_Reader->seek(m_GcmHdr.dolOff);
        return m_Reader->readData(m_DolHdr.getDolSize());
    }

    std::vector<u8> Reader::readAppLoader() const
    {
        m_Reader->seek(0x2440 + sizeof(AppLoaderHeader));
        return m_Reader->readData(m_AppLoaderHdr.size);
    }
    
    
    void Reader::iterFst(std::function<void(u32, std::string, FileEntry*)> callback)
    {
        iterFst(0, "", 1, m_FileEntries[0].dir.nextOff, callback);
    }

    size_t Reader::iterFst(u32 level, std::string parentPath, size_t i, size_t next, std::function<void(u32, std::string, FileEntry*)> callback)
    {
        while (i < next)
        {   
            Gc::Gcm::FileEntry* entry = &m_FileEntries[i];

            std::string path = parentPath + "/" + getName(entry);

            callback(level, path, entry);

            if (entry->isDir)
                i = iterFst(level+1, path, i+1, entry->dir.nextOff, callback);
            else
                i++;
                
        }
        return next;
    }

    std::vector<u8> Reader::readFile(FileEntry* entry) const
    {
        assert(!entry->isDir);
        m_Reader->seek(entry->file.fileOff);
        return m_Reader->readData(entry->file.fileSize);
    }

    void normalizePath(std::string& path)
    {
        // add leading '/'
        if (path[0] != '/')
            path.insert(0, "/");

        // replace '\' with '/'
        size_t pos = path.find("\\");
        while (pos != std::string::npos)
        {
            path[pos] = '/';
            pos = path.find("\\");
        }
        
        // remove double '/'
        pos = path.find("//");
        while (pos != std::string::npos)
        {
            path.erase(pos, 1);
            pos = path.find("//");
        }

        // remove trailing '/'
        if (path[path.size()-1] == '/')
            path.erase(path.size()-1, 1);
    }

    FileEntry* Reader::getFileEntry(std::string path)
    {
        normalizePath(path);

        if (path == "/")
            return &m_FileEntries[0];
            
        size_t startPos = 1;
        size_t endPos = path.find("/", 1);
        if (endPos == std::string::npos)
            endPos = path.size();

        std::string name = path.substr(startPos, endPos-startPos);

        size_t i = 1;
        FileEntry* parent = &m_FileEntries[0];

        while (i < parent->dir.nextOff)
        {
            if (name == getName(&m_FileEntries[i]))
            {
                // done
                if (endPos == path.size())
                    return &m_FileEntries[i];

                
                // try to enter directory

                assert(m_FileEntries[i].isDir);

                startPos = endPos+1;
                endPos = path.find("/", startPos);

                if (endPos == std::string::npos)
                    endPos = path.size();

                name = path.substr(startPos, endPos - startPos);

                parent = &m_FileEntries[i++];
            }
            // skip directory
            else if (m_FileEntries[i].isDir)
                i = m_FileEntries[i].dir.nextOff;
            // next file
            else
                i++;
        }
        
        return nullptr;
    }
}