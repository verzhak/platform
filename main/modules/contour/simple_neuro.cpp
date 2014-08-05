
#include "contour/base_cnn.hpp"

double c_simple_neuro::A(const Mat & Y, const int ci, const int cj)
{
	double coef = 0;

	if(! ci && ! cj)
		coef = 2;

	return coef * Y.at<double>(i + ci, j + cj);
}

double c_simple_neuro::B(const Mat & U, const int ci, const int cj)
{
	double coef = -1;

	if(! ci && ! cj)
		coef = size() * size() - 1;

	return coef * U.at<double>(i + ci, j + cj);
}

double c_simple_neuro::I()
{
	return -2; // size() / 11; // 0.3;
}

unsigned c_simple_neuro::size()
{
	return 11;
}

double c_simple_neuro::f()
{
	return (fabs(x + 1) - fabs(x - 1)) / 2;
}

