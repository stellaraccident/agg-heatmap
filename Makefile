CXXFLAGS = -I/usr/include/agg2 -Wall -O3

all: agg-heatmap

SOURCES := main.cpp density_mask.cpp imageio.cpp
HEADERS := density_mask.hpp imageio.hpp filters.hpp

agg-heatmap: $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o agg-heatmap $(SOURCES) -lpng
	
