
#include "main_loop.hpp"

CMainLoop::CMainLoop(CMap & map, CPci & pci, const int port) :
	server_sock(-1), client_sock(-1), __map(map), __pci(pci)
{
	const int is_reuseaddr = 1;
	const unsigned height_width = __map.height * __map.width;
	sockaddr_in addr;

	throw_if(port < 1024);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	throw_if((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0);
	throw_if(setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, & is_reuseaddr, sizeof(is_reuseaddr)));
	throw_if(bind(server_sock, (sockaddr *) & addr, sizeof(addr)));
	throw_if(listen(server_sock, 1));

	img_3.reset(new uint8_t[height_width * 3], std::default_delete<uint8_t[]>());
	throw_null(img_3.get());

	img.reset(new uint8_t[height_width], std::default_delete<uint8_t[]>());
	throw_null(img.get());
}

CMainLoop::~CMainLoop()
{
	if(server_sock >= 0)
		close(server_sock);

	if(client_sock >= 0)
		close(client_sock);
}

bool CMainLoop::read_packet(void * packet, const unsigned size)
{
	const ssize_t recv_size = recv(client_sock, packet, size, 0);

	if(recv_size <= 0)
		return false;

	throw_if(recv_size != size);
	throw_if(* ((uint32_t *) packet) != CARD_MARK);

	return true;
}

bool CMainLoop::write_packet(void * packet, const unsigned size)
{
	const ssize_t send_size = send(client_sock, packet, size, 0);

	if(send_size < 0)
		return false;

	throw_if(send_size != size);

	return true;
}

void CMainLoop::run()
{
	SCardHeaderPacket packet;

	while((client_sock = accept(server_sock, NULL, NULL)) >= 0)
	{
		bool is_run = true;

		while(is_run)
		{
			try
			{
				if(
					(is_run = read_packet(& packet, sizeof(packet)))
				  )
					switch(packet.command)
					{
						case CARD_COMMAND_ORIENTATION:
						{
							orientation_ind = packet.ind;
							orientation();

							break;
						}
						case CARD_COMMAND_CORRELATION:
						{
							correlation_ind = packet.ind;
							correlation();

							break;
						}
						case CARD_COMMAND_EXIT:
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

		close(client_sock);
		client_sock = -1;
	}
}

void CMainLoop::orientation()
{
	try
	{
		SCardOrientationPacket orientation_packet;
		
		read_packet(& orientation_packet, sizeof(orientation_packet));

		switch(orientation_packet.coord_system)
		{
			case 0:
			{
				coord_system = ECT_GL;

				break;
			}
			case 2:
			{
				coord_system = ECT_GEO;

				break;
			}
			case 3:
			{
				coord_system = ECT_GAUSS_KRUEGER;

				break;
			}
			default:
			{
				throw_;
			}
		}

		x = orientation_packet.x;
		y = orientation_packet.y;
		h = orientation_packet.h;
		course = orientation_packet.course;
		roll = orientation_packet.roll;
		pitch = orientation_packet.pitch;
		aspect_x = orientation_packet.aspect_x;
		aspect_y = orientation_packet.aspect_y;
	}
	catch(...)
	{
		;
	}
}

#include <opencv2/opencv.hpp>
using namespace cv;

void CMainLoop::correlation()
{
	unsigned result = CARD_CORRELATION_RESULT_UNKNOWN;

	try
	{
		const unsigned height = __map.height, width = __map.width;
		const unsigned wnd_height = 512, wnd_width = 512;
		const unsigned from_v = (height - wnd_height) / 2, from_u = (width - wnd_width) / 2;
		const unsigned to_v = from_v + wnd_height, to_u = from_u + wnd_width;
		unsigned v, u, w_v, w_u;
		uint8_t * ptr_3 = img_3.get(), * ptr = img.get();

		throw_if(orientation_ind != correlation_ind);

		__pci.reset();

		__map(img_3.get(), x, y, h, course, roll, pitch, coord_system, aspect_x, aspect_y);

		for(v = from_v, w_v = 0; v < to_v; v++, w_v++)
			for(u = from_u, w_u = 0; u < to_u; u++, w_u++)
			{
				const unsigned ind_3 = (v * width + u) * 3;
				const unsigned ind = w_v * wnd_width + w_u;

				ptr[ind] = 
					(ptr_3[ind_3] > 100 || ptr_3[ind_3 + 1] > 100 || ptr_3[ind_3 + 2] > 100)
					?
					255 : 0;
			}

		Mat todo(wnd_height, wnd_width, CV_8U);

		for(v = 0; v < wnd_height; v++)
			for(u = 0; u < wnd_width; u++)
				todo.at<uint8_t>(v, u) = ptr[v * wnd_width + u];

		imshow("todo", todo);
		waitKey(40);
	}
	catch(...)
	{
		result = CARD_CORRELATION_RESULT_FAIL;
	}

	try
	{
		const vector<s_result> results = __pci.results();
		SCardHeaderPacket header = { .mark = CARD_MARK, .command = CARD_COMMAND_CORRELATION_RESULT, .ind = correlation_ind };
		SCardCorrelationResultInfoPacket info_packet = { .mark = CARD_MARK, .result = result, .result_num = results.size() };
		SCardCorrelationResultPacket packet = { .mark = CARD_MARK };

		write_packet(& header, sizeof(header));
		write_packet(& info_packet, sizeof(info_packet));

		for(auto & res : results)
		{
			packet.ab = res.ab;
			packet.cd = res.cd;
			packet.fe = res.fe;
			packet.dx = res.dx;
			packet.dy = res.dy;
			packet.num = res.num;

			write_packet(& packet, sizeof(packet));
		}
	}
	catch(...)
	{
		;
	}
}


