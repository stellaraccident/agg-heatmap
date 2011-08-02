/**
 * agg-heatmap
 * Copyright 2011, Stella Laurenzo
 */
#ifndef __IMAGEIO_HPP__
#define __IMAGEIO_HPP__

#include "agg_rendering_buffer.h"

/// Save a buffer assumed to be organized as RGBA as
/// a png file.  This does minimal error handling
bool save_png32(agg::rendering_buffer& rbuf, const char* file_name);

/// Save a buffer assumed to be a u8 grayscale image
bool save_png8(agg::rendering_buffer& rbuf, const char* file_name);

#endif
