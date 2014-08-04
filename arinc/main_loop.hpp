
#ifndef MAIN_LOOP_HPP
#define MAIN_LOOP_HPP

#include "all.hpp"

class CMainLoop : public QCoreApplication
{
	Q_OBJECT

	const unsigned wait_for_ready_read = 1000;
	QUdpSocket sock;
	Mat img;

	void read_packet(void * packet, const unsigned size);
	void image();

	public:

		CMainLoop(int argc, char * argv[], const uint16_t port);

	public slots:

		void command();
		void todo(Mat todo_img);

	signals:

		void get_image(Mat img);
};

#endif

