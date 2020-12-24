#include "Gcm.hpp"

namespace Gc::Gcm
{

    void GcmHeader::read(FILE* f, bool be)
    {
        fread(this, 1, sizeof(GcmHeader), f);
        if (be)
            bomSwap();
    }

    void GcmHeader::bomSwap()
    {
        BOM_SWAP(unk_1C);
        BOM_SWAP(debugMonitorOff);
        BOM_SWAP(debugMonitorAddr);
        BOM_SWAP(dolOff);
        BOM_SWAP(fstOff);
        BOM_SWAP(fstSize);
        BOM_SWAP(fstMaxSize);
        BOM_SWAP(userPosition);
        BOM_SWAP(userLength);
    }

    void GcmHeader::print()
    {
        printf("Game Code : %c%c%c%c%c%c\n", gameCode[0], gameCode[1], gameCode[2], gameCode[3], gameCode[4], gameCode[5]);
        printf("Disk ID : 0x%02X\n", diskID);
        printf("Version : 0x%02X\n", version);
        printf("Streaming : 0x%02X\n", streaming);
        printf("Stream BufferSize : 0x%02X\n", streamBufSize);
        printf("Game Name : \"%s\"\n", gameName);
        printf("Debug Monitor Offset : 0x%08X\n", debugMonitorOff);
        printf("Debug Monitor Address : 0x%08X\n", debugMonitorAddr);
        printf("DOL Offset : 0x%08X\n", dolOff);
        printf("FST Offset : 0x%08X\n", fstOff);
        printf("FST Size : 0x%08X\n", fstSize);
        printf("FST Max Size : 0x%08X\n", fstMaxSize);
        printf("User Position : 0x%08X\n", userPosition);
        printf("User Size : 0x%08X\n", userLength);
    }

    void AppLoaderHeader::read(FILE* f, bool be)
    {
        fread(this, 1, sizeof(AppLoaderHeader), f);
        if (be)
            bomSwap();
    }

    void AppLoaderHeader::bomSwap()
    {
        BOM_SWAP(entrypoint);
        BOM_SWAP(size);
        BOM_SWAP(unk_18);
    }

    void AppLoaderHeader::print()
    {
        printf("Date : \"%s\"\n", date);
        printf("Address : %08X-%08X (0x%X)\n", entrypoint, entrypoint+size, size);
    }

    void FileEntry::read(FILE* f, bool be)
    {
        fread(this, 1, sizeof(FileEntry), f);
        if (be)
            bomSwap();
    }

    void FileEntry::bomSwap()
    {
        Utils::Be::bomSwap(reinterpret_cast<u8*>(this)+1, 3);
        BOM_SWAP(dir.parentOff);
        BOM_SWAP(dir.nextOff);
    }

}