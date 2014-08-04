
#include "main_loop.hpp"

CMainLoop::CMainLoop(int argc, char * argv[], const uint16_t port) :
	QCoreApplication(argc, argv)
{
	if(! sock.bind(port))
		throw_;

	connect(& sock, SIGNAL(readyRead()), this, SLOT(command()));
	connect(this, SIGNAL(get_image(Mat)), this, SLOT(todo(Mat)));
}

void CMainLoop::command()
{
	try
	{
		SCardHeaderPacket header;

		throw_if(sock.readDatagram((char *) & header, sizeof(header)) != sizeof(header));
		qDebug() << header.mark << header.command;
		throw_if(header.mark != CARD_MARK);

		switch(header.command)
		{
			case CARD_COMMAND_IMAGE:
			{
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

void CMainLoop::read_packet(void * packet, const unsigned size)
{
	throw_if(sock.readDatagram((char *) packet, size) != size);
	throw_if(* ((uint32_t *) packet) != CARD_MARK);
}

void CMainLoop::image()
{
	try
	{
		SCardImagePacket packet;

		read_packet(& packet, sizeof(packet));

		const unsigned height = packet.height, width = packet.width, block_size = packet.block_size;
		const unsigned buf_size = height * width;
		const unsigned blocks_num_1 = buf_size / block_size, last_block_size = buf_size % block_size;
		unsigned v;
		shared_ptr<uint8_t> buf(new uint8_t[buf_size], std::default_delete<uint8_t[]>());
		uint8_t * ptr = buf.get();

		auto get_block = [ this ](uint8_t * ptr, const unsigned block_size)
		{
			throw_if(! sock.waitForReadyRead(wait_for_ready_read));
			throw_if(sock.readDatagram((char *) ptr, block_size) != block_size);
		};

		throw_null(ptr);

		for(v = 0; v < blocks_num_1; v++)
			get_block(ptr + v * block_size, block_size);

		if(last_block_size)
			get_block(ptr + blocks_num_1 * block_size, last_block_size);

		img.create(height, width, CV_8U);

		for(v = 0; v < height; v++)
			memcpy(img.data + img.step[0] * v, ptr + v * width, width);

		emit get_image(img.clone());
	}
	catch(...)
	{
		;
	}
}

void CMainLoop::todo(Mat todo_img)
{
	imshow("todo_img", todo_img);
}

