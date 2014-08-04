
#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "all.hpp"

class CCommand
{
	const QHostAddress __card_address, __arinc_address;
	const uint16_t __card_port, __arinc_port;
	const unsigned max_packet_size = 4096;
	QUdpSocket sock;

	public:

		CCommand(const QHostAddress card_address, const uint16_t card_port, const QHostAddress arinc_address, const uint16_t arinc_port);

		void send_image(const Mat & img);
		void send_orientation(const QMap<QString, QVariant> & metadata);
		void correlation();
};

#endif

