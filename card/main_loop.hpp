
#ifndef MAIN_LOOP_HPP
#define MAIN_LOOP_HPP

#include "all.hpp"
#include "map.hpp"
#include "pci.hpp"
#include "socket.hpp"

class CMainLoop
{
	int server_sock, client_sock;
	unsigned ind;
	shared_ptr<uint8_t> img_3, img;
	double x, y, h, course, roll, pitch, aspect_x, aspect_y;
	ECoordType coord_system;
	CMap & __map;
	CPci & __pci;

	void orientation(CTree & packet);
	void correlation(CTree & packet);

	public:

		CMainLoop(CMap & map, CPci & pci, const int port);
		~CMainLoop();

		void run();
};

#endif

