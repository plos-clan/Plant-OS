#pragma once
#include <pl2d.hpp>

namespace pl2d::framebuffer {

template <PixFmt fmt>
finline void fb_flush_pix(FrameBuffer &fb, const pl2d::TextureB &tex, u32 x, u32 y);
template <PixFmt fmt>
finline void fb_flush_pix(FrameBuffer &fb, const pl2d::TextureF &tex, u32 x, u32 y);
template <PixFmt fmt>
finline void fb_copy_to_pix(const FrameBuffer &fb, pl2d::TextureB &tex, u32 x, u32 y);
template <PixFmt fmt>
finline void fb_copy_to_pix(const FrameBuffer &fb, pl2d::TextureF &tex, u32 x, u32 y);

template <PixFmt fmt>
finline void fb_flush(FrameBuffer &fb, const pl2d::TextureB &tex, const pl2d::Rect &rect);
template <PixFmt fmt>
finline void fb_flush(FrameBuffer &fb, const pl2d::TextureF &tex, const pl2d::Rect &rect);
template <PixFmt fmt>
finline void fb_copy_to(const FrameBuffer &fb, pl2d::TextureB &tex, const pl2d::Rect &rect);
template <PixFmt fmt>
finline void fb_copy_to(const FrameBuffer &fb, pl2d::TextureF &tex, const pl2d::Rect &rect);

} // namespace pl2d::framebuffer
