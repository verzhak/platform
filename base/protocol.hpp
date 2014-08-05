
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

class CPacket
{
	protected:

		t_fun recv_fun, send_fun;

		struct __attribute__((packed))
		{
			uint32_t mark;
			uint32_t command;
			uint32_t ind;
		} header;

		void __send(const unsigned command, const unsigned ind, const void * packet, const unsigned size);

	public:

		typedef function<void(void *, const unsigned)> t_fun;

		CPacket(t_fun recv_fun, t_fun send_fun);

		unsigned recv_header();
		unsigned recv_header(unsigned & ind);
		virtual void recv() = 0;
		virtual void send(const unsigned ind) = 0;
};

class CImagePacket : public CPacket
{
	public:

		struct __attribute__((packed))
		{
			uint32_t mark;
			uint32_t height;
			uint32_t width;
			uint32_t block_size;
		} data;

		void recv();
		void send(const unsigned ind);
};

class COrientationPacket : public CPacket
{
	public:

		struct __attribute__((packed))
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
		} data;

		void recv();
		void send(const unsigned ind);
};

class CCorrelationResultInfoPacket : public CPacket
{
	public:

		struct __attribute__((packed))
		{
			uint32_t mark;
			uint32_t result;
			uint32_t result_num;
		} data;

		void recv();
		void send(const unsigned ind);
};

class CCorrelationResultPacket : public CPacket
{
	public:

		struct __attribute__((packed))
		{
			uint32_t mark;
			uint32_t ab;
			uint32_t cd;
			uint32_t fe;
			uint32_t dx;
			uint32_t dy;
			uint32_t num;
		} data;

		void recv();
		void send(const unsigned ind);
};

#endif

