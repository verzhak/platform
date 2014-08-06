
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

	recv_fun = [ & client_sock ](void * packet, const unsigned size)
	{
		const ssize_t recv_size = recv(client_sock, packet, size, 0);

		// TODO
		// if(recv_size <= 0)
		//	return false;

		throw_if(recv_size != size);
	};

	send_fun = [ & client_sock ](const void * packet, const unsigned size)
	{
		const ssize_t send_size = send(client_sock, packet, size, 0);

		// TODO
		// if(send_size < 0)
		//	return false;

		throw_if(send_size != size);
	};
}

CMainLoop::~CMainLoop()
{
	if(server_sock >= 0)
		close(server_sock);

	if(client_sock >= 0)
		close(client_sock);
}

void CMainLoop::run()
{
	while((client_sock = accept(server_sock, NULL, NULL)) >= 0)
	{
		bool is_run = true;

		while(is_run)
		{
			try
			{
				unsigned ind;
				CHeaderPacket header(recv_fun, send_fun);
				CPacket * packet;

				// TODO Ссылка на указатель
				switch(header.recv(& packet, ind))
				{
					case CARD_COMMAND_ORIENTATION:
					{
						orientation_ind = header->ind;
						orientation(dynamic_cast<COrientationPacket *>(packet));

						break;
					}
					case CARD_COMMAND_CORRELATION:
					{
						correlation_ind = header->ind;
						correlation(dynamic_cast<CCorrelationPacket *>(packet));

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

void CMainLoop::orientation(COrientationPacket * packet)
{
	try
	{
		switch(packet->data.coord_system)
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

		x = packet->data.x;
		y = packet->data.y;
		h = packet->data.h;
		course = packet->data.course;
		roll = packet->data.roll;
		pitch = packet->data.pitch;
		aspect_x = packet->data.aspect_x;
		aspect_y = packet->data.aspect_y;
	}
	catch(...)
	{
		;
	}
}

#include <opencv2/opencv.hpp>
using namespace cv;

void CMainLoop::correlation(CCorrelationPacket * packet)
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
		CHeaderPacket header(CARD_COMMAND_CORRELATION_RESULT, correlation_ind);
		CCorrelationResultInfoPacket info_packet;
		CCorrelationResultPacket packet;

		info_packet.data.result = result;
		info_packet.data.result_num = results.size();

		info_packet.send(correlation_ind);

		for(auto & res : results)
		{
			packet.data.ab = res.ab;
			packet.data.cd = res.cd;
			packet.data.fe = res.fe;
			packet.data.dx = res.dx;
			packet.data.dy = res.dy;
			packet.data.num = res.num;

			packet.send(correlation_ind);
		}
	}
	catch(...)
	{
		;
	}
}


