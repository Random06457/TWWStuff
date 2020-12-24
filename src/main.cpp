#include <stdio.h>
#include <cstring>
#include <functional>
#include <memory>
#include "Utils/Be.hpp"
#include "Utils/Fs.hpp"
#include "Utils/Str.hpp"
#include "Utils/FileReader.hpp"
#include "Gc/GcmReader.hpp"
#include "Macros.h"
#include "Args.hpp"
#include "Zelda/Yaz0.hpp"
#include "Zelda/ArcReader.hpp"


template <class T>
T getReader(ArgReader* args)
{
    auto path = args->read();
    assert(Utils::Fs::fileExists(path));
    return T(path);
}

void gcmInfoHandler(ArgReader* args)
{
    auto reader = getReader<Gc::Gcm::Reader>(args);

    printf("GCM Image Header:\n");
    reader.m_GcmHdr.print();
    printf("\n");
    printf("DOL Header:\n");
    reader.m_DolHdr.print();
    printf("\n");
    printf("AppLoader:\n");
    reader.m_AppLoaderHdr.print();
}

void gcmExtractHandler(ArgReader* args)
{
    auto extractArg = args->read();
    auto reader = getReader<Gc::Gcm::Reader>(args);
    auto outPath = args->read();

    auto lowerCaseArg = Utils::Str::toLower(extractArg);
    if (lowerCaseArg == "dol")
    {
        auto dolData = reader.readDol();
        Utils::Fs::writeFile(outPath, dolData.data(), dolData.size());
    }
    if (lowerCaseArg == "fst")
    {
        reader.m_Reader->seek(reader.m_GcmHdr.fstOff);
        auto fstBuffer = reader.m_Reader->readData(reader.m_GcmHdr.fstSize);
        Utils::Fs::writeFile(outPath, fstBuffer.data(), fstBuffer.size());
    }
    if (lowerCaseArg == "fs")
    {
        reader.iterFst([&reader, outPath] (u32 level, std::string fullPath, Gc::Gcm::FileEntry* entry) {
            if (entry->isDir)
                Utils::Fs::createDir(outPath + fullPath);
            else
            {
                auto data = reader.readFile(entry);
                Utils::Fs::writeFile(outPath + fullPath, data.data(), data.size());
                printf("Writing %s...\n", fullPath.c_str());
            }
        });
    }
    if (lowerCaseArg == "apploader")
    {
        auto appLoader = reader.readAppLoader();
        Utils::Fs::writeFile(outPath, appLoader.data(), appLoader.size());
    }
    if (lowerCaseArg.find("fs:") == 0)
    {
        auto file = reader.getFileEntry(extractArg.substr(3));
        assert(file);
        auto data = reader.readFile(file);
        Utils::Fs::writeFile(outPath, data.data(), data.size());
    }
}

void gcmLsHandler(ArgReader* args)
{
    auto reader = getReader<Gc::Gcm::Reader>(args);

    reader.iterFst([] (u32 level, std::string fullPath, Gc::Gcm::FileEntry* entry) {
        if (!entry->isDir)
            printf("%s\n", fullPath.c_str());
    });
}

void gcmFsTreeHandler(ArgReader* args)
{
    auto reader = getReader<Gc::Gcm::Reader>(args);

    reader.iterFst([&reader] (u32 level, std::string fullPath, Gc::Gcm::FileEntry* entry) {
        for (size_t j = 0; j < level; j++)
            printf("    ");

        printf("%s\n", reader.getName(entry));
    });
}

void gcmTestHandler(ArgReader* args)
{
    auto reader = getReader<Gc::Gcm::Reader>(args);

    auto file = reader.getFileEntry("/opening.bnr");
    assert(file);
    auto mapFile = reader.readFile(file);
    std::string str = std::string(reinterpret_cast<char*>(mapFile.data()), mapFile.size());
    printf("%s\n", str.c_str());
}

void arcInfoHandler(ArgReader* args)
{
    auto reader = getReader<Zelda::Arc::Reader>(args);

    reader.print();
}

void arcLsHandler(ArgReader* args)
{
    auto reader = getReader<Zelda::Arc::Reader>(args);

    reader.iterFiles([&reader] (u32 level, std::string path, Zelda::Arc::FileEntry* entry) {
        if (!entry->isDir())
            printf("%s\n", path.c_str());
    });
}

void arcTreeHandler(ArgReader* args)
{
    auto reader = getReader<Zelda::Arc::Reader>(args);

    reader.iterFiles([&reader] (u32 level, std::string path, Zelda::Arc::FileEntry* entry) {

        for (size_t i = 0; i < level; i++)
            printf("    ");

        printf("%s\n", reader.getName(entry));
    });
}

void arcExtractHandler(ArgReader* args)
{
    assert(0 && "not implemented");
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
    { "test", &gcmTestHandler },
};

std::vector<ArgHandler> g_arcCmdHandler = {
    { "info", &arcInfoHandler },
    { "extract", &arcExtractHandler },
    { "ls", &arcLsHandler },
    { "tree", &arcTreeHandler },
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