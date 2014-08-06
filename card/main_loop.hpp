
#ifndef MAIN_LOOP_HPP
#define MAIN_LOOP_HPP

#include "all.hpp"
#include "map.hpp"
#include "pci.hpp"

class CMainLoop
{
	int server_sock, client_sock;
	unsigned orientation_ind, correlation_ind;
	shared_ptr<uint8_t> img_3, img;
	double x, y, h, course, roll, pitch, aspect_x, aspect_y;
	ECoordType coord_system;
	CMap & __map;
	CPci & __pci;
	t_recv_fun recv_fun;
	t_send_fun send_fun;

	void orientation(COrientationPacket * packet);
	void correlation(CCorrelationPacket * packet);

	public:

		CMainLoop(CMap & map, CPci & pci, const int port);
		~CMainLoop();

		void run();
};

#endif

