
#include "all.hpp"
#include "base/config.hpp"
#include "main_loop.hpp"

int main(int argc, char * argv[])
{
	amv_init();

	throw_if(argc != 2);

	CConfig config(argv[1]);
	CMainLoop main_loop(config["card"]["port"].toUInt(), config["card"]["combine_port"].toUInt());

	main_loop.run();

	amv_destroy();

	return 0;
}

