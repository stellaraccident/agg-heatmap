#include <png.h>

#include "imageio.hpp"

bool save_png_generic(agg::rendering_buffer& rbuf, const char* file_name, int color_type)
{
	FILE* fd=fopen(file_name, "wb");
	if (!fd) {
		perror("Could not open output file");
		return false;
	}

	png_structp png_ptr=png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (!png_ptr) {
		fprintf(stderr, "Could not create png writer\n");
		fclose(fd);
		return false;
	}

	png_infop info_ptr=png_create_info_struct(png_ptr);
	if (!info_ptr) {
		fprintf(stderr, "Could not initialize png info\n");
		png_destroy_write_struct(&png_ptr, 0);
		fclose(fd);
		return false;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(fd);
		return false;
	}

	png_init_io(png_ptr, fd);
	png_set_IHDR(png_ptr, info_ptr,
			rbuf.width(),
			rbuf.height(),
			8,
			color_type,
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT);
	png_write_info(png_ptr, info_ptr);

	for (unsigned y=0; y<rbuf.height(); y++) {
		png_write_row(png_ptr, (png_byte*)rbuf.row(y).ptr);
	}

	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(fd);
	return true;
}

bool save_png32(agg::rendering_buffer& rbuf, const char* file_name)
{
	return save_png_generic(rbuf, file_name, PNG_COLOR_TYPE_RGB_ALPHA);
}

bool save_png8(agg::rendering_buffer& rbuf, const char* file_name)
{
	return save_png_generic(rbuf, file_name, PNG_COLOR_TYPE_GRAY);
}
