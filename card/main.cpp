
#include "all.hpp"
#include "base/tree.hpp"
#include "main_loop.hpp"

int main(int argc, char * argv[])
{
	amv_init();

	throw_if(argc != 2);

	vector<unsigned> codes =
	{
	//	0x1DADA80, // ОЗЕРА (ПОСТОЯННЫЕ)  = НАБОР
	//	0x1DB0578, // ВОДОХРАНИЛИЩА ДЕЙСТВУЮЩИЕ = НАБОР
		0x1DF4750, // РЕКИ ПОСТОЯНЫЕ (от 20-120 м) = ЛИНИЯ
	//	0x1DF9958, // КАНАЛЫ = НАБОР (ЛИНИЯ)
	//	0x1E0CDF0, // УРЕЗЫ ВОДЫ = ТОЧКА
	//	0x44F5CF0, // БОЛОТА НЕПРОХОДИМЫЕ = ШТРИХОВАННАЯ ПЛОЩАДЬ

		0x3A7AB40, // ДОРОГИ ГРУНТОВЫЕ ПРОСЕЛОЧНЫЕ = ЛИНИЯ
	//	0x3A7D250, // ДОРОГИ ПОЛЕВЫЕ,ЛЕСНЫЕ = ПУНКТИРНАЯ ЛИНИЯ
	//	0x3A78430, // УЛУЧШЕННЫЕ ГРУНТ.ДОРОГИ ДЕЙСТВ = НАБОР
	//	0x3B12120, // ДОРОГИ ТРУДНОПРОЕЗЖИЕ(улуч.гр) = НАБОР
	//	0x3B6D9F8, // МОСТЫ ПРОЧИЕ = 149 (TODO)

	//	0x3A624A0, // ШОССЕ УСОВЕРШЕНСТВ.(ДЕЙСТВ.) = НАБОР
		0x3A64BB0, // ШОССЕ ДЕЙСТВУЮЩИЕ  = НАБОР
	//	0x3B6CE40, // МОСТЫ НА РАЗОБЩЕНН.ОСНОВАНИЯХ = 149 (TODO)
	};

	CConfig config = CConfig::load_from_file(argv[1]);
	CMyMap map(config["card"]["map"].value, config["card"]["height_map"].value, codes);
	CPci pci(config["card"]["pci"]["fname"].value);
	CMainLoop main_loop(map, pci, config["card"]["port"].uint());

	main_loop.run();

	amv_destroy();

	return 0;
}

