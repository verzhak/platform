
#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "all.hpp"

class CCommand
{
	const unsigned max_packet_size = 4096;
	const int delay = 1000, correlation_delay = 30000, arinc_delay = -1;;
	unsigned ind;
	QTcpSocket card_sock, arinc_sock;

	void send_image(const Mat & img);
	void send_orientation(const QMap<QString, QVariant> & metadata);
	void correlation();

	public:

		CCommand(const QHostAddress card_address, const uint16_t card_port, const QHostAddress arinc_address, const uint16_t arinc_port);

		bool operator()(const Mat & img, const QMap<QString, QVariant> & metadata);
};

#endif

