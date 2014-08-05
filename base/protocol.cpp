
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
	static void pack(TYPE * buf, const unsigned num = 1)
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
	static void unpack(TYPE * buf, const unsigned num = 1)
{
	CFile::pack<TYPE>(buf, num);
}

// ############################################################################ 

unsigned CPacket::recv_header()
{
	unsigned notused;

	return recv_header(notused);
}

unsigned CPacket::recv_header(unsigned & ind)
{
	recv_fun((void *) header, sizeof(header));

	// TODO header unpack

	throw_if(header.mark != CARD_MARK);
	ind = header.ind;

	return header.command;
}

void CPacket::__send(const unsigned command, const unsigned ind, const void * packet, const unsigned size)
{
	header.mark = CARD_MARK;
	header.command = command;
	header.ind = ind;

	// TODO header_pack

	send_fun((void *) & header, sizeof(header));
	send_fun(packet, size);
}

void CImagePacket::recv()
{
	recv_fun((void *) data, sizeof(data));

	// TODO unpack

	throw_if(data.mark != CARD_MARK);
}

void CImagePacket::send(const unsigned ind)
{
	// TODO data pack

	__send(CARD_COMMAND_IMAGE, ind, (void *) & data, sizeof(data));
}

