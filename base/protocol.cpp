
#include "protocol.hpp"

CTree protocol::base(const unsigned command, const unsigned ind)
{
	CTree packet;

	packet.get("command") = command;
	packet.get("ind") = ind;

	return packet;
}

CTree protocol::image_header(unsigned ind, unsigned height, unsigned width, unsigned block_num)
{
	CTree packet;

	packet.get("height") = height;
	packet.get("width") = width;
	packet.get("block_num") = block_num;

	return packet + protocol::base(CARD_COMMAND_IMAGE_HEADER, ind);
}

CTree protocol::image_block(unsigned ind, string data, unsigned block_ind)
{
	CTree packet;

	packet.get("data") = data;
	packet.get("block_ind") = block_ind;

	return packet + protocol::base(CARD_COMMAND_IMAGE_BLOCK, ind);
}

CTree protocol::orientation(unsigned ind, double x, double y, double h, double course, double roll, double pitch, double aspect_x, double aspect_y, unsigned coord_system)
{
	CTree packet;

	packet.get("x") = x;
	packet.get("y") = y;
	packet.get("h") = h;
	packet.get("course") = course;
	packet.get("roll") = roll;
	packet.get("pitch") = pitch;
	packet.get("aspect_x") = aspect_x;
	packet.get("aspect_y") = aspect_y;
	packet.get("coord_system") = coord_system;

	return packet + protocol::base(CARD_COMMAND_ORIENTATION, ind);
}

CTree protocol::correlation_info(unsigned ind, unsigned result, unsigned result_num)
{
	CTree packet;

	packet.get("result") = result;
	packet.get("result_num") = result_num;

	return packet + protocol::base(CARD_COMMAND_CORRELATION_RESULT_INFO, ind);
}

CTree protocol::correlation_result(unsigned ind, unsigned ab, unsigned cd, unsigned fe, unsigned dx, unsigned dy, unsigned num)
{
	CTree packet;

	packet.get("ab") = ab;
	packet.get("cd") = cd;
	packet.get("fe") = fe;
	packet.get("dx") = dx;
	packet.get("dy") = dy;
	packet.get("num") = num;

	return packet + protocol::base(CARD_COMMAND_CORRELATION_RESULT, ind);
}

