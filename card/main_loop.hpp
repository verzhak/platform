
#ifndef MAIN_LOOP_HPP
#define MAIN_LOOP_HPP

#include "all.hpp"
#include "map.hpp"
#include "socket.hpp"

class CMainLoop
{
	int server_sock, client_sock;
	unsigned ind;
	double x, y, h, course, roll, pitch, aspect_x, aspect_y;
	CMyMap & __map;
	CPci & __pci;

	void orientation(CTree & packet);
	unsigned correlation(CTree & packet);
	void send_correlation_result(const unsigned result);

	public:

		CMainLoop(CMyMap & map, CPci & pci, const int port);
		~CMainLoop();

		void run();
};

#endif

