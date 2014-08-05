
#include <cstdint>
#include <opencv2/opencv.hpp>
#include "interface.hpp"
#include "exception.hpp"

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
		// TODO
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
		// TODO
	}
	catch(...)
	{
		ret = -1;
	}

	return ret;
}

