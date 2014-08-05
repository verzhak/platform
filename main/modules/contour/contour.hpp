
#ifndef ALGO_CONTOUR_CONTOUR_HPP
#define ALGO_CONTOUR_CONTOUR_HPP

#include "contour/all.hpp"
#include "contour/approx.hpp"

Mat preprocess(const Mat &);
Mat bits(const Mat &);
Mat morphology(const Mat &);
Mat gabor(const Mat &);
Mat ann(const Mat &);
Mat sobel(const Mat &);

#endif

