
#include "pci.hpp"

unsigned & s_result::operator[](const unsigned ind)
{
	switch(ind)
	{
		case 0: return ab;
		case 1: return cd;
		case 2: return fe;
		case 3: return dx;
		case 4: return dy;
		case 5: return num;
		default: throw_;
	}
}

// ############################################################################ 

CPci::CPci(const string fname)
{
	fl = -1;
	ptr = NULL;

	throw_if((fl = open(fname.c_str(), O_RDWR | O_SYNC )) < 0);
	throw_null(ptr = (uint32_t *) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED , fl, 0));
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
	while(ptr[REG_STATE] != state)
		sched_yield();
}

uint32_t CPci::wait(const uint32_t state_1, const uint32_t state_2)
{
	while(ptr[REG_STATE] != state_1 || ptr[REG_STATE] != state_2)
		sched_yield();

	return ptr[REG_STATE];
}

void CPci::set_and_wait(const uint32_t state, const uint32_t from_state)
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

		set_and_wait(STATE_WRITE, STATE_WAIT);

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

		set_and_wait(STATE_WRITE, STATE_WAIT);
		memcpy(ptr_8 + reg_size, contour_buf + c_offset, c_real_block_size);
	}

	for(; m_offset < matrix_buf_size; m_offset += matrix_size)
	{
		const unsigned m_real_block_size = min(matrix_buf_size - m_offset, matrix_size);

		set_and_wait(STATE_WRITE, STATE_WAIT);
		memcpy(ptr_8 + reg_size + contour_size, matrix_buf + m_offset, m_real_block_size);
	}
	
	ptr[REG_STATE] = STATE_WRITE_END;
}

vector<s_result> CPci::read()
{
	bool is_first = true;
	unsigned number_of_results, v, ind = 0;
	const unsigned elem_num_in_block = (contour_size + matrix_size) / 4;
	const unsigned reg_num = reg_size / 4;
	vector<s_result> results;

	while(wait(STATE_READ, STATE_READ_END) == STATE_READ)
	{
		if(is_first)
		{
			is_first = false;
			number_of_results = ptr[REG_NUMBER_OF_RESULTS];
			results.resize(number_of_results);
		}

		for(v = 0; v < elem_num_in_block && (ind / 6) < number_of_results; v++, ind++)
			results[ind / 6][ind % 6] = ptr[reg_num + v];
	}

	return results;
}

