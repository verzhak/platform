
#include <cstdint>
#include <opencv2/opencv.hpp>
#include "interface.hpp"
#include "exception.hpp"
#include "contour/contour.hpp"

using namespace std;
using namespace cv;

int process_init(int * is_opencv_interface)
{
	int ret = 0;

	try
	{
		throw_null(is_opencv_interface);

		* is_opencv_interface = 1;
	}
	catch(...)
	{
		ret = -1;
	}

	return ret;
}

int process_destroy()
{
	return 0;
}

int process_memory(const uint8_t * src, uint8_t * dst, const unsigned height, const unsigned width, const unsigned channels)
{
	int ret = 0;

	try
	{
		unsigned v, u;
		Mat __src(height, width, CV_8UC3), __dst(height, width, CV_8U);

		throw_if(channels != 3); // Костыль

		for(v = 0; v < height; v++)
			for(u = 0; u < width; u++)
			{
				Vec3b pixel;

				pixel[0] = src[v * width * channels + u * channels];
				pixel[1] = src[v * width * channels + u * channels + 1];
				pixel[2] = src[v * width * channels + u * channels + 2];

				__src.at<Vec3b>(v, u) = pixel;
			}

		throw_if(process_opencv(& __src, & __dst));

		for(v = 0; v < height; v++)
			for(u = 0; u < width; u++)
				dst[v * width + u] = __dst.at<uint8_t>(v, u);
	}
	catch(...)
	{
		ret = -1;
	}

	return ret;
}

int process_opencv(const void * src, void * dst)
{
	int ret = 0;

	try
	{
		const Mat * __src = (const Mat *) src;
		Mat * __dst = (Mat *) dst;

		* __dst = bits(* __src);

		if(__dst->channels() == 3)
			cvtColor(* __dst, * __dst, CV_RGB2GRAY);

		threshold(* __dst, * __dst, 100, 255, THRESH_BINARY);
	}
	catch(...)
	{
		ret = -1;
	}

	return ret;
}

