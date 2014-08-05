
#include "pci.hpp"

CPci::CPci(const string fname, const unsigned size) :
	__size(size)
{
	fl = -1;
	ptr = NULL;

	throw_if((fl = open(fname.c_str(), O_RDWR | O_SYNC)) < 0);
	throw_null(ptr = (uint32_t *) mmap(NULL, __size, PROT_READ | PROT_WRITE, MAP_SHARED, fl, 0));
}

CPci::~CPci()
{
	if(ptr != NULL)
		munmap(ptr, __size);

	if(fl >= 0)
		close(fl);
}

void CPci::reset()
{
	return; // TODO
	ptr[REG_RESET] = DO_RESET;

	while(ptr[REG_RESET] != DONE_RESET)
		sched_yield();
}

vector<s_result> CPci::results()
{
	vector<s_result> todo;
	
	{
		s_result res = { .ab = 1, .cd = 2, .fe = 3, .dx = 10, .dy = 11, .num = 152 };
		todo.push_back(res);
	}
	{
		s_result res = { .ab = 4, .cd = 5, .fe = 6, .dx = 17, .dy = 27, .num = 453 };
		todo.push_back(res);
	}
	{
		s_result res = { .ab = 7, .cd = 7, .fe = 9, .dx = 37, .dy = 34, .num = 837 };
		todo.push_back(res);
	}

	return todo;
	; // TODO
}

