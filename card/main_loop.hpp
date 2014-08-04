
#ifndef MAIN_LOOP_HPP
#define MAIN_LOOP_HPP

#include "all.hpp"

class CMainLoop
{
	const int __combine_port;
	int sock;

	void read_packet(void * packet, const unsigned size);

	void orientation();
	void correlation();

	public:

		CMainLoop(const int port, const int combine_port);
		~CMainLoop();

		void run();
};

#endif

