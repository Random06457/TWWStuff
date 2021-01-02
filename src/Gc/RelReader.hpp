#pragma once
#include <memory>
#include "Utils/DataReader.hpp"
#include "Rel.hpp"

namespace Gc::Rel
{
    class Reader
    {
    public:
        Reader(std::unique_ptr<Utils::DataReader> reader);

        std::vector<RelocEntry> getRelocEntries(const ImportEntry* entry) const;
        size_t getRelocCount(const ImportEntry* entry) const;
        void print() const;
        
        inline void setStringble(const char* strTable)
        {
            m_StrTable = strTable;
        }

    private:
        void parse();
        
    public:
        std::unique_ptr<Utils::DataReader> m_Reader;
        RelHeader m_Hdr;
        std::vector<SectionEntry> m_Sections;
        std::vector<ImportEntry> m_Imports;

    private:
        const char* m_StrTable;
        
    };
    
}