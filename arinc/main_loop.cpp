
#include "main_loop.hpp"

CMainLoop::CMainLoop(int argc, char * argv[], const uint16_t port) :
	QCoreApplication(argc, argv), height(0), width(0), client_sock(NULL)
{
	throw_if(! server_sock.listen(QHostAddress::AnyIPv4, port));

	connect(& server_sock, SIGNAL(newConnection()), this, SLOT(new_connection()));
	connect(this, SIGNAL(arinc_write(const uint8_t *, const unsigned, const unsigned)), this, SLOT(__arinc_write(const uint8_t *, const unsigned, const unsigned)));
	connect(this, SIGNAL(arinc_done()), this, SLOT(__arinc_done()));
}

void CMainLoop::new_connection()
{
	if(client_sock == NULL)
	{
		throw_null(client_sock = server_sock.nextPendingConnection());

		connect(client_sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
		connect(client_sock, SIGNAL(readyRead()), this, SLOT(command()));
	}
}

void CMainLoop::error(QAbstractSocket::SocketError err)
{
	if(err == QAbstractSocket::RemoteHostClosedError)
	{
		client_sock->disconnect(SIGNAL(readyRead()));
		client_sock = NULL;
	}
}

void CMainLoop::command()
{
	try
	{
		CQtSocket sock(* client_sock);
		CXML packet = sock.recv();

		switch(packet["command"].uint())
		{
			case CARD_COMMAND_IMAGE_HEADER:
			{
				image(sock, packet);

				break;
			}
			default:
				throw_;
		}
	}
	catch(...)
	{
		;
	}
}

void CMainLoop::image(CQtSocket & sock, CXML & packet)
{
	try
	{
		const unsigned current_height = packet["height"].uint(), current_width = packet["width"].uint();
		const unsigned buf_size = current_height * current_width;
		unsigned size, block_size;
		uint8_t * ptr;
		shared_ptr<uint8_t> block_buffer;

		ind = packet["ind"].uint();

		if(height != current_height || width != current_width)
		{
			img.reset(new uint8_t[buf_size], std::default_delete<uint8_t[]>());

			height = current_height;
			width = current_width;
		}

		throw_null(ptr = img.get());

		for(size = 0; size < buf_size; )
		{
			CXML block = sock.recv();

			throw_if(block["command"].uint() != CARD_COMMAND_IMAGE_BLOCK);
			throw_if(block["ind"].uint() != ind);

			protocol::decode_base64(block["data"].value, block_buffer, block_size);

			memcpy(ptr + size, block_buffer.get(), block_size);
			size += block_size;
		}

		emit arinc_write(img.get(), height, width);
	}
	catch(...)
	{
		;
	}
}

void CMainLoop::__arinc_done()
{
	try
	{
		CQtSocket sock(* client_sock);

		sock.send
		(
			protocol::base
			(
				CARD_COMMAND_ARINC_DONE,
				ind
			)
		);
	}
	catch(...)
	{
		;
	}
}

void CMainLoop::__arinc_write(const uint8_t * img, const unsigned height, const unsigned width)
{
	unsigned v;
	Mat __img;

	__img.create(height, width, CV_8U);

	for(v = 0; v < height; v++)
		memcpy(__img.data + __img.step[0] * v, img + v * width, width);

	imshow("todo_img", __img);

	emit arinc_done();
}

