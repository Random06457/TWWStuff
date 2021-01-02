#include "ArcReader.hpp"
#include "Utils/FileReader.hpp"
#include "Utils/MemReader.hpp"
#include "NameHash.hpp"

namespace Zelda::Arc
{
    Reader::Reader(std::unique_ptr<Utils::DataReader> reader) :
        m_Reader(std::move(reader))
    {
        readHeaders();   
    }

    void Reader::readHeaders()
    {
        m_Reader->readStrucBe(&m_Hdr);
        assert(m_Hdr.valid());

        m_Reader->seek(m_Hdr.headerSize);
        m_Reader->readStrucBe(&m_Hdr2);

        size_t hdr2Pos = m_Hdr.headerSize;

        // read string table
        m_Reader->seek(hdr2Pos + m_Hdr2.strTabOff);
        m_StrTab.resize(m_Hdr2.strTabSize);
        m_Reader->readData(m_StrTab.data(), m_StrTab.size());

        // read res entries
        m_Reader->seek(hdr2Pos + m_Hdr2.resOff);
        m_ResEntries.reserve(m_Hdr2.resCount);
        for (size_t i = 0; i < m_Hdr2.resCount; i++)
            m_ResEntries.push_back(m_Reader->readStrucBe<ResEntry>());

        // read file entries
        m_Reader->seek(hdr2Pos + m_Hdr2.fileOff);
        m_FileEntries.reserve(m_Hdr2.fileCount);
        for (size_t i = 0; i < m_Hdr2.fileCount; i++)
            m_FileEntries.push_back(m_Reader->readStrucBe<FileEntry>());
    }

    void Reader::print()
    {
        for (size_t i = 0; i < m_Hdr2.resCount; i++)
        {
            printf("type \"%.*s\"; name=\"%s\"", 4, m_ResEntries[i].resType.value, getName(&m_ResEntries[i]));

            if (!NameHash::validHash(getName(&m_ResEntries[i]), m_ResEntries[i].nameHash))
                printf(" (INVALID NAME HASH)");
            printf("\n");

            for (size_t j = m_ResEntries[i].firstFileIdx; j < m_ResEntries[i].firstFileIdx + m_ResEntries[i].fileCount; j++)
            {
                if (m_FileEntries[j].isDir())
                {
                    printf("(d) attr=0x%02X; name=\"%s\"; parent=\"%.*s\"",
                        m_FileEntries[j].attr,
                        &m_StrTab[m_FileEntries[j].name],
                        4,
                        m_FileEntries[j].parent == -1
                            ? "NULL" : 
                            m_ResEntries[m_FileEntries[j].parent].resType.value
                        );

                    if (!NameHash::validHash(getName(&m_FileEntries[j]), m_FileEntries[j].nameHash))
                        printf(" (INVALID NAME HASH)");
                    printf("\n");
                }
                else
                {
                    printf("(f) attr=0x%02X; name=\"%s\"; off=0x%X; size=0x%X\n",
                    m_FileEntries[j].attr,
                    &m_StrTab[m_FileEntries[j].name],
                    m_FileEntries[j].fileOff, m_FileEntries[j].fileSize);
                }
            }
        }
    }
    
    std::vector<u8> Reader::readFile(const FileEntry* entry) const
    {
        assert(!entry->isDir());
        m_Reader->seek(m_Hdr.headerSize + m_Hdr2.strTabOff + m_Hdr2.strTabSize + entry->fileOff);
        return m_Reader->readData(entry->fileSize);
    }

    ResEntry* Reader::getRootRes() const
    {
        for (auto& res : m_ResEntries)
            if (res.resType.matches("ROOT"), "ROOT")
                return const_cast<ResEntry*>(&res);
        /*
        for (auto& res : m_ResEntries)
        {
            for (size_t i = res.firstFileIdx; i < res.firstFileIdx + res.fileCount; i++)
            {
                auto file = m_FileEntries.data() + i;
                if (!strcmp(getName(file), "..") && file->isDir() && file->parent == -1)
                    return const_cast<ResEntry*>(&res);
            }
        }
        */
        return nullptr;
    }

    
    
    void Reader::iterFiles(std::function<void(u32, std::string, FileEntry*)> callback)
    {
        iterFiles(getRootRes(), 0, "", callback);
    }
    
    void Reader::iterFiles(ResEntry* res, u32 level, std::string resPath, std::function<void(u32, std::string, FileEntry*)> callback)
    {
        for (size_t i = res->firstFileIdx; i < res->firstFileIdx + res->fileCount; i++)
        {
            auto file = &m_FileEntries[i];

            // skip "." and ".."
            if (isSpecialDir(file))
                continue;

            std::string fullPath = resPath + "/" + getName(file);
            
            callback(level, fullPath, file);

            if (file->isDir())
                iterFiles(getRes(file), level+1, fullPath, callback);
        }
        
    }

}
