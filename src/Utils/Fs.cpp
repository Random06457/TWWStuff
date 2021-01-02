#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Types.h"
#include "Fs.hpp"

namespace Utils
{
    bool Fs::writeFile(std::string path, void* data, size_t size)
    {
        FILE* f = fopen(path.c_str(), "wb");
        assert(f);

        size_t written = 0;
        while (written < size)
            written += fwrite(reinterpret_cast<u8*>(data) + written, 1, size-written, f);
        
        fclose(f);
        return true;
    }

    std::vector<u8> Fs::readFile(std::string path)
    {
        FILE* f = fopen(path.c_str(), "rb");
        assert(f);

        fseek(f, 0, SEEK_END);
        size_t size = ftell(f);
        fseek(f, 0, SEEK_SET);

        std::vector<u8> ret(size);

        size_t read = 0;
        while (read < size)
            read += fread(ret.data() + read, 1, size-read, f);

        fclose(f);

        return ret;
    }

    bool Fs::fileExists(std::string path)
    {
        return !access(path.c_str(), F_OK);
    }

    void Fs::createDir(std::string path)
    {
        for (size_t i = 0; i < path.size()+1; i++)
            if (i == path.size() || path[i] == '/' || path[i] == '\\')
            {
#ifdef _WIN32
                s32 res = mkdir(std::string(path, 0, i).c_str());
#else
                s32 res = mkdir(std::string(path, 0, i).c_str(), 0777);
#endif
                assert(!res || errno == EEXIST);
            }
    }
}