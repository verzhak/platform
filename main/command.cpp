
#include "command.hpp"

CCommand::CCommand(const QHostAddress card_address, const uint16_t card_port, const QHostAddress arinc_address, const uint16_t arinc_port) :
	__card_address(card_address), __arinc_address(arinc_address), __card_port(card_port), __arinc_port(arinc_port)
{
	;
}

void CCommand::send_image(const Mat & img)
{
	const unsigned height = img.rows, width = img.cols;
	const unsigned buf_size = height * width;
	const unsigned blocks_num_1 = buf_size / max_packet_size, last_block_size = buf_size % max_packet_size;
	unsigned v;
	shared_ptr<uint8_t> buf(new uint8_t[buf_size], std::default_delete<uint8_t[]>());
	uint8_t * ptr = buf.get();
	const SCardHeaderPacket header = { .mark = CARD_MARK, .command = CARD_COMMAND_IMAGE };
	const SCardImagePacket packet = { .mark = CARD_MARK, .height = height, .width = width, .block_size = max_packet_size };

	throw_null(ptr);

	throw_if(sock.writeDatagram((const char *) & header, sizeof(header), __arinc_address, __arinc_port) != sizeof(header));
	throw_if(sock.writeDatagram((const char *) & packet, sizeof(packet), __arinc_address, __arinc_port) != sizeof(packet));

	for(v = 0; v < height; v++)
		memcpy(ptr + v * width, img.data + img.step[0] * v, width);

	for(v = 0; v < blocks_num_1; v++)
		throw_if(sock.writeDatagram((const char *) (ptr + v * max_packet_size), max_packet_size, __arinc_address, __arinc_port) != max_packet_size);

	if(last_block_size)
		throw_if(sock.writeDatagram((const char *) (ptr + blocks_num_1 * max_packet_size), last_block_size, __arinc_address, __arinc_port) != last_block_size);
}

void CCommand::send_orientation(const QMap<QString, QVariant> & metadata)
{
	const SCardHeaderPacket header = { .mark = CARD_MARK, .command = CARD_COMMAND_ORIENTATION };
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

	throw_if(sock.writeDatagram((const char *) & header, sizeof(header), __card_address, __card_port) != sizeof(header));
	throw_if(sock.writeDatagram((const char *) & packet, sizeof(packet), __card_address, __card_port) != sizeof(packet));
}

void CCommand::correlation()
{
	const SCardHeaderPacket header = { .mark = CARD_MARK, .command = CARD_COMMAND_CORRELATION };

	throw_if(sock.writeDatagram((const char *) & header, sizeof(header), __card_address, __card_port) != sizeof(header));
}

