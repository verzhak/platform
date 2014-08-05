
#include "command.hpp"

CCommand::CCommand(const QHostAddress card_address, const uint16_t card_port, const QHostAddress arinc_address, const uint16_t arinc_port)
{
	ind = 0;

	card_sock.connectToHost(card_address, card_port);
	arinc_sock.connectToHost(arinc_address, arinc_port);

	card_sock.waitForConnected(delay);
	arinc_sock.waitForConnected(delay);
}

void CCommand::send_image(const Mat & img)
{
	throw_if(arinc_sock.state() != QAbstractSocket::ConnectedState);

	try
	{
		const unsigned height = img.rows, width = img.cols;
		const unsigned buf_size = height * width;
		const unsigned blocks_num_1 = buf_size / max_packet_size, last_block_size = buf_size % max_packet_size;
		unsigned v;
		shared_ptr<uint8_t> buf(new uint8_t[buf_size], std::default_delete<uint8_t[]>());
		uint8_t * ptr = buf.get();
		SCardHeaderPacket header = { .mark = CARD_MARK, .command = CARD_COMMAND_IMAGE, .ind = ind };
		const SCardImagePacket packet = { .mark = CARD_MARK, .height = height, .width = width, .block_size = max_packet_size };

		throw_null(ptr);

		throw_if(arinc_sock.write((const char *) & header, sizeof(header)) != sizeof(header));
		throw_if(! arinc_sock.waitForBytesWritten(-1));

		throw_if(arinc_sock.write((const char *) & packet, sizeof(packet)) != sizeof(packet));
		throw_if(! arinc_sock.waitForBytesWritten(-1));

		for(v = 0; v < height; v++)
			memcpy(ptr + v * width, img.data + img.step[0] * v, width);

		for(v = 0; v < blocks_num_1; v++)
			throw_if(arinc_sock.write((const char *) (ptr + v * max_packet_size), max_packet_size) != max_packet_size);

		if(last_block_size)
			throw_if(arinc_sock.write((const char *) (ptr + blocks_num_1 * max_packet_size), last_block_size) != last_block_size);

		throw_if(! arinc_sock.waitForBytesWritten(-1));
		throw_if(! arinc_sock.waitForReadyRead(arinc_delay));

		throw_if(arinc_sock.read((char *) & header, sizeof(header)) != sizeof(header));
		throw_if(header.mark != CARD_MARK);
		throw_if(header.command != CARD_COMMAND_ARINC_DONE);
	}
	catch(...)
	{
		;
	}
}

void CCommand::send_orientation(const QMap<QString, QVariant> & metadata)
{
	throw_if(card_sock.state() != QAbstractSocket::ConnectedState);

	try
	{
		const SCardHeaderPacket header = { .mark = CARD_MARK, .command = CARD_COMMAND_ORIENTATION, .ind = ind };
		const SCardOrientationPacket packet =
		{
			.mark = CARD_MARK, 
			.x = metadata["x"].toDouble(),
			.y = metadata["y"].toDouble(),
			.h = metadata["h"].toDouble(),
			.course = metadata["course"].toDouble(),
			.roll = metadata["roll"].toDouble(),
			.pitch = metadata["pitch"].toDouble(),
			.aspect_x = metadata["aspect_x"].toDouble(),
			.aspect_y = metadata["aspect_y"].toDouble(),
			.coord_system = metadata["coord_system"].toUInt()
		};

		throw_if(card_sock.write((const char *) & header, sizeof(header)) != sizeof(header));
		throw_if(! card_sock.flush());

		throw_if(card_sock.write((const char *) & packet, sizeof(packet)) != sizeof(packet));
		throw_if(! card_sock.flush());
	}
	catch(...)
	{
		;
	}
}

void CCommand::correlation()
{
	throw_if(card_sock.state() != QAbstractSocket::ConnectedState);

	try
	{
		unsigned v;
		SCardHeaderPacket header = { .mark = CARD_MARK, .command = CARD_COMMAND_CORRELATION, .ind = ind };
		SCardCorrelationResultInfoPacket info_packet;
		SCardCorrelationResultPacket packet;

		throw_if(card_sock.write((const char *) & header, sizeof(header)) != sizeof(header));
		throw_if(! card_sock.flush());

		throw_if(! card_sock.waitForReadyRead(correlation_delay));

		throw_if(card_sock.read((char *) & header, sizeof(header)) != sizeof(header));
		throw_if(header.mark != CARD_MARK);
		throw_if(header.command != CARD_COMMAND_CORRELATION_RESULT);

		throw_if(card_sock.read((char *) & info_packet, sizeof(info_packet)) != sizeof(info_packet));
		throw_if(info_packet.mark != CARD_MARK);

		for(v = 0; v < info_packet.result_num; v++)
		{
			throw_if(card_sock.read((char *) & packet, sizeof(packet)) != sizeof(packet));
			throw_if(packet.mark != CARD_MARK);

			printf_TODO("Correlation result %u of %u (%lf%%): ab = %u, cd = %u, fe = %u, dx = %u, dy = %u, num = %u\n",
				(v + 1), info_packet.result_num, ((v + 1) * 100.) / info_packet.result_num,
				packet.ab, packet.cd, packet.fe, packet.dx, packet.dy, packet.num);
		}
	}
	catch(...)
	{
		;
	}
}

bool CCommand::operator()(const Mat & img, const QMap<QString, QVariant> & metadata)
{
	bool ret = true;

	try
	{
		send_image(img);
		send_orientation(metadata);
		correlation();

		ind++;
	}
	catch(...)
	{
		ret = false;
	}

	return ret;
}

