
#include "contour/contour.hpp"

Mat bits(const Mat & src)
{
	const unsigned height = src.rows, width = src.cols;
	unsigned v, u, t;
	int tv, tu, rv, ru;
	Mat wnd, dst(height, width, CV_8UC3);
	Vec3b pixel;

	for(v = 0; v < height; v++)
		for(u = 0; u < width; u++)
		{
			wnd = Mat::zeros(3, 3, CV_8UC3);

			for(tv = -1; tv <= 1; tv++)
				for(tu = -1; tu <= 1; tu++)
				{
					rv = v + tv;
					ru = u + tu;

					if(rv >= 0 && rv < height && ru >= 0 && ru < width)
						for(t = 0; t < 3; t++)
							wnd.at<Vec3b>(tv + 1, tu + 1)[t] = src.at<Vec3b>(rv, ru)[t] & 0xFF;
				}

			for(t = 0; t < 3; t++)
				pixel[t] = wnd.at<Vec3b>(1, 1)[t] & ((~ wnd.at<Vec3b>(0, 0)[t]) | (~ wnd.at<Vec3b>(0, 1)[t]) | (~ wnd.at<Vec3b>(0, 2)[t]) | (~ wnd.at<Vec3b>(1, 0)[t]) | (~ wnd.at<Vec3b>(1, 2)[t]) | (~ wnd.at<Vec3b>(2, 0)[t]) | (~ wnd.at<Vec3b>(2, 1)[t]) | (~ wnd.at<Vec3b>(2, 2)[t]));

			dst.at<Vec3b>(v, u) = pixel;
		}

	return dst;
}

