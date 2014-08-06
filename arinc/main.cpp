
#include "all.hpp"
#include "main_loop.hpp"

int main(int argc, char * argv[])
{
	amv_init();

	throw_if(argc != 2);

	int ret;
	CConfig config(argv[1]);
	CMainLoop main_loop(argc, argv, config["arinc"]["port"].uint());

	ret = main_loop.exec();

	amv_destroy();

	return ret;
}

