#pragma once

#include <vector>
#include "../Types.h"
#include "Gx.hpp"
#include "../Utils/Be.hpp"


namespace Gc::Gx::Texture
{
    std::vector<u8> convertToRGBA8(const void* data, size_t width, size_t height, u8 fmt, const void* tlut = nullptr, u8 tlutFmt = 0xFF);
    size_t getTexSize(u8 fmt, u16 width, u16 height);
}
