#include <cstring>
#include "Args.hpp"

void ArgReader::processHandlers(std::vector<ArgHandler> handlers)
{
    auto mode = read(true);
    for (auto& entry : handlers)
    {
        if (!strcmp(mode.c_str(), entry.name))
        {
            entry.callback(this);
            break;
        }
    }
}