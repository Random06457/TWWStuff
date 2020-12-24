#pragma once
#include <cstring>

namespace Utils
{
    template<int N>
    struct Signature
    {
        char value[N];

        inline bool matches(const char* data) const
        {
            return !memcmp(value, data, N);
        }
    };
}