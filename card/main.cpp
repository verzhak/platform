
#include "all.hpp"
#include "base/config.hpp"
#include "main_loop.hpp"

int main(int argc, char * argv[])
{
	amv_init();

	throw_if(argc != 2);

	CConfig config(argv[1]);
	CMap map(config["card"]["map"].value, config["card"]["height_map"].value);
	CPci pci(config["card"]["pci"]["fname"].value, config["card"]["pci"]["size"].to_uint());
	CMainLoop main_loop(map, pci, config["card"]["port"].to_uint());

	main_loop.run();

	amv_destroy();

	return 0;
}

