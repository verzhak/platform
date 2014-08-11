
#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "all.hpp"
#include "socket.hpp"

class CCommand
{
	const int delay = 1000, correlation_delay = 30000, arinc_delay = -1;
	const unsigned __block_size;
	unsigned ind;
	QTcpSocket card_sock, arinc_sock;

	void send_image(const Mat & img);
	void send_orientation(const QMap<QString, QVariant> & metadata);
	void correlation();

	public:

		CCommand(const QHostAddress card_address, const uint16_t card_port, const QHostAddress arinc_address, const uint16_t arinc_port, const unsigned block_size);

		bool operator()(const Mat & img, const QMap<QString, QVariant> & metadata);
};

#endif

