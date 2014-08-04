
#ifndef PROCESS_HPP
#define PROCESS_HPP

#include "all.hpp"
#include "interface.hpp"

class CProcess
{
	int is_opencv_interface;
	QLibrary lib;

	decltype(process_init) * process_init_fun;
	decltype(process_destroy) * process_destroy_fun;
	decltype(process1) * process_fun;
	decltype(process_opencv) * process_opencv_fun;

	public:

		CProcess(const QString process_module_fname);
		~CProcess();

		Mat operator()(const Mat & frame);
};

#endif

