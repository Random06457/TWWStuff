#include "RelReader.hpp"

namespace Gc::Rel
{
    Reader::Reader(std::unique_ptr<Utils::DataReader> reader) :
        m_Reader(std::move(reader)),
        m_StrTable(nullptr)
    {
        parse();
    }

    void Reader::parse()
    {
        // read header
        m_Reader->readStrucBe(&m_Hdr);

        // read sections
        m_Reader->seek(m_Hdr.sectionsOff);
        m_Sections.reserve(m_Hdr.sectionCount);
        for (size_t i = 0; i < m_Hdr.sectionCount; i++)
            m_Sections.push_back(m_Reader->readStrucBe<SectionEntry>());
        
        // read imports
        m_Reader->seek(m_Hdr.importOff);
        size_t count = m_Hdr.importSize / sizeof(ImportEntry);
        m_Sections.reserve(count);
        for (size_t i = 0; i < count; i++)
            m_Imports.push_back(m_Reader->readStrucBe<ImportEntry>());
    }

    size_t Reader::getRelocCount(const ImportEntry* entry) const
    {
        m_Reader->seek(entry->off);
        size_t count;

        for (count = 0; m_Reader->readStrucBe<RelocEntry>().type != 0xCB; count++)
            ;
        
        return count;
    }
    std::vector<RelocEntry> Reader::getRelocEntries(const ImportEntry* entry) const
    {
        m_Reader->seek(entry->off);
        std::vector<RelocEntry> relocs;

        RelocEntry rel;
        do
        {
            m_Reader->readStrucBe(&rel);
            relocs.push_back(rel);
        } while (rel.type != 0xCB);
        
        return relocs;
    }

    void Reader::print() const
    {
        printf("Module Id : %d\n", m_Hdr.id);
        if (m_StrTable)
            printf("Module name : %.*s\n", m_Hdr.nameSize, m_StrTable + m_Hdr.nameOff);
        printf("Version : %d\n", m_Hdr.version);
        printf("init : 0x%08X\n", m_Hdr.initFunc);
        printf("fini : 0x%08X\n", m_Hdr.finiFunc);
        printf("unresolved : 0x%08X\n", m_Hdr.unresolved);
        printf("Align : 0x%X\n", m_Hdr.align);
        printf("BSS Align : 0x%X\n", m_Hdr.bssAlign);
        printf("Size : 0x%X\n", m_Hdr.fixSize);

        printf("\n");
        printf("Sections:\n");
        for (size_t i = 0; i < m_Sections.size(); i++)
        {
            auto& sec = m_Sections[i];
            printf("\tSection [%ld] (%s) %08X-%08X(0x%X) ", i,
                sec.isExec() ? "r-x" : "rw-",
                sec.getOff(), sec.getOff()+sec.size,
                sec.size);
            
            bool first = true;
            if (i == m_Hdr.initArraySection)
            {
                printf(first ? "(" : "/");
                printf(".init_array");
                first = false;
            }
            if (i == m_Hdr.finiArraySection)
            {
                printf(first ? "(" : "/");
                printf(".fini_array");
                first = false;
            }
            if (i == m_Hdr.initArraySection)
            {
                printf(first ? "(" : "/");
                printf("unresolved");
                first = false;
            }
            if (!first)
                printf(")");

            if (sec.getOff() == 0)
            {
                printf(i == m_Hdr.bssSection ? "(.bss)" : "(UNUSED)");
            }


            printf("\n");
        }

        printf("\n");
        printf("Imports:\n");
        for (auto& imp : m_Imports)
            printf("\tModule [%d] (%ld reloc entries)\n", imp.id, getRelocCount(&imp));
    }
}