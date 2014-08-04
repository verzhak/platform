
#include "all.hpp"
#include "main_loop.hpp"

int main(int argc, char * argv[])
{
	amv_init();

	throw_if(argc != 2);

	CMainLoop main_loop(atoi(argv[1]), 7777);

	main_loop.run();

	amv_destroy();

	return 0;
}

