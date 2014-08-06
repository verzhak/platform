
#ifndef CARD_PROTOCOL_HPP
#define CARD_PROTOCOL_HPP

#include "all.hpp"

#define CARD_MARK 0xBAAB

#define CARD_COMMAND_IMAGE						1
#define CARD_COMMAND_ORIENTATION				2
#define CARD_COMMAND_CORRELATION				3
#define CARD_COMMAND_EXIT						4
#define CARD_COMMAND_CORRELATION_RESULT			5
#define CARD_COMMAND_ARINC_DONE					6
#define CARD_COMMAND_CORRELATION_RESULT_INFO	7
#define CARD_COMMAND_NOTHING					8

#define CARD_CORRELATION_RESULT_UNKNOWN			0
#define CARD_CORRELATION_RESULT_FAIL			1

typedef function<void(void *, const unsigned)> t_recv_fun;
typedef function<void(const void *, const unsigned)> t_send_fun;

class CPacket
{
	t_recv_fun __recv_fun;
	t_send_fun __send_fun;

	struct __attribute__((packed))
	{
		uint32_t mark;
		uint32_t command;
		uint32_t ind;
	} header;

	void header_pack();
	void header_unpack();

	protected:

		virtual void packet_pack() { header_pack(); };
		virtual void packet_unpack() { header_unpack(); };
		virtual inline unsigned get_command() { return CARD_COMMAND_NOTHING; };
		virtual inline void * get_packet() { return (void *) & header; };
		virtual inline unsigned get_packet_size() { return sizeof(header); };

	public:

		CPacket(t_recv_fun recv_fun, t_send_fun send_fun);

		void send(const unsigned command, const unsigned ind);
		inline void send(const unsigned ind) { send(get_command(), ind); };
		unsigned recv(CPacket ** packet, unsigned & ind);
};

class CImagePacket : public CPacket
{
	protected:

		void packet_pack();
		void packet_unpack();

		inline unsigned get_command() { return CARD_COMMAND_IMAGE; };
		inline void * get_packet() { return (void *) & data; };
		inline unsigned get_packet_size() { return sizeof(data); };

	public:

		CImagePacket(t_recv_fun recv_fun, t_send_fun send_fun) : CPacket(recv_fun, send_fun) {};

		struct __attribute__((packed))
		{
			uint32_t mark;
			uint32_t height;
			uint32_t width;
			uint32_t block_size;
		} data;
};

class COrientationPacket : public CPacket
{
	protected:

		void packet_pack();
		void packet_unpack();

		inline unsigned get_command() { return CARD_COMMAND_ORIENTATION; };
		inline void * get_packet() { return (void *) & data; };
		inline unsigned get_packet_size() { return sizeof(data); };

	public:

		COrientationPacket(t_recv_fun recv_fun, t_send_fun send_fun) : CPacket(recv_fun, send_fun) {};

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
};

class CCorrelationResultInfoPacket : public CPacket
{
	protected:

		void packet_pack();
		void packet_unpack();

		inline unsigned get_command() { return CARD_COMMAND_CORRELATION_RESULT_INFO; };
		inline void * get_packet() { return (void *) & data; };
		inline unsigned get_packet_size() { return sizeof(data); };

	public:

		CCorrelationResultInfoPacket(t_recv_fun recv_fun, t_send_fun send_fun) : CPacket(recv_fun, send_fun) {};

		struct __attribute__((packed))
		{
			uint32_t mark;
			uint32_t result;
			uint32_t result_num;
		} data;
};

class CCorrelationResultPacket : public CPacket
{
	protected:

		void packet_pack();
		void packet_unpack();

		inline unsigned get_command() { return CARD_COMMAND_CORRELATION_RESULT; };
		inline void * get_packet() { return (void *) & data; };
		inline unsigned get_packet_size() { return sizeof(data); };

	public:

		CCorrelationResultPacket(t_recv_fun recv_fun, t_send_fun send_fun) : CPacket(recv_fun, send_fun) {};

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
};

class CCorrelationPacket : public CPacket
{
	protected:

		void packet_pack() {};
		void packet_unpack() {};

		inline unsigned get_command() { return CARD_COMMAND_CORRELATION; };
		inline void * get_packet() { return NULL; };
		inline unsigned get_packet_size() { return 0; };

	public:

		CCorrelationPacket(t_recv_fun recv_fun, t_send_fun send_fun) : CPacket(recv_fun, send_fun) {};
};

class CExitPacket : public CPacket
{
	protected:

		void packet_pack() {};
		void packet_unpack() {};

		inline unsigned get_command() { return CARD_COMMAND_EXIT; };
		inline void * get_packet() { return NULL; };
		inline unsigned get_packet_size() { return 0; };

	public:

		CExitPacket(t_recv_fun recv_fun, t_send_fun send_fun) : CPacket(recv_fun, send_fun) {};
};

class CArincDonePacket : public CPacket
{
	protected:

		void packet_pack() {};
		void packet_unpack() {};

		inline unsigned get_command() { return CARD_COMMAND_ARINC_DONE; };
		inline void * get_packet() { return NULL; };
		inline unsigned get_packet_size() { return 0; };

	public:

		CArincDonePacket(t_recv_fun recv_fun, t_send_fun send_fun) : CPacket(recv_fun, send_fun) {};
};

#endif

