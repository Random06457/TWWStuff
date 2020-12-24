#pragma once

#include <string>
#include <vector>

namespace Utils
{
    class Fs
    {
    public:

        static bool fileExists(std::string path);
        static bool writeFile(std::string path, void* data, size_t size);
        static std::vector<u8> readFile(std::string path);
        static void createDir(std::string path);
    };
}
