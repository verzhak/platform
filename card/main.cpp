
#include "all.hpp"
#include "base/tree.hpp"
#include "main_loop.hpp"

int main(int argc, char * argv[])
{
	amv_init();

	throw_if(argc != 2);

	CConfig config = CConfig::load_from_file(argv[1]);
	CMap map(config["card"]["map"].value, config["card"]["height_map"].value);
	CPci pci(config["card"]["pci"]["fname"].value, config["card"]["pci"]["size"].uint());
	CMainLoop main_loop(map, pci, config["card"]["port"].uint());

	main_loop.run();

	amv_destroy();

	return 0;
}

