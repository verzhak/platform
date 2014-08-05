
#include "contour/contour.hpp"

Mat morphology(const Mat & src)
{
	Mat dst, src_gray, t_src, t_dst, t_t_dst;

//	t_src = preprocess(src);
	cvtColor(src, src_gray, CV_RGB2GRAY);

	morphologyEx(src_gray, t_dst, MORPH_GRADIENT, Mat::ones(3, 3, CV_8U), Point(-1, -1), 1);
	threshold(t_dst, dst, 20, 255, THRESH_BINARY);

	dst = skeleton_(dst);

	return dst;
}

