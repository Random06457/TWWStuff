#pragma once

#include <vector>
#include <functional>
#include <string>
#include "../Types.h"
#include "../Utils/Be.hpp"
#include "../Utils/DataReader.hpp"
#include "Gcm.hpp"
#include "Dol.hpp"

namespace Gc
{
    namespace Gcm
    {    
        class Reader
        {
        public:
            Reader(std::string path);
            Reader(FILE* f);
            ~Reader();

            std::vector<u8> readData(size_t off, size_t size) const;
            std::vector<u8> readDol() const;
            std::vector<u8> readAppLoader() const;

            FileEntry* getFileEntry(std::string path);
            const char* getName(FileEntry* entry) const;
            std::vector<u8> readFile(FileEntry* entry) const;
            void iterFst(std::function<void(u32, std::string, std::string, FileEntry*)> callback);
        private:
            size_t iterFst(u32 level, std::string parentPath, size_t i, size_t next, std::function<void(u32, std::string, std::string, FileEntry*)> callback);
            void readHeaders();
            void seek(size_t addr) const;

        public:
            Utils::DataReader* m_Reader;
            GcmHeader m_GcmHdr;
            Dol::DolHeader m_DolHdr;
            AppLoaderHeader m_AppLoaderHdr;
            std::vector<FileEntry> m_FileEntries;

        private:
            FILE* m_File;
            std::vector<char> m_FstStrTable;
        };
    };
}