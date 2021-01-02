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
#include "Zelda/Yaz0.hpp"
#include "Zelda/ArcReader.hpp"
#include "Zelda/BdlReader.hpp"
#include "Gc/RelReader.hpp"
#include "Zelda/BtiReader.hpp"
#include "3rdparty/lodepng.h"
#include "Gc/GxTexture.hpp"
#include "Zelda/BfnReader.hpp"
#include "CmdMain.hpp"

namespace CmdMain
{

    std::unique_ptr<Utils::DataReader> getFileReader(ArgReader* args, bool decompress = true)
    {
        auto path = args->read();
        assert(Utils::Fs::fileExists(path));
        auto fsReader = std::make_unique<Utils::FileReader>(path);
        
        Zelda::Yaz0::Yaz0Header hdr;
        fsReader->readStrucBe(&hdr);
        fsReader->seek(0);
        
        return (decompress && hdr.valid())
            ? Zelda::Yaz0::createReader(std::move(fsReader))
            : std::move(fsReader);
    }

    void gcmInfoHandler(ArgReader* args)
    {
        Gc::Gcm::Reader reader(getFileReader(args));

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
        Gc::Gcm::Reader reader(getFileReader(args));
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
            reader.iterFst([&reader, &outPath] (u32 level, std::string fullPath, Gc::Gcm::FileEntry* entry) {
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
        Gc::Gcm::Reader reader(getFileReader(args));

        reader.iterFst([] (u32 level, std::string fullPath, Gc::Gcm::FileEntry* entry) {
            if (!entry->isDir)
                printf("%s\n", fullPath.c_str());
        });
    }

    void gcmFsTreeHandler(ArgReader* args)
    {
        Gc::Gcm::Reader reader(getFileReader(args));

        reader.iterFst([&reader] (u32 level, std::string fullPath, Gc::Gcm::FileEntry* entry) {
            for (size_t j = 0; j < level; j++)
                printf("    ");

            printf("%s\n", reader.getName(entry));
        });
    }

    void gcmTestHandler(ArgReader* args)
    {
        Gc::Gcm::Reader reader(getFileReader(args));

        auto file = reader.getFileEntry("/opening.bnr");
        assert(file);
        auto mapFile = reader.readFile(file);
        std::string str = std::string(reinterpret_cast<char*>(mapFile.data()), mapFile.size());
        printf("%s\n", str.c_str());
    }

    void arcInfoHandler(ArgReader* args)
    {
        Zelda::Arc::Reader reader(getFileReader(args));

        reader.print();
    }

    void arcLsHandler(ArgReader* args)
    {
        Zelda::Arc::Reader reader(getFileReader(args));

        reader.iterFiles([&reader] (u32 level, std::string path, Zelda::Arc::FileEntry* entry) {
            if (!entry->isDir())
                printf("%s\n", path.c_str());
        });
    }

    void arcTreeHandler(ArgReader* args)
    {
        Zelda::Arc::Reader reader(getFileReader(args));

        reader.iterFiles([&reader] (u32 level, std::string path, Zelda::Arc::FileEntry* entry) {

            for (size_t i = 0; i < level; i++)
                printf("    ");

            printf("%s\n", reader.getName(entry));
        });
    }

    void arcExtractHandler(ArgReader* args)
    {
        Zelda::Arc::Reader reader(getFileReader(args));
        auto outPath = args->read();
        
        Utils::Fs::createDir(outPath);

        reader.iterFiles([&reader, &outPath] (u32 level, std::string path, Zelda::Arc::FileEntry* entry) {
            if (entry->isDir())
                    Utils::Fs::createDir(outPath + path);
            else
            {
                auto data = reader.readFile(entry);
                printf("Writing %s...\n", path.c_str());
                Utils::Fs::writeFile(outPath + path, data.data(), data.size());
            }
        });
    }

    void yaz0Decompress(ArgReader* args)
    {
        auto input = args->read();
        auto output = args->read();

        auto data = Utils::Fs::readFile(input);
        data = Zelda::Yaz0::decompress(data);
        Utils::Fs::writeFile(output, data.data(), data.size());
    }

    void bdlInfoHandler(ArgReader* args)
    {
        Zelda::Bdl::Reader reader(getFileReader(args));
    }

    void fileHandler(ArgReader* args)
    {
        auto reader = getFileReader(args, false);

        char buff[0x10];
        reader->readData(buff, sizeof(buff));
        reader->seek(0);

        if (reinterpret_cast<Zelda::Yaz0::Yaz0Header*>(buff)->valid())
            printf("Yaz0 Compressed File\n");
        else if (reinterpret_cast<Zelda::Arc::ArcHeader*>(buff)->valid())
            printf("RARC Archive\n");
        else if (reinterpret_cast<Zelda::Bdl::BdlHeader*>(buff)->valid())
            printf("Binary Display List File\n");
        else
            printf("Unknown\n");
        
    }

    void dolInfoHandler(ArgReader* args)
    {
        auto reader = getFileReader(args);

        Gc::Dol::DolHeader hdr;
        reader->readStrucBe(&hdr);
        hdr.print();
    }

    void relInfoHandler(ArgReader* args)
    {
        Gc::Rel::Reader rel(getFileReader(args));

        if (args->canRead())
        {
            auto strTable = getFileReader(args);
            auto str = strTable->readData(strTable->getSize());
            rel.setStringble(reinterpret_cast<char*>(str.data()));
        }

        rel.print();
    }


    void btiInfoHandler(ArgReader* args)
    {
        Zelda::Bti::Reader reader(getFileReader(args));
        reader.print();
    }

    void btiToPngHandler(ArgReader* args)
    {
        Zelda::Bti::Reader reader(getFileReader(args));
        auto outPath = args->read();

        auto rgba = reader.toRGBA8();
        lodepng::encode(outPath, rgba, reader.m_Hdr.width, reader.m_Hdr.height);
    }

    void ddsToPngHandler(ArgReader* args)
    {
        const size_t w = 152, h = 104;
        const u8 fmt = GX_TF_CMPR;

        auto reader = getFileReader(args);
        auto outPath = args->read();

        assert(reader->getSize() == w*h*4/8);

        reader->seek(0);
        auto data = reader->readData(reader->getSize());
        
        auto rgba = Gc::Gx::Texture::convertToRGBA8(data.data(), w, h, fmt);
        lodepng::encode(outPath, rgba, w, h);
    }

    void bfnInfoHandler(ArgReader* args)
    {
        Zelda::Bfn::Reader reader(getFileReader(args));
        
        reader.print();
    }
    void bfnToPngHandler(ArgReader* args)
    {
        Zelda::Bfn::Reader reader(getFileReader(args));
        auto outPath = args->read();

        size_t width, height;
        auto rgba = reader.getTexture(&width, &height);
        lodepng::encode(outPath, rgba, width, height);
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

    std::vector<ArgHandler> g_bdlCmdHandler = {
        { "info", &bdlInfoHandler },  
    };

    std::vector<ArgHandler> g_dolCmdHandler = {
        { "info", &dolInfoHandler },  
    };

    std::vector<ArgHandler> g_relCmdHandler = {
        { "info", &relInfoHandler },  
    };

    std::vector<ArgHandler> g_btiCmdHandler = {
        { "info", &btiInfoHandler },
        { "topng", &btiToPngHandler },
    };

    std::vector<ArgHandler> g_ddsCmdHandler = {
        { "topng", &ddsToPngHandler },
    };

    std::vector<ArgHandler> g_bfnCmdHandler = {
        { "info", &bfnInfoHandler },
        { "topng", &bfnToPngHandler },
    };

    std::vector<ArgHandler> g_fileTypeHandler = {
        { "gcm", std::bind(&ArgReader::processHandlers, std::placeholders::_1, g_gcmCmdHandler) },
        { "arc", std::bind(&ArgReader::processHandlers, std::placeholders::_1, g_arcCmdHandler) },
        { "yaz0", std::bind(&ArgReader::processHandlers, std::placeholders::_1, g_yaz0CmdHandler) },
        { "bdl", std::bind(&ArgReader::processHandlers, std::placeholders::_1, g_bdlCmdHandler) },
        { "rel", std::bind(&ArgReader::processHandlers, std::placeholders::_1, g_relCmdHandler) },
        { "dol", std::bind(&ArgReader::processHandlers, std::placeholders::_1, g_dolCmdHandler) },
        { "bti", std::bind(&ArgReader::processHandlers, std::placeholders::_1, g_btiCmdHandler) },
        { "dds", std::bind(&ArgReader::processHandlers, std::placeholders::_1, g_ddsCmdHandler) },
        { "bfn", std::bind(&ArgReader::processHandlers, std::placeholders::_1, g_bfnCmdHandler) },
        { "file", &fileHandler },
    };
}