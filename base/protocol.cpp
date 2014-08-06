
#include "base/protocol.hpp"

#define PACK(__TYPE, fun)\
else if(typeid(TYPE) == typeid(__TYPE))\
{\
	for(v = 0; v < num; v++)\
		buf[v] = fun(buf[v]);\
}

#define PACK_REAL(__TYPE, __D_TYPE, fun)\
else if(typeid(TYPE) == typeid(__TYPE))\
{\
	__D_TYPE val;\
	\
	for(v = 0; v < num; v++)\
	{\
		val = fun(* (__D_TYPE *) (buf + v));\
		buf[v] = * (TYPE *) & val;\
	}\
}

template<typename TYPE>
void pack(TYPE * buf, const unsigned num = 1)
{
	#ifndef __MINGW32__

		unsigned v;

		if(typeid(TYPE) == typeid(uint8_t))
		{
			;
		}
		PACK(uint16_t, htole16)
		PACK(uint32_t, htole32)
		PACK(uint64_t, htole64)
		PACK_REAL(float, uint32_t, htole32)
		PACK_REAL(double, uint64_t, htole64)
		else
		{
			throw_;
		}

	#endif
}

template<typename TYPE>
void unpack(TYPE * buf, const unsigned num = 1)
{
	pack<TYPE>(buf, num);
}

// ############################################################################ 

CPacket::CPacket(t_recv_fun recv_fun, t_send_fun send_fun) :
	__recv_fun(recv_fun), __send_fun(send_fun)
{
	;
}

void CPacket::header_pack()
{
	pack<uint32_t>(& header.mark);
	pack<uint32_t>(& header.command);
	pack<uint32_t>(& header.ind);
}

void CPacket::header_unpack()
{
	unpack<uint32_t>(& header.mark);
	unpack<uint32_t>(& header.command);
	unpack<uint32_t>(& header.ind);
}

void CPacket::send(const unsigned command, const unsigned ind)
{
	header.mark = CARD_MARK;
	header.command = command;
	header.ind = ind;

	header_pack();

	__send_fun((void *) & header, sizeof(header));

	if(command != CARD_COMMAND_NOTHING)
	{
		* (uint32_t *) packet = CARD_MARK;

		packet_pack();

		__send_fun(packet, size);
	}
}

unsigned CPacket::recv(CPacket ** packet, unsigned & ind)
{
	unsigned command;

	throw_null(packet);

	__recv_fun((void *) & header, sizeof(header));
	header_unpack();

	throw_if(header.mark != CARD_MARK);
	command = header.command;
	ind = header.ind;

	switch(command)
	{
		case CARD_COMMAND_IMAGE:
		{
			* packet = new CImagePacket(__recv_fun, __send_fun);

			break;
		}
		case CARD_COMMAND_ORIENTATION:
		{
			* packet = new COrientationPacket(__recv_fun, __send_fun);

			break;
		}
		case CARD_COMMAND_CORRELATION:
		{
			* packet = new CCorrelationPacket(__recv_fun, __send_fun);

			break;
		}
		case CARD_COMMAND_EXIT:
		{
			* packet = new CExitPacket(__recv_fun, __send_fun);

			break;
		}
		case CARD_COMMAND_CORRELATION_RESULT:
		{
			* packet = new CCorrelationResultPacket(__recv_fun, __send_fun);

			break;
		}
		case CARD_COMMAND_ARINC_DONE:
		{
			* packet = new CArincDonePacket(__recv_fun, __send_fun);

			break;
		}
		case CARD_COMMAND_CORRELATION_RESULT_INFO:
		{
			* packet = new CCorrelationResultInfoPacket(__recv_fun, __send_fun);

			break;
		}
		default:
		{
			throw_;
		}
	}

	__recv_fun((* packet)->get_packet(), (* packet)->get_packet_size());
	(* packet)->packet_unpack();

	throw_if(* (uint32_t *) packet != CARD_MARK);

	return command;
}

// ############################################################################ 

void CImagePacket::packet_pack()
{
	pack<uint32_t>(& data.mark);
	pack<uint32_t>(& data.height);
	pack<uint32_t>(& data.width);
	pack<uint32_t>(& data.block_size);
}

void CImagePacket::packet_unpack()
{
	unpack<uint32_t>(& data.mark);
	unpack<uint32_t>(& data.height);
	unpack<uint32_t>(& data.width);
	unpack<uint32_t>(& data.block_size);
}

// ############################################################################ 

void COrientationPacket::packet_pack()
{
	pack<uint32_t>(& data.mark);
	pack<double>(& data.x);
	pack<double>(& data.y);
	pack<double>(& data.h);
	pack<double>(& data.course);
	pack<double>(& data.roll);
	pack<double>(& data.pitch);
	pack<double>(& data.aspect_x);
	pack<double>(& data.aspect_y);
	pack<uint32_t>(& data.coord_system);
}

void COrientationPacket::packet_unpack()
{
	unpack<uint32_t>(& data.mark);
	unpack<double>(& data.x);
	unpack<double>(& data.y);
	unpack<double>(& data.h);
	unpack<double>(& data.course);
	unpack<double>(& data.roll);
	unpack<double>(& data.pitch);
	unpack<double>(& data.aspect_x);
	unpack<double>(& data.aspect_y);
	unpack<uint32_t>(& data.coord_system);
}

// ############################################################################ 

void CCorrelationResultInfoPacket::packet_pack()
{
	pack<uint32_t>(& data.mark);
	pack<uint32_t>(& data.result);
	pack<uint32_t>(& data.result_num);
}

void CCorrelationResultInfoPacket::packet_unpack()
{
	unpack<uint32_t>(& data.mark);
	unpack<uint32_t>(& data.result);
	unpack<uint32_t>(& data.result_num);
}

// ############################################################################ 

void CCorrelationResultPacket::packet_pack()
{
	pack<uint32_t>(& data.mark);
	pack<uint32_t>(& data.ab);
	pack<uint32_t>(& data.cd);
	pack<uint32_t>(& data.fe);
	pack<uint32_t>(& data.dx);
	pack<uint32_t>(& data.dy);
	pack<uint32_t>(& data.num);
}

void CCorrelationResultPacket::packet_unpack()
{
	unpack<uint32_t>(& data.mark);
	unpack<uint32_t>(& data.ab);
	unpack<uint32_t>(& data.cd);
	unpack<uint32_t>(& data.fe);
	unpack<uint32_t>(& data.dx);
	unpack<uint32_t>(& data.dy);
	unpack<uint32_t>(& data.num);
}

