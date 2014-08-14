
#include "pci.hpp"

CPci::CPci(const string fname)
{
	fl = -1;
	ptr = NULL;

	throw_if((fl = open(fname.c_str(), O_RDWR | O_SYNC | O_NONBLOCK)) < 0);
	throw_null(ptr = (uint32_t *) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_NONBLOCK | MAP_POPULATE, fl, 0));
}

CPci::~CPci()
{
	if(ptr != NULL)
		munmap(ptr, size);

	if(fl >= 0)
		close(fl);
}

void CPci::wait(const uint32_t state)
{
	return;

	while(ptr[REG_STATE] != state)
		sched_yield();
}

void CPci::wait(const uint32_t state, const uint32_t from_state)
{
	ptr[REG_STATE] = from_state;
	wait(state);
}

void CPci::write(function<void(uint32_t *)> update_regs, const uint8_t * contour_buf, const unsigned contour_buf_size, const uint8_t * matrix_buf, const unsigned matrix_buf_size)
{
	shared_ptr<uint8_t> buf;
	const unsigned buf_size = contour_size + matrix_size;
	uint8_t * __ptr_8, * ptr_8 = (uint8_t *) ptr;
	unsigned c_offset, m_offset;

	wait(STATE_WRITE);
	update_regs(ptr);

	// ############################################################################ 

	buf.reset(new uint8_t[buf_size], std::default_delete<uint8_t[]>());
	throw_null(__ptr_8 = buf.get());

	for
	(
		c_offset = 0, m_offset = 0;
		c_offset < contour_buf_size && m_offset < matrix_buf_size;
		c_offset += contour_size, m_offset += matrix_size
	)
	{
		const unsigned c_real_block_size = min(contour_buf_size - c_offset, contour_size);
		const unsigned m_real_block_size = min(matrix_buf_size - m_offset, matrix_size);

		wait(STATE_WRITE, STATE_WAIT);
		wait(STATE_WRITE, STATE_READ);

		if(c_real_block_size < contour_size)
		{
			memcpy(ptr_8 + reg_size, contour_buf + c_offset, c_real_block_size);
			memcpy(ptr_8 + reg_size + contour_size, matrix_buf + m_offset, m_real_block_size);
		}
		else
		{
			memcpy(__ptr_8, contour_buf + c_offset, c_real_block_size);
			memcpy(__ptr_8 + contour_size, matrix_buf + m_offset, m_real_block_size);
		
			memcpy(ptr_8 + reg_size, __ptr_8, c_real_block_size + m_real_block_size);
		}
	}

	for(; c_offset < contour_buf_size; c_offset += contour_size)
	{
		const unsigned c_real_block_size = min(contour_buf_size - c_offset, contour_size);

		wait(STATE_WRITE, STATE_WAIT);
		memcpy(ptr_8 + reg_size, contour_buf + c_offset, c_real_block_size);
	}

	for(; m_offset < matrix_buf_size; m_offset += matrix_size)
	{
		const unsigned m_real_block_size = min(matrix_buf_size - m_offset, matrix_size);

		wait(STATE_WRITE, STATE_WAIT);
		memcpy(ptr_8 + reg_size + contour_size, matrix_buf + m_offset, m_real_block_size);
	}

	ptr[REG_STATE] = STATE_WRITE_END;
}

vector<s_result> CPci::read()
{
	vector<s_result> res;

	wait(STATE_READ);

	// ############################################################################ 
	// TODO

	{
		s_result __res = { .ab = 1, .cd = 2, .fe = 3, .dx = 10, .dy = 11, .num = 152 };
		res.push_back(__res);
	}
	{
		s_result __res = { .ab = 4, .cd = 5, .fe = 6, .dx = 17, .dy = 27, .num = 453 };
		res.push_back(__res);
	}
	{
		s_result __res = { .ab = 7, .cd = 7, .fe = 9, .dx = 37, .dy = 34, .num = 837 };
		res.push_back(__res);
	}

	// ############################################################################ 

	ptr[REG_STATE] = STATE_WAIT;

	return res;
}

