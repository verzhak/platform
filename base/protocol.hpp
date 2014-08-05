
#ifndef CARD_PROTOCOL_HPP
#define CARD_PROTOCOL_HPP

#include "all.hpp"

#define CARD_MARK 0xBAAB

#define CARD_COMMAND_IMAGE					1
#define CARD_COMMAND_ORIENTATION			2
#define CARD_COMMAND_CORRELATION			3
#define CARD_COMMAND_EXIT					4
#define CARD_COMMAND_CORRELATION_RESULT		5
#define CARD_COMMAND_ARINC_DONE				6

#define CARD_CORRELATION_RESULT_UNKNOWN		0
#define CARD_CORRELATION_RESULT_FAIL		1

struct __attribute__((packed)) SCardHeaderPacket
{
	uint32_t mark;
	uint32_t command;
	uint32_t ind;
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

struct __attribute__((packed)) SCardCorrelationResultInfoPacket
{
	uint32_t mark;
	uint32_t result;
	uint32_t result_num;
};

struct __attribute__((packed)) SCardCorrelationResultPacket
{
	uint32_t mark;
	uint32_t ab;
	uint32_t cd;
	uint32_t fe;
	uint32_t dx;
	uint32_t dy;
	uint32_t num;
};

#endif

