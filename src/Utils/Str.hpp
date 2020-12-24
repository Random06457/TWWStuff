#pragma once
#include <cctype>
#include <string>
#include <algorithm>

namespace Utils
{
    class Str
    {
    public:
        static std::string toLower(std::string str)
        {
            std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char c){ return std::tolower(c); });
            return str;
        }
    };
};
