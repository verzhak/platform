
#include <cstdio>
#include <cstdint>
#include <memory>
#include <opencv2/opencv.hpp>
#include "interface.hpp"
#include "exception.hpp"

using namespace std;
using namespace cv;

int main(const int argc, const char * argv[])
{
	int ret = 0;

	try
	{
		bool is_first = true;
		int is_opencv_interface;
		unsigned v, u, height, width;
		shared_ptr<uint8_t> src_buf, dst_buf;
		uint8_t * p_src_buf, * p_dst_buf;
		Mat src, dst_memory, dst_opencv;

		if(argc != 2)
		{
			fprintf(stderr, "[ ERROR ] ./test\tVIDEO\n");

			throw_;
		}
	
		throw_if(process_init(& is_opencv_interface));
		throw_if(! is_opencv_interface);

		/*
		VideoCapture video(argv[1]);
		throw_if(! video.isOpened());

		while(video.read(src))
		{
		*/
		
		src = imread(argv[1], -1);
		
			if(is_first)
			{
				is_first = false;
				height = src.rows;
				width = src.cols;

				dst_memory.create(src.size(), CV_8U);
				src_buf.reset(new uint8_t[height * width * 3], std::default_delete<uint8_t[]>());
				dst_buf.reset(new uint8_t[height * width], std::default_delete<uint8_t[]>());

				throw_null(p_src_buf = src_buf.get());
				throw_null(p_dst_buf = dst_buf.get());
			}

			for(v = 0; v < height; v++)
				for(u = 0; u < width; u++)
				{
					const Vec3b pixel = src.at<Vec3b>(v, u);

					p_src_buf[(v * width + u) * 3] = pixel[0];
					p_src_buf[(v * width + u) * 3 + 1] = pixel[1];
					p_src_buf[(v * width + u) * 3 + 2] = pixel[2];
				}

			throw_if(process_memory(p_src_buf, p_dst_buf, height, width, 3)); // Можно передавать через .data, но никто не гарантирует отсутствие выравнивания
			throw_if(process_opencv((void *) & src, (void *) & dst_opencv));

			for(v = 0; v < height; v++)
				for(u = 0; u < width; u++)
					dst_memory.at<uint8_t>(v, u) = p_dst_buf[v * width + u];

			imshow("source", src);
			imshow("destination (memory)", dst_memory);
			imshow("destination (opencv)", dst_opencv);

			waitKey(-1);
		
		// }
	}
	catch(...)
	{
		ret = -1;
	}

	process_destroy();

	return ret;
}

