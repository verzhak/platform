
#include "main_loop.hpp"

CMainLoop::CMainLoop(CMyMap & map, CPci & pci, const int port) :
	server_sock(-1), client_sock(-1), __map(map), __pci(pci)
{
	const int is_reuseaddr = 1;
	sockaddr_in addr;

	throw_if(port < 1024);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	throw_if((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0);
	throw_if(setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, & is_reuseaddr, sizeof(is_reuseaddr)));
	throw_if(bind(server_sock, (sockaddr *) & addr, sizeof(addr)));
	throw_if(listen(server_sock, 1));
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
				CXML packet = sock.recv();

				switch(packet["command"].uint())
				{
					case CARD_COMMAND_ORIENTATION:
					{
						orientation(packet);

						break;
					}
					case CARD_COMMAND_CORRELATION:
					{
						send_correlation_result(correlation(packet));

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

void CMainLoop::orientation(CXML & packet)
{
	try
	{
		ind = packet["ind"].uint();

		x = packet["x"].real();
		y = packet["y"].real();
		h = packet["h"].real();
		course = packet["course"].real();
		roll = packet["roll"].real();
		pitch = packet["pitch"].real();
		aspect_x = packet["aspect_x"].real();
		aspect_y = packet["aspect_y"].real();
		// coord_system
	}
	catch(...)
	{
		;
	}
}

unsigned CMainLoop::correlation(CXML & packet)
{
	unsigned result = CARD_CORRELATION_RESULT_SUCCESS;

	try
	{
		throw_if(ind != packet["ind"].uint());

		// ############################################################################ 
		// Карта

		unsigned pnts_num;
		const vector< vector<CVertex> > cnts = __map.contours(pnts_num);
		const unsigned cnts_num = cnts.size();
		const unsigned elem_num = cnts_num + pnts_num * 3;

		// ############################################################################ 
		// Матрицы

		const double xy_size_2 = 100;
		const double h_size_2 = 100;
		const double angle_size_2 = 10;

		const unsigned step_per_xy = 3;
		const unsigned step_per_h = 3;
		const unsigned step_per_angle = 5;

		const unsigned matrices_num = step_per_xy * step_per_xy * step_per_h * step_per_angle * step_per_angle * step_per_angle;
		const unsigned matrices_size = matrices_num * 16;

		// ############################################################################ 
		// Выделение памяти под буфер

		shared_ptr<float> buf;
		float * p_buf_cnts, * p_buf_matrices;
		const unsigned buf_size = elem_num + matrices_size;

		buf.reset(new float[buf_size], std::default_delete<float[]>());
		throw_null(p_buf_cnts = buf.get());
		p_buf_matrices = p_buf_cnts + elem_num;

		// ############################################################################ 
		// Карта - заполнение буфера

		for(auto & cnt : cnts)
		{
			for(auto & pnt : cnt)
			{
				p_buf_cnts[0] = pnt.x;
				p_buf_cnts[1] = pnt.y;
				p_buf_cnts[2] = pnt.z;

				p_buf_cnts += 3;
			}

			* ((uint32_t *) p_buf_cnts) = CONTOUR_END;
			p_buf_cnts ++;
		}

		// ############################################################################ 
		// Матрицы - заполнение буфера

		unsigned v;
		const double dxy = 2 * xy_size_2 / step_per_xy;
		const double dh = 2 * h_size_2 / step_per_h;
		const double dangle = 2 * angle_size_2 / step_per_angle;
		unsigned __x, __y, __h, __course, __roll, __pitch;

#define ITER(var, step_num)\
	for(var = 0; var < step_num; var++)

		ITER(__x, step_per_xy)
		ITER(__y, step_per_xy)
		ITER(__h, step_per_h)
		ITER(__course, step_per_angle)
		ITER(__roll, step_per_angle)
		ITER(__pitch, step_per_angle)
		{
			CMatrix mtx = __map(
				x - xy_size_2 + dxy * __x,
				y - xy_size_2 + dxy * __y,
				h - h_size_2 + dh * __h,
				course - angle_size_2 + dangle * __course,
				roll - angle_size_2 + dangle * __roll,
				pitch - angle_size_2 + dangle * __pitch,
				aspect_x, aspect_y);

			for(v = 0; v < 16; v++)
				p_buf_matrices[v] = mtx[v];
			
			p_buf_matrices += 16;
		}

		// ############################################################################ 

		function<void(uint32_t *)> update_regs = [ cnts_num, matrices_num ](uint32_t * reg)
		{
			reg[REG_NUMBER_OF_CONTOUR] = cnts_num;
			reg[REG_NUMBER_OF_MATRICES] = matrices_num; 
		};

		__pci.write(update_regs, (uint8_t *) buf.get(), elem_num * sizeof(float), (uint8_t *) (buf.get() + elem_num), matrices_size * sizeof(float));
	}
	catch(...)
	{
		result = CARD_CORRELATION_RESULT_FAIL;
	}

	return result;
}

void CMainLoop::send_correlation_result(const unsigned result)
{
	try
	{
		const vector<s_result> results = __pci.read();
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

