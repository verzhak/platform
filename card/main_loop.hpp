
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

	bool read_packet(void * packet, const unsigned size);
	bool write_packet(void * packet, const unsigned size);

	void orientation();
	void correlation();

	public:

		CMainLoop(CMap & map, CPci & pci, const int port);
		~CMainLoop();

		void run();
};

#endif

