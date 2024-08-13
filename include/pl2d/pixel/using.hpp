#pragma once
#include "define.hpp"

namespace pl2d {

template <BasePixelTemplate>
struct BasePixel;

using PixelB = BasePixelBT; // byte
using PixelS = BasePixelST; // short
using PixelI = BasePixelIT; // int
using PixelF = BasePixelFT; // float
using PixelD = BasePixelDT; // double
using Pixel  = PixelB;

} // namespace pl2d
