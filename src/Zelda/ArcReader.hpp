#pragma once

#include <string>
#include <memory>
#include <functional>
#include "Archive.hpp"
#include "Utils/DataReader.hpp"

namespace Zelda::Arc
{
    class Reader
    {
    public:
        Reader(std::unique_ptr<Utils::DataReader> reader);

        void print();
        
        ResEntry* getRootRes() const;
        void iterFiles(std::function<void(u32, std::string, FileEntry*)> callback);

        std::vector<u8> readFile(const FileEntry* entry) const;

        inline const char* getName(const FileEntry* entry) const
        {
            return &m_StrTab[entry->name];
        }
        inline const char* getName(const ResEntry* entry) const
        {
            return &m_StrTab[entry->name];
        }
        inline ResEntry* getRes(const FileEntry* dir) const
        {
            assert(dir->isDir());
            return const_cast<ResEntry*>(&m_ResEntries[dir->parent]);
        }
        inline bool isCurrentDir(const FileEntry* dir) const
        {
            return dir->isDir() && !strcmp(getName(dir), ".");
        }
        inline bool isParentDir(const FileEntry* dir) const
        {
            return dir->isDir() && !strcmp(getName(dir), "..");
        }
        inline bool isSpecialDir(const FileEntry* dir) const
        {
            return isCurrentDir(dir) || isParentDir(dir);
        }

    private:
        void readHeaders();
        void iterFiles(ResEntry* res, u32 level, std::string parentPath, std::function<void(u32, std::string, FileEntry*)> callback);

    public:
        std::unique_ptr<Utils::DataReader> m_Reader;

    private:
        ArcHeader m_Hdr;
        ArcHeader2 m_Hdr2;
        std::vector<ResEntry> m_ResEntries;
        std::vector<FileEntry> m_FileEntries;
        std::vector<char> m_StrTab;
    };
};

