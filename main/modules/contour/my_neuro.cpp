
#include "contour/base_cnn.hpp"

double c_my_neuro::A(const Mat & Y, const int ci, const int cj)
{
	double coef = 0;

	if(! ci && ! cj)
		coef = 2;

	return coef * Y.at<double>(i + ci, j + cj);
}

double c_my_neuro::B(const Mat & U, const int ci, const int cj)
{
	double coef = (fabs(ci) + fabs(cj)) / (2. * size());

//	if(! ci || ! cj)
//		coef = 0;

	return coef * (U.at<double>(i, j) - U.at<double>(i + ci, j + cj));
}

double c_my_neuro::I()
{
	return -1;
}

unsigned c_my_neuro::size()
{
	return 3; // + t % 6;

	switch(t % 3)
	{
		case 0: return 5;
		case 1: return 6;
		case 2: return 7;
	}
}

double c_my_neuro::f()
{
	return tanh(x); // 1 / (1 + exp(-x));
}

