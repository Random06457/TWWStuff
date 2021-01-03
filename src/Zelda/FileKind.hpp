#pragma once

#include "Types.h"
#include "Gc/GcmReader.hpp"

namespace Zelda
{
    enum class FileKind : u32
    {
        Yaz0 = (1u << 31u),
        Unknown = 1u,
        Bdl,
        Bfn,
        Arc,
        Bnr,
        Bti,
        Dds,
        Jpc,
    };

    inline FileKind operator|(FileKind a, FileKind b)
    {
        return static_cast<FileKind>(static_cast<u32>(a) | static_cast<u32>(b));
    }
    inline FileKind& operator|=(FileKind& a, const FileKind& b)
    {
        a = a | b;
        return a;
    }
    inline FileKind operator&(FileKind a, FileKind b)
    {
        return static_cast<FileKind>(static_cast<u32>(a) & static_cast<u32>(b));
    }

    inline FileKind operator~(FileKind a)
    {
        return static_cast<FileKind>(~static_cast<u32>(a));
    }

    bool isCompressed(Gc::Gcm::Reader* reader, Gc::Gcm::FileEntry* entry);
    FileKind getFileKind(Gc::Gcm::Reader* reader, Gc::Gcm::FileEntry* entry);
};