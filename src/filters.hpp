/**
 * agg-heatmap
 * Copyright 2011, Stella Laurenzo
 */
#ifndef __FILTERS_HPP__
#define __FILTERS_HPP__

/// Originally copied from http://old.nabble.com/convolution-matrix-filters-td4956875.html
template<int k00, int k01, int k02, int k10, int k11, int k12, int k20, int k21,
		int k22, int weight, int add> struct convolution_3x3_filter_gray8 {
	template<class Img>
	void apply(const Img& dst, const Img& src) {
		const agg::int8u* psrc;
		agg::int8u* pdst;

		if (src.width() != dst.width() || src.height() != dst.height())
			return;

		unsigned w = src.width();
		unsigned h = src.height();
		unsigned dy = w * Img::pix_step;
		unsigned dx = Img::pix_step;
		unsigned x, y;

		for (y = 0; y < h; y++) {
			psrc = src.row_ptr(y);
			pdst = (agg::int8u*) dst.row_ptr(y);
			if (y == 0 || y == h - 1) {
				for (x = 0; x < w; x++) {
					//*pdst = *psrc;
					*pdst=0;
					pdst += Img::pix_step;
					psrc += Img::pix_step;
				}
			} else {
				for (x = 0; x < w; x++) {
					if (x == 0 || x == w - 1)
						//*pdst = *psrc;
						*pdst=0;
					else
						kernel(pdst, psrc, dx, dy);
					pdst += Img::pix_step;
					psrc += Img::pix_step;
				}
			}
		}
	}

	static AGG_INLINE void kernel(agg::int8u* pdst, const agg::int8u* psrc, unsigned dx, unsigned dy)
	{
		int res = (*(psrc - dy - dx) * k00 +
				*(psrc - dy ) * k01 +
				*(psrc - dy + dx) * k02 +
				*(psrc - dx) * k10 +
				*(psrc ) * k11 +
				*(psrc + dx) * k12 +
				*(psrc + dy - dx) * k20 +
				*(psrc + dy ) * k21 +
				*(psrc + dy + dx) * k22) / weight + add;
		if (res < 0) res = 0;
		if (res > 255) res = 255;
		*pdst = res;
	}
};

#endif
