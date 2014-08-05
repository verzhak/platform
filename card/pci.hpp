
#ifndef PCI_HPP
#define PCI_HPP

#include "all.hpp"

struct s_result
{
	unsigned ab;
	unsigned cd;
	unsigned fe;
	unsigned dx;
	unsigned dy;
	unsigned num;
};

#define REG_RESET	0
#define DO_RESET	1
#define DONE_RESET	2

#define REG_READY_READ	1
#define READY_READ		1

class CPci
{
	const unsigned __size;
	int fl;
	uint32_t * ptr;

	public:

		CPci(const string fname, const unsigned size);
		~CPci();

		void reset();
		vector<s_result> results();
};

#endif

