/**
 * agg-heatmap
 * Copyright 2011, Stella Laurenzo
 */
#ifndef __DENSITY_MASK_HPP__
#define __DENSITY_MASK_HPP__

#include "agg_alpha_mask_u8.h"
#include "agg_pixfmt_gray.h"
#include "agg_blur.h"

/**
 * A density mask is a gray-scale image buffer intended to store density samples
 * over an area.  It is backed by a u8 data model so it can store 0..255 range
 * of fractions between 0..1.0.
 * <p>
 * Operations are defined for the sample handling side and the imaging side.
 */
class density_mask {
	unsigned m_width;
	unsigned m_height;
	agg::int8u *m_buffer;

	agg::rendering_buffer m_rbuf;
	agg::pixfmt_gray8 m_pixf;

	static agg::int8u* allocate_buffer(unsigned width, unsigned height);

	density_mask(const density_mask& other);
	density_mask& operator=(const density_mask& other);
public:
	density_mask(int width, int height):
		m_width(width),
		m_height(height),
		m_buffer(allocate_buffer(width, height)),
		m_rbuf(m_buffer, width, height, width),
		m_pixf(m_rbuf)
	{
	}

	~density_mask() {
		delete[] m_buffer;
	}

	//// Access core objects
	unsigned width() {
		return m_width;
	}
	unsigned height() {
		return m_height;
	}
	agg::int8u* buffer() {
		return m_buffer;
	}
	size_t buffer_size() {
		return m_width*m_height;
	}
	agg::rendering_buffer& rbuf() {
		return m_rbuf;
	}
	agg::pixfmt_gray8& pixf() {
		return m_pixf;
	}

};

/**
 * Contains a rectangular grid of floating point samples and provides
 * operations for smoothing and transferring them to a density_mask as
 * pixel values.
 */
class density_grid {
	float* m_buffer;
	unsigned m_width;
	unsigned m_height;

	float m_minlevel;
	float m_maxlevel;

	density_grid(const density_grid& other);
	density_grid& operator=(const density_grid& other);

public:
	density_grid(unsigned width, unsigned height):
		m_width(width),
		m_height(height),
		m_minlevel(0),
		m_maxlevel(1)
	{
		m_buffer=new float[width*height];
	}
	~density_grid() {
		delete[] m_buffer;
	}

	// Sample operations
	void set_sample(int x, int y, float value) {
		if (x<0 || y<0 || x>=(int)m_width || y>=(int)m_height) return;

		m_buffer[y*m_width+x]=value;
	}

	float get_sample(int x, int y) {
		if (x<0 || y<0 || x>=(int)m_width || y>=(int)m_height) return 0;

		return m_buffer[y*m_width+x];
	}

	void set_random_sample(bool use_random_value) {
		// Note: this is just for testing and plays a little fast and loose
		// with ranges (safe, just not accurate)
		int x=(int)(m_width * ((float)rand() / ((float)RAND_MAX)));
		int y=(int)(m_height * ((float)rand() / ((float)RAND_MAX)));
		float value=use_random_value ? (float)rand() / (float)RAND_MAX : 1.0;

		set_sample(x, y, value);
	}

	void set_random_samples(int count, bool use_random_value) {
		for (int i=0; i<count; i++) {
			set_random_sample(use_random_value);
		}
	}

	void fill_mask(density_mask& dst) {
		if (dst.width()!=m_width || dst.height()!=m_height) return;

		size_t size=m_width*m_height;
		float minlevel=m_minlevel;
		float maxlevel=m_maxlevel;
		float* src_buffer=m_buffer;
		agg::int8u* dst_buffer=dst.buffer();

		for (size_t i=0; i<size; i++) {
			float norm=(src_buffer[i] - minlevel) / maxlevel;
			int v=255 * norm;
			if (v<0) v=0;
			else if (v>255) v=255;
			dst_buffer[i]=v;
		}
	}

	/**
	 * Calculate the current maximum range of values
	 */
	float range_max();

	/**
	 * Multiply all values by the given factor
	 */
	void multiply(float factor);

	/**
	 * Perform a gaussian blend of the given radius, optionally
	 * adjusting the intensity to be of the same ratio as the original.
	 */
	void gaussian_blend(int radius, bool correct_intensity);
};

#endif
