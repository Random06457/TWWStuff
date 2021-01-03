#include "FileKind.hpp"
#include "Yaz0.hpp"
#include "Bdl.hpp"
#include "Bfn.hpp"
#include "Archive.hpp"

namespace Zelda
{
    
    bool isCompressed(Gc::Gcm::Reader* reader, Gc::Gcm::FileEntry* entry)
    {
        assert(!entry->isDir);
        
        auto seek = reader->m_Reader->scopedSeek(entry->file.fileOff);

        Zelda::Yaz0::Yaz0Header hdr;
        reader->m_Reader->readStrucBe(&hdr);
        return hdr.valid();
    }

    FileKind getFileKind(Gc::Gcm::Reader* reader, Gc::Gcm::FileEntry* entry)
    {
        assert(!entry->isDir);

        FileKind ret = static_cast<FileKind>(0);

        auto seek = reader->m_Reader->scopedSeek(entry->file.fileOff);

        char magic[8];
        reader->m_Reader->readData(magic, sizeof(magic));

#define MAGIC_MATCHES(str) (!strncmp(magic, str, sizeof(str)-1))

        if (MAGIC_MATCHES(YAZ0_MAGIC))
        {
            char buff[0x18];
            reader->m_Reader->seek(entry->file.fileOff);
            reader->m_Reader->readData(buff, sizeof(buff));

            Zelda::Yaz0::decompress(buff, magic, sizeof(magic));
            ret = FileKind::Yaz0;
        }

        if (MAGIC_MATCHES(BDL3_MAGIC))
            ret |= FileKind::Bdl;
        else if (MAGIC_MATCHES(BFN_MAGIC))
            ret |= FileKind::Bfn;
        else if (MAGIC_MATCHES(ARC_MAGIC))
            ret |= FileKind::Arc;
        else
            ret |= FileKind::Unknown;
        
#undef MAGIC_MATCHES
        return ret;
    }
};