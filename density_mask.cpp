#include <stdio.h>
#include <stdlib.h>
#include "density_mask.hpp"
#include "filters.hpp"

agg::int8u* density_mask::allocate_buffer(unsigned width, unsigned height) {
	size_t size=width*height;
	agg::int8u* ret=new agg::int8u[size];
	memset(ret, width*height, size);
	return ret;
}


class kernel_1d {
public:
	float* matrix;
	unsigned length;

	kernel_1d(unsigned alength): length(alength) {
		matrix=new float[alength];
	}
	~kernel_1d() {
		delete[] matrix;
	}

	void initialize_gauss() {
		int middle=length/2;
		double sigma=middle/3.0;
		double a=1 / sqrt(2*M_PI*sigma*sigma);
		double den=2*sigma*sigma;
		for (unsigned i=0; i<length; i++) {
			double x=(double)i - middle;
			double v=a * exp(-(x*x) / den);
			matrix[i]=v;
		}
	}

	void print() {
		for (unsigned i=0; i<length; i++) {
			printf("\t%f\n", (double)matrix[i]);
		}
	}

};

template<class T>
void swap(T& a, T& b) {
	T tmp=a;
	a=b;
	b=tmp;
}

void apply_horizontal(kernel_1d& kernel, float* src, float* dst, unsigned width, unsigned height) {
	int w=(int)width;
	int h=(int)height;
	int radius=kernel.length/2;

	for (int y=radius; y<h-radius; y++) {
		float* src_row=src + w*y;
		float* dst_row=dst + w*y;

		for (int x=radius; x<w-radius; x++) {
			float accum=0;
			for (unsigned i=0; i<kernel.length; i++) {
				accum+=src_row[x+i-radius] * kernel.matrix[i];
			}
			dst_row[x]=accum;
		}
	}
}

void apply_vertical(kernel_1d& kernel, float* src, float* dst, unsigned width, unsigned height) {
	int w=(int)width;
	int h=(int)height;
	int radius=kernel.length/2;

	for (int x=radius; x<w-radius; x++) {
		float* src_col=src+x;
		float* dst_col=dst+x;

		for (int y=radius; y<h-radius; y++) {
			float accum=0;
			for (unsigned i=0; i<kernel.length; i++) {
				accum+=src_col[width*(y+i-radius)] * kernel.matrix[i];
			}
			dst_col[y*width]=accum;
		}
	}
}

float density_grid::range_max() {
	size_t size=m_width*m_height;
	float* buffer=m_buffer;
	float mx=-INFINITY;

	for (size_t i=0; i<size; i++) {
		float v=buffer[i];
		mx=fmaxf(mx, v);
	}
	return mx;
}

void density_grid::multiply(float factor) {
	size_t size=m_width*m_height;
	float* buffer=m_buffer;
	for (size_t i=0; i<size; i++) {
		buffer[i]*=factor;
	}
}

void density_grid::gaussian_blend(int radius, bool correct_intensity) {
	float orig_mx;
	if (correct_intensity) {
		orig_mx=range_max();
	} else {
		orig_mx=0;
	}

	int length=radius * 2 + 1;

	kernel_1d kernel(length);
	kernel.initialize_gauss();

	printf("Gaussian kernel:\n");
	kernel.print();

	size_t size=m_width*m_height;
	float* dst=new float[size];

	memset(dst, 0, size*sizeof(float));
	apply_horizontal(kernel, m_buffer, dst, m_width, m_height);

	memset(m_buffer, 0, size*sizeof(float));
	apply_vertical(kernel, dst, m_buffer, m_width, m_height);

	delete[] dst;

	if (correct_intensity) {
		float res_mx=range_max();

		// Gaussian blur reduces intensity.  Adjust by scaling the result
		float factor=orig_mx / res_mx;
		printf("Intensity factor=%f\n", factor);
		multiply(factor);
	}
}

