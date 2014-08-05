
#ifndef MAIN_LOOP_HPP
#define MAIN_LOOP_HPP

#include "all.hpp"

class CMainLoop : public QCoreApplication
{
	Q_OBJECT

	const unsigned delay = 1000;
	unsigned arinc_ind, height, width;
	shared_ptr<uint8_t> img;
	QTcpServer server_sock;
	QTcpSocket * client_sock;

	void read_packet(void * packet, const unsigned size);
	void image();

	public:

		CMainLoop(int argc, char * argv[], const uint16_t port);

	public slots:

		void new_connection();
		void command();
		void error(QAbstractSocket::SocketError err);
		void __arinc_write(const uint8_t * img, const unsigned height, const unsigned width);
		void __arinc_done();

	signals:

		void arinc_write(const uint8_t * img, const unsigned height, const unsigned width);
		void arinc_done();
};

#endif

