#include "Dol.hpp"

namespace Gc::Dol
{
    void DolHeader::print()
    {
        #define PRINT_SECTION(i)
        
        for (size_t i = 0; i < SECTION_COUNT; i++)
        {
            printf("Section [%02ld] (%s) : Address=%08X-%08X; Offset=%08X-%08X (0x%0X)\n",
                i, (i < TEXT_SECTION_COUNT) ? "r-x" : "rw-",
                sectionAddr[i], sectionAddr[i] + sectionSize[i], 
                sectionOff[i], sectionOff[i] + sectionSize[i], sectionSize[i]);
        }

        printf("BSS : %08X-%08X (0x%X)\n", bssAddr, bssAddr+bssSize, bssSize);
        
        #undef PRINT_SECTION
    }

    
    size_t DolHeader::getDolSize() const
    {
        u32 ret = 0;
        for (size_t i = 0; i < SECTION_COUNT; i++)
            ret = std::max(ret, sectionOff[i] + sectionSize[i]);

        return ret; 
    }
}