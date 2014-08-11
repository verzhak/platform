
#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "base/all.hpp"
#include "base/tree.hpp"
#include "base/protocol.hpp"

class CSocket
{
	string buf;

	protected:

		virtual unsigned raw_recv(uint8_t * buf, const unsigned size) = 0;
		virtual unsigned raw_send(const uint8_t * buf, const unsigned size) = 0;

	public:

		CSocket();

		virtual CTree recv();
		virtual void send(const CTree & packet);
};

#endif

