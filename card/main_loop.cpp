
#include "main_loop.hpp"

CMainLoop::CMainLoop(const int port, const int combine_port) :
	__combine_port(combine_port)
{
	sockaddr_in addr;

	throw_if(port < 1024);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	throw_if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0);
	throw_if(bind(sock, (sockaddr *) & addr, sizeof(addr)));
}

CMainLoop::~CMainLoop()
{
	close(sock);
}

void CMainLoop::read_packet(void * packet, const unsigned size)
{
	throw_if(recv(sock, packet, size, 0) != size);
	throw_if(* ((uint32_t *) packet) != CARD_MARK);
}

void CMainLoop::run()
{
	bool is_run = true;
	SCardHeaderPacket packet;

	while(is_run)
	{
		try
		{
			read_packet(& packet, sizeof(packet));

			switch(packet.command)
			{
				case CARD_COMMAND_ORIENTATION:
				{
					orientation();

					break;
				}
				case CARD_COMMAND_CORRELATION:
				{
					correlation();

					break;
				}
				case CARD_EXIT:
				{
					is_run = false;

					break;
				}
			}
		}
		catch(...)
		{
			;
		}
	}
}

void CMainLoop::orientation()
{
	int combine_sock = -1;

	try
	{
		SCardOrientationPacket orientation_packet;
		
		read_packet(& orientation_packet, sizeof(orientation_packet));

		unsigned command;
		const SPacketSetAll packet =
		{
			.x = orientation_packet.x,
			.y = orientation_packet.y,
			.h = orientation_packet.h,
			.course = orientation_packet.course,
			.roll = orientation_packet.roll,
			.pitch = orientation_packet.pitch
		};
		SPacketHeader header;
		SPacketGetSet aspect_packet;
		sockaddr_in address;
		
		address.sin_family = AF_INET;
		address.sin_port = htons(__combine_port);
		address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

		throw_if((combine_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0);
		throw_if(connect(combine_sock, (sockaddr *) & address, sizeof(address)));

		auto __send = [ this, address, & combine_sock, & header ](const unsigned command, const void * packet, const unsigned size_of_packet)
		{
			header.command = command;
			header.size = size_of_packet;

			DEBUG;
			throw_if(send(combine_sock, & header, sizeof(header), 0) != sizeof(header));
			DEBUG;
			throw_if(send(combine_sock, packet, size_of_packet, 0) != size_of_packet);
			DEBUG;
		};

		header.port = 111111; // TODO Переназначить на другой порт

		switch(orientation_packet.coord_system)
		{
			case 1:
			{
				command = COMMAND_SET_ALL_GL;

				break;
			}
			case 2:
			{
				command = COMMAND_SET_ALL_GEO;

				break;
			}
			case 3:
			{
				command = COMMAND_SET_ALL_GAUSS_KRUEGER;

				break;
			}
			default:
				throw_;
		}

		__send(command, & packet, sizeof(packet));

		aspect_packet.value = orientation_packet.aspect_x;
		__send(COMMAND_SET_ASPECT_X, & aspect_packet, sizeof(aspect_packet));

		aspect_packet.value = orientation_packet.aspect_y;
		__send(COMMAND_SET_ASPECT_Y, & aspect_packet, sizeof(aspect_packet));
	}
	catch(...)
	{
		;
	}

	if(combine_sock >= 0)
		close(combine_sock);
}

void CMainLoop::correlation()
{
	printf_TODO("correlation\n");
}


