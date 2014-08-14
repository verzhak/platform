
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

#define STATE_WAIT			0
#define STATE_WRITE			1
#define STATE_READ			2
#define STATE_WRITE_END		3

#define REG_STATE				0
#define REG_NUMBER_OF_CONTOUR	1
#define REG_NUMBER_OF_MATRICES	2
#define REG_NUMBER_OF_RESULTS	1

#define CONTOUR_END 0xFFFFFFFF

class CPci
{
	int fl;
	uint32_t * ptr;
	const unsigned contour_size = 1024, matrix_size = 960, reg_size = 64;
	const unsigned size = reg_size + contour_size + matrix_size;

	public:

		CPci(const string fname);
		~CPci();

		void wait(const uint32_t state);
		void wait(const uint32_t state, const uint32_t from_state);
		void write(function<void(uint32_t *)> update_regs, const uint8_t * contour_buf, const unsigned contour_buf_size, const uint8_t * matrix_buf, const unsigned matrix_buf_size);
		vector<s_result> read();
};

#endif

