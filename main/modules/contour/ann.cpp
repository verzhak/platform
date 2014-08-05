
#include "contour/contour.hpp"
#include "contour/base_cnn.hpp"

Mat ann(const Mat & src)
{
	unsigned v, u;
	double min_Y, max_Y, a, b;
	Mat src_gray, dst;
	c_cnn<c_simple_neuro> cnn(src.rows, src.cols);
//	c_cnn<c_my_neuro> cnn(src.rows, src.cols);
	
	// cvtColor(src, src_gray, CV_RGB2GRAY);
	cvtColor(preprocess(src), src_gray, CV_RGB2GRAY);

	Mat Y = Mat::zeros(src.rows, src.cols, CV_64F);
	Mat U = Mat_<double>(src_gray) / 255.;

	for(v = 0; v < 12; v++)
	{
		char buf[1024];
		sprintf(buf, "dst_%u", v);
		Y = cnn(Y, U);
		
		minMaxLoc(Y, & min_Y, & max_Y);
		a = 255. / (max_Y - min_Y);
		b = - a * min_Y;

		printf("TODO %u = %lf %lf\n", v, min_Y, max_Y);
	}

	dst = Mat::zeros(Y.rows, Y.cols, CV_8U);

	/*
	for(v = 0; v < Y.rows; v++)
		for(u = 0; u < Y.cols; u++)
		{
			double pixel = Y.at<double>(v, u);
			dst.at<uint8_t>(v, u) = fabs(pixel) < 0.8 ? 255 : 0;
		}
	*/

	minMaxLoc(Y, & min_Y, & max_Y);
	a = 255. / (max_Y - min_Y);
	b = - a * min_Y;
	dst = Mat_<uint8_t>(Y * a + b);

	printf("TODO %lf %lf\n", min_Y, max_Y);

	threshold(dst, dst, 150, 255, THRESH_BINARY);

//	dst = skeleton_(dst(Range(20, dst.rows - 20), Range(20, dst.cols - 20)));
//	dst = approx_(dst);

//	dst = approx_(dst(Range(20, dst.rows - 20), Range(20, dst.cols - 20)));

	return dst;
}

