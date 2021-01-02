#pragma once

#include <vector>
#include <functional>
#include <string>
#include <memory>
#include "Types.h"
#include "Utils/Be.hpp"
#include "Utils/DataReader.hpp"
#include "Gcm.hpp"
#include "Dol.hpp"

namespace Gc
{
    namespace Gcm
    {    
        class Reader
        {
        public:
            Reader(std::unique_ptr<Utils::DataReader> reader);

            std::vector<u8> readDol() const;
            std::vector<u8> readAppLoader() const;

            FileEntry* getFileEntry(std::string path);
            std::vector<u8> readFile(FileEntry* entry) const;
            inline const char* getName(FileEntry* entry) const
            {
                return m_FstStrTable.data() + entry->nameOff;
            }

            void iterFst(std::function<void(u32, std::string, FileEntry*)> callback);
        private:
            size_t iterFst(u32 level, std::string parentPath, size_t i, size_t next, std::function<void(u32, std::string, FileEntry*)> callback);
            void parse();

        public:
            std::unique_ptr<Utils::DataReader> m_Reader;
            GcmHeader m_GcmHdr;
            Dol::DolHeader m_DolHdr;
            AppLoaderHeader m_AppLoaderHdr;
            std::vector<FileEntry> m_FileEntries;

        private:
            std::vector<char> m_FstStrTable;
        };
    };
}