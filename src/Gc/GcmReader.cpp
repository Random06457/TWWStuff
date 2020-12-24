#include "GcmReader.hpp"

namespace Gc::Gcm
{
    Reader::Reader(FILE* f) : m_File(f)
    {
        readHeaders();
    }
    Reader::Reader(std::string path) : Reader(fopen(path.c_str(), "rb"))
    {

    }
    Reader::~Reader()
    {
        fclose(m_File);
    }

    void Reader::seek(size_t off) const
    {
        fseek(m_File, off, SEEK_SET);
    }

    void Reader::readHeaders()
    {
        seek(0);
        m_GcmHdr.read(m_File);

        seek(0x2440);
        m_AppLoaderHdr.read(m_File);

        seek(m_GcmHdr.dolOff);
        m_DolHdr.read(m_File);
        
        seek(m_GcmHdr.fstOff);
        FileEntry root;
        root.read(m_File);

        seek(m_GcmHdr.fstOff);
        m_FileEntries.reserve(root.dir.nextOff);
        m_FileEntries.resize(root.dir.nextOff);
        fread(m_FileEntries.data(), sizeof(FileEntry), root.dir.nextOff, m_File);
        for (auto& entry : m_FileEntries)
            entry.bomSwap();

        size_t strTabSize = m_GcmHdr.fstSize - root.dir.nextOff;
        m_FstStrTable.reserve(strTabSize);
        m_FstStrTable.resize(strTabSize);
        fread(m_FstStrTable.data(), 1, strTabSize, m_File);
    }

    std::vector<u8> Reader::readData(size_t off, size_t size) const
    {
        std::vector<u8> ret(size);

        seek(off);
        fread(ret.data(), 1, size, m_File);

        return ret;
    }

    std::vector<u8> Reader::readDol() const
    {
        return readData(m_GcmHdr.dolOff, m_DolHdr.getDolSize());
    }

    std::vector<u8> Reader::readAppLoader() const
    {
        return readData(0x2440 + sizeof(AppLoaderHeader), m_AppLoaderHdr.size);
    }

    const char* Reader::getName(FileEntry* entry) const
    {
        return m_FstStrTable.data() + entry->nameOff;
    }
    
    
    void Reader::iterFst(std::function<void(u32, std::string, std::string, FileEntry*)> callback)
    {
        iterFst(0, "", 1, m_FileEntries[0].dir.nextOff, callback);
    }

    size_t Reader::iterFst(u32 level, std::string parentPath, size_t i, size_t next, std::function<void(u32, std::string, std::string, FileEntry*)> callback)
    {
        while (i < next)
        {   
            Gc::Gcm::FileEntry* entry = &m_FileEntries[i];

            std::string path = parentPath + "/" + getName(entry);

            callback(level, parentPath, path, entry);

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
        return readData(entry->file.fileOff, entry->file.fileSize);
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