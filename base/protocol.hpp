
#ifndef CARD_PROTOCOL_HPP
#define CARD_PROTOCOL_HPP

#include "all.hpp"

#define CARD_MARK 0xBAAB

#define CARD_COMMAND_IMAGE			1
#define CARD_COMMAND_ORIENTATION	2
#define CARD_COMMAND_CORRELATION	3
#define CARD_EXIT					4

struct __attribute__((packed)) SCardHeaderPacket
{
	uint32_t mark;
	uint32_t command;
};

struct __attribute__((packed)) SCardImagePacket
{
	uint32_t mark;
	uint32_t height;
	uint32_t width;
	uint32_t block_size;
};

struct __attribute__((packed)) SCardOrientationPacket
{
	uint32_t mark;
	double x;
	double y;
	double h;
	double course;
	double roll;
	double pitch;
	double aspect_x;
	double aspect_y;
	uint32_t coord_system;
};

#endif

