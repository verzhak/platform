
#include "all.hpp"
#include "pci.hpp"

int main(int argc, char * argv[])
{
	amv_init();

	throw_if(argc != 2);

	CConfig config = CConfig::load_from_file(argv[1]);
	CPci pci(config["card"]["pci"]["fname"].value);

	// TODO

	amv_destroy();

	return 0;
}

