#pragma once
#include <assert.h>
#include <vector>
#include <functional>
#include "Utils/Str.hpp"

class ArgReader;

struct ArgHandler
{
    const char* name;
    std::function<void(ArgReader*)> callback;
};

class ArgReader
{
public:
    ArgReader(size_t argc, const char** argv) : m_Argc(argc), m_Argv(argv), m_Idx(1)
    { }

    inline const char* getProgramName() const { return m_Argv[0]; }
    inline bool canRead() const { return m_Idx < m_Argc; }
    inline std::string read(bool lowerCase = false)
    {
        assert(canRead());
        std::string ret(m_Argv[m_Idx++]);
        return lowerCase ? Utils::Str::toLower(ret) : ret;
    }

    void processHandlers(std::vector<ArgHandler> list);

private:
    size_t m_Argc;
    const char** m_Argv;
    size_t m_Idx;
};