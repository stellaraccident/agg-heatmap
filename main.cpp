/**
 * agg-heatmap
 * Copyright 2011, Stella Laurenzo
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "imageio.hpp"
#include "density_mask.hpp"

#include "agg_rendering_buffer.h"
#include "agg_pixfmt_rgba.h"
#include "agg_gradient_lut.h"

int main(int argc, char** argv)
{
	unsigned width=640;
	unsigned height=480;
	unsigned bands=4;

	srand(time(0));


	/// Initialize density grid
	density_grid dg(width, height);
	//dm.set_random_samples(1, true);
	dg.set_random_samples(250, true);
	//dg.set_sample(100, 100, 1.0);
	//dg.set_sample(110, 110, 0.3);
	//dg.set_sample(300, 300, 0.5);

	density_mask dm(width, height);

	//dg.fill_mask(dm);
	//save_png8(dm.rbuf(), "testmask.png");

	dg.gaussian_blend(20, true);
	dg.fill_mask(dm);

	save_png8(dm.rbuf(), "blurredmask.png");

	/// Start assembling the color map
	unsigned char* buffer = new unsigned char[width * height * bands];
	memset(buffer, 0, width*height*bands);

	agg::rendering_buffer rbuf(buffer,
			width,
			height,
			width * bands);
	agg::pixfmt_rgba32 pixf(rbuf);

	// Color gradient
	agg::gradient_lut<agg::color_interpolator<agg::rgba8> > color_lut;
	color_lut.remove_all();
	color_lut.add_color(0, agg::rgba8(0,0,128,0));
	color_lut.add_color(0.45, agg::rgba8(0,0,255,0));
	color_lut.add_color(0.55, agg::rgba8(0,255,255,0));
	color_lut.add_color(0.65, agg::rgba8(0,255,0,0));
	color_lut.add_color(0.95, agg::rgba8(139,139,0,0));
	color_lut.add_color(1.0, agg::rgba8(255,0,0,0));
	color_lut.build_lut();

	agg::gradient_lut<agg::color_interpolator<agg::gray8> > alpha_lut;
	alpha_lut.remove_all();
	alpha_lut.add_color(0, agg::gray8(0));
	alpha_lut.add_color(0.1, agg::gray8(150));
	alpha_lut.add_color(1.0, agg::gray8(255));
	alpha_lut.build_lut();

	for (unsigned y=0; y<pixf.height(); y++) {
		for (unsigned x=0; x<pixf.width(); x++) {
			agg::gray8 maskval=dm.pixf().pixel(x, y);
			if (maskval.v==0) continue;

			agg::rgba8 c=color_lut[maskval.v];
			c.a=alpha_lut[maskval.v].v;
			//c.a=255;

			pixf.copy_pixel(x, y, c);
		}
	}

	save_png32(rbuf, "testout.png");
	return 0;
}
