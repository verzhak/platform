
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

void CMainLoop::read_packet(void * packet, const unsigned size)
{
	while(client_sock->bytesAvailable() < size)
		client_sock->waitForReadyRead(delay);

	throw_if(client_sock->read((char *) packet, size) != size);
}

void CMainLoop::command()
{
	try
	{
		SCardHeaderPacket header;

		read_packet(& header, sizeof(header));
		throw_if(header.mark != CARD_MARK);

		switch(header.command)
		{
			case CARD_COMMAND_IMAGE:
			{
				arinc_ind = header.ind;
				image();

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

void CMainLoop::image()
{
	try
	{
		SCardImagePacket packet;

		read_packet(& packet, sizeof(packet));
		throw_if(packet.mark != CARD_MARK);

		const unsigned block_size = packet.block_size;
		const unsigned buf_size = packet.height * packet.width;
		const unsigned blocks_num_1 = buf_size / block_size, last_block_size = buf_size % block_size;
		unsigned v;
		uint8_t * ptr;

		if(height != packet.height || width != packet.width)
		{
			img.reset(new uint8_t[buf_size], std::default_delete<uint8_t[]>());

			height = packet.height;
			width = packet.width;
		}

		throw_null(ptr = img.get());

		for(v = 0; v < blocks_num_1; v++)
			read_packet(ptr + v * block_size, block_size);

		if(last_block_size)
			read_packet(ptr + blocks_num_1 * block_size, last_block_size);

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
		const SCardHeaderPacket header = { .mark = CARD_MARK, .command = CARD_COMMAND_ARINC_DONE, .ind = arinc_ind };

		throw_if(client_sock->write((const char *) & header, sizeof(header)) != sizeof(header));
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

