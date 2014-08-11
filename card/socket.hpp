
#ifndef CARD_SOCKET_HPP
#define CARD_SOCKET_HPP

#include "all.hpp"

class CCardSocket : public CSocket
{
	const int __sock;

	protected:

		unsigned raw_recv(uint8_t * buf, const unsigned size);
		unsigned raw_send(const uint8_t * buf, const unsigned size);

	public:

		CCardSocket(const int sock);
};

#endif

