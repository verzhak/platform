
#ifndef QT_SOCKET_HPP
#define QT_SOCKET_HPP

#include "all.hpp"

class CQtSocket : public CSocket
{
	QTcpSocket & __sock;

	const unsigned default_delay = 1000;
	unsigned delay;

	protected:

		unsigned raw_recv(uint8_t * buf, const unsigned size);
		unsigned raw_send(const uint8_t * buf, const unsigned size);

	public:

		CQtSocket(QTcpSocket & sock);

		CXML recv();
		CXML recv(unsigned __delay);
};

#endif

