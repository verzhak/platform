
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

void CMainLoop::run()
{
	while((client_sock = accept(server_sock, NULL, NULL)) >= 0)
	{
		bool is_run = true;
		CCardSocket sock(client_sock);

		while(is_run)
		{
			try
			{
				CTree packet = sock.recv();

				switch(packet["command"].uint())
				{
					case CARD_COMMAND_ORIENTATION:
					{
						orientation(packet);

						break;
					}
					case CARD_COMMAND_CORRELATION:
					{
						correlation(packet);

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

void CMainLoop::orientation(CTree & packet)
{
	try
	{
		ind = packet["ind"].uint();

		switch(packet["coord_system"].uint())
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

		x = packet["x"].real();
		y = packet["y"].real();
		h = packet["h"].real();
		course = packet["course"].real();
		roll = packet["roll"].real();
		pitch = packet["pitch"].real();
		aspect_x = packet["aspect_x"].real();
		aspect_y = packet["aspect_y"].real();
	}
	catch(...)
	{
		;
	}
}

#include <opencv2/opencv.hpp>
using namespace cv;

void CMainLoop::correlation(CTree & packet)
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

		throw_if(ind != packet["ind"].uint());

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
		CCardSocket sock(client_sock);

		sock.send
		(
			protocol::correlation_info
			(
				ind,
				result,
				results.size()
			)
		);

		for(auto & res : results)
			sock.send
			(
				protocol::correlation_result
				(
					ind,
					res.ab,
					res.cd,
					res.fe,
					res.dx,
					res.dy,
					res.num
				)
			);
	}
	catch(...)
	{
		;
	}
}


