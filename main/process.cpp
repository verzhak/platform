
#include "process.hpp"

CProcess::CProcess(const QString process_module_fname) :
	lib(process_module_fname)
{
	throw_if(! lib.load());

	throw_null(process_init_fun = (decltype(process_init_fun)) lib.resolve("process_init"));
	throw_null(process_destroy_fun = (decltype(process_destroy_fun)) lib.resolve("process_destroy"));
	throw_null(process_memory_fun = (decltype(process_memory_fun)) lib.resolve("process_memory"));
	throw_null(process_opencv_fun = (decltype(process_opencv_fun)) lib.resolve("process_opencv"));

	throw_if((* process_init_fun)(& is_opencv_interface));
}

CProcess::~CProcess()
{
	throw_if((* process_destroy_fun)());
}

Mat CProcess::operator()(const Mat & frame)
{
	Mat src = frame.clone(), dst;

	if(is_opencv_interface)
	{
		throw_if((* process_opencv_fun)((void *) & src, (void *) & dst));
		throw_if(dst.channels() != 1);
	}
	else
	{
		dst = Mat::zeros(src.size(), CV_8U);

		throw_if((* process_memory_fun)(src.data, dst.data, src.rows, src.cols, src.channels()));
	}

	return dst;
}

