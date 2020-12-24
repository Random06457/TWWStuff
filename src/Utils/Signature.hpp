#pragma once

namespace Utils
{
    template<int N>
    struct Signature
    {
        char value[N];

        inline void matches(const char* data) const
        {
            return !memcmp(value, data, N);
        }
    };
}