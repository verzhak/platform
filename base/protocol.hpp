
#ifndef CARD_PROTOCOL_HPP
#define CARD_PROTOCOL_HPP

#include "base/all.hpp"
#include "base/tree.hpp"

#define CARD_COMMAND_IMAGE_HEADER				1
#define CARD_COMMAND_IMAGE_BLOCK				2
#define CARD_COMMAND_ORIENTATION				3
#define CARD_COMMAND_CORRELATION				4
#define CARD_COMMAND_EXIT						5
#define CARD_COMMAND_CORRELATION_RESULT			6
#define CARD_COMMAND_ARINC_DONE					7
#define CARD_COMMAND_CORRELATION_RESULT_INFO	8
#define CARD_COMMAND_NOTHING					9

#define CARD_CORRELATION_RESULT_FAIL			0
#define CARD_CORRELATION_RESULT_SUCCESS			1

namespace protocol
{
	string encode_base64(const uint8_t * src, const unsigned size);
	void decode_base64(const string src, shared_ptr<uint8_t> & dst, unsigned & size);
	
	CTree base(const unsigned command, const unsigned ind);
	CTree image_header(unsigned ind, unsigned height, unsigned width, unsigned block_num);
	CTree image_block(unsigned ind, string data, unsigned block_ind);
	CTree orientation(unsigned ind, double x, double y, double h, double course, double roll, double pitch, double aspect_x, double aspect_y, unsigned coord_system);
	CTree correlation_info(unsigned ind, unsigned result, unsigned result_num);
	CTree correlation_result(unsigned ind, unsigned ab, unsigned cd, unsigned fe, unsigned dx, unsigned dy, unsigned num);
};

#endif

