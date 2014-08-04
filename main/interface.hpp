
#ifndef INTERFACE_HPP
#define INTERFACE_HPP

extern "C"
{
	int process_init(int * is_opencv_interface);
	int process_destroy();
	int process1(const uint8_t * src, uint8_t * dst, const unsigned height, const unsigned width, const unsigned channels);
	int process_opencv(const void * src_Mat, void * dst_Mat);
};

#endif

