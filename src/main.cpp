#include <stdio.h>
#include <cstring>
#include <functional>
#include "Utils/Be.hpp"
#include "Utils/Fs.hpp"
#include "Utils/Str.hpp"
#include "Gc/GcmReader.hpp"
#include "Macros.h"
#include "Args.hpp"
#include "Zelda/Yaz0.hpp"
#include "Zelda/ArcReader.hpp"



Gc::Gcm::Reader getGcmReader(ArgReader* args)
{
    auto path = args->read();
    assert(Utils::Fs::fileExists(path));
    return Gc::Gcm::Reader(path);
}

void gcmInfoHandler(ArgReader* args)
{
    auto gcmReader = getGcmReader(args);

    printf("GCM Image Header:\n");
    gcmReader.m_GcmHdr.print();
    printf("\n");
    printf("DOL Header:\n");
    gcmReader.m_DolHdr.print();
    printf("\n");
    printf("AppLoader:\n");
    gcmReader.m_AppLoaderHdr.print();
}

void gcmExtractHandler(ArgReader* args)
{
    auto extractArg = args->read();
    auto gcmReader = getGcmReader(args);
    auto outPath = args->read();

    auto lowerCaseArg = Utils::Str::toLower(extractArg);
    if (lowerCaseArg == "dol")
    {
        auto dolData = gcmReader.readDol();
        Utils::Fs::writeFile(outPath, dolData.data(), dolData.size());
    }
    if (lowerCaseArg == "fst")
    {
        auto fstBuffer = gcmReader.readData(gcmReader.m_GcmHdr.fstOff, gcmReader.m_GcmHdr.fstSize);
        Utils::Fs::writeFile(outPath, fstBuffer.data(), fstBuffer.size());
    }
    if (lowerCaseArg == "fs")
    {
        gcmReader.iterFst([&gcmReader, outPath] (u32 level, std::string parentPath, std::string fullPath, Gc::Gcm::FileEntry* entry) {
            if (entry->isDir)
                Utils::Fs::createDir(outPath + fullPath);
            else
            {
                auto data = gcmReader.readData(entry->file.fileOff, entry->file.fileSize);
                Utils::Fs::writeFile(outPath + fullPath, data.data(), data.size());
                printf("Writing %s...\n", fullPath.c_str());
            }
        });
    }
    if (lowerCaseArg == "apploader")
    {
        auto appLoader = gcmReader.readAppLoader();
        Utils::Fs::writeFile(outPath, appLoader.data(), appLoader.size());
    }
    if (lowerCaseArg.find("fs:") == 0)
    {
        auto file = gcmReader.getFileEntry(extractArg.substr(3));
        assert(file);
        auto data = gcmReader.readFile(file);
        Utils::Fs::writeFile(outPath, data.data(), data.size());
    }
}

void gcmLsHandler(ArgReader* args)
{
    auto gcmReader = getGcmReader(args);

    gcmReader.iterFst([] (u32 level, std::string parentPath, std::string fullPath, Gc::Gcm::FileEntry* entry) {
        if (!entry->isDir)
            printf("%s\n", fullPath.c_str());
    });
}

void gcmFsTreeHandler(ArgReader* args)
{
    auto gcmReader = getGcmReader(args);

    gcmReader.iterFst([&gcmReader] (u32 level, std::string parentPath, std::string fullPath, Gc::Gcm::FileEntry* entry) {
        for (size_t j = 0; j < level; j++)
            printf("    ");

        printf("%s\n", gcmReader.getName(entry));
    });
}

void gcmTest(ArgReader* args)
{
    auto gcmReader = getGcmReader(args);

    auto file = gcmReader.getFileEntry("/opening.bnr");
    assert(file);
    auto mapFile = gcmReader.readFile(file);
    std::string str = std::string(reinterpret_cast<char*>(mapFile.data()), mapFile.size());
    printf("%s\n", str.c_str());
}

void arcTest(ArgReader* args)
{
    auto path = args->read();
    assert(Utils::Fs::fileExists(path));

    auto rawArc = Utils::Fs::readFile(path);
    u8* ptr = rawArc.data();

    Zelda::ArcHeader* hdr = reinterpret_cast<Zelda::ArcHeader*>(ptr);
    hdr->bomSwap();
    ptr += hdr->headerSize;
    Zelda::ArcHeader2* hdr2 = reinterpret_cast<Zelda::ArcHeader2*>(ptr);
    hdr2->bomSwap();
    const char* strTab = reinterpret_cast<const char*>(ptr) + hdr2->strTabOff;
    Zelda::ArcResEntry* res = reinterpret_cast<Zelda::ArcResEntry*>(ptr + hdr2->resOff);
    Zelda::ArcFileEntry* file = reinterpret_cast<Zelda::ArcFileEntry*>(ptr + hdr2->fileOff);
    for (size_t i = 0; i < hdr2->resCount; i++)
    {
        res[i].bomSwap();
        printf("type \"%s\"; name=\"%s\"\n", std::string(res[i].resType, 4).c_str(), strTab + res[i].name);

        for (size_t j = res[i].firstFileIdx; j < res[i].firstFileIdx + res[i].fileCount; j++)
        {
            file[j].bomSwap();
            if (file[j].attr & ATTR_DIR)
            {
                printf("(d) attr=0x%02X; name=\"%s\"; parent=\"%s\"\n",
                    file[j].attr,
                    strTab + file[j].name,
                    file[j].parent == -1
                        ? "<NULL>" : 
                        std::string(res[file[j].parent].resType, 4).c_str()
                    );
            }
            else
            {
                printf("(f) attr=0x%02X; name=\"%s\"; off=0x%X; size=0x%X\n",
                file[j].attr,
                strTab + file[j].name,
                file[j].fileOff, file[j].fileSize);
            }
        }
    }
}

void yaz0Decompress(ArgReader* args)
{
    auto input = args->read();
    auto output = args->read();

    auto data = Utils::Fs::readFile(input);
    data = Zelda::Yaz0::decompress(data);
    Utils::Fs::writeFile(output, data.data(), data.size());
}

std::vector<ArgHandler> g_gcmCmdHandler = {
    { "info", &gcmInfoHandler },
    { "extract", &gcmExtractHandler },
    { "ls", &gcmLsHandler },
    { "tree", &gcmFsTreeHandler },
    { "test", &gcmTest },
};

std::vector<ArgHandler> g_arcCmdHandler = {
    { "test", &arcTest },  
};

std::vector<ArgHandler> g_yaz0CmdHandler = {
    { "decompress", &yaz0Decompress },  
};

std::vector<ArgHandler> g_fileTypeHandler = {
    { "gcm", std::bind(&ArgReader::processHandlers, std::placeholders::_1, g_gcmCmdHandler) },
    { "arc", std::bind(&ArgReader::processHandlers, std::placeholders::_1, g_arcCmdHandler) },
    { "yaz0", std::bind(&ArgReader::processHandlers, std::placeholders::_1, g_yaz0CmdHandler) },
};



int main(s32 argc, const char** argv)
{
    ArgReader args(argc, argv);
    args.processHandlers(g_fileTypeHandler);
    
    return 0;
}