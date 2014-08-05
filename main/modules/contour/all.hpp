
#ifndef ALL_HPP
#define ALL_HPP

#include <vector>
#include <opencv2/opencv.hpp>
#include "exception.hpp"

using namespace std;
using namespace cv;

template <class tp> tp to_color(const unsigned pixel)
{
	return tp((pixel & 0xFF0000) >> 16, (pixel & 0xFF00) >> 8, pixel & 0xFF);
};

#endif

