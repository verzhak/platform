
#include "command.hpp"

CCommand::CCommand(const QHostAddress card_address, const uint16_t card_port, const QHostAddress arinc_address, const uint16_t arinc_port, const unsigned block_size) :
	__block_size(block_size)
{
	ind = 0;

	card_sock.connectToHost(card_address, card_port);
	arinc_sock.connectToHost(arinc_address, arinc_port);

	card_sock.waitForConnected(delay);
	arinc_sock.waitForConnected(delay);
}

void CCommand::send_image(const Mat & img)
{
	try
	{
		const unsigned height = img.rows, width = img.cols;
		const unsigned buf_size = height * width;
		const unsigned blocks_num_1 = buf_size / __block_size, last_block_size = buf_size % __block_size;
		unsigned v;
		shared_ptr<uint8_t> buf(new uint8_t[buf_size], std::default_delete<uint8_t[]>());
		uint8_t * ptr = buf.get();
		CQtSocket sock(arinc_sock);

		throw_null(ptr);

		sock.send
		(
			protocol::image_header
			(
				ind,
				height,
				width,
				blocks_num_1 + ((last_block_size) ? 1 : 0)
			)
		);

		for(v = 0; v < height; v++)
			memcpy(ptr + v * width, img.data + img.step[0] * v, width);

		for(v = 0; v < blocks_num_1; v++)
			sock.send
			(
				protocol::image_block
				(
					ind,
					protocol::encode_base64(ptr + v * __block_size, __block_size),
					v
				)
			);

		if(last_block_size)
			sock.send
			(
				protocol::image_block
				(
					ind,
					protocol::encode_base64(ptr + blocks_num_1 * __block_size, last_block_size),
					blocks_num_1
				)
			);

		CTree arinc_done = sock.recv();

		throw_if(arinc_done["command"].uint() != CARD_COMMAND_ARINC_DONE);
		throw_if(arinc_done["ind"].uint() != ind);
	}
	catch(...)
	{
		;
	}
}

void CCommand::send_orientation(const QMap<QString, QVariant> & metadata)
{
	try
	{
		CQtSocket sock(card_sock);

		sock.send
		(
			protocol::orientation
			(
				ind,
				metadata["x"].toDouble(),
				metadata["y"].toDouble(),
				metadata["h"].toDouble(),
				metadata["course"].toDouble(),
				metadata["roll"].toDouble(),
				metadata["pitch"].toDouble(),
				metadata["aspect_x"].toDouble(),
				metadata["aspect_y"].toDouble(),
				metadata["coord_system"].toUInt()
			)
		);
	}
	catch(...)
	{
		;
	}
}

void CCommand::correlation()
{
	try
	{
		unsigned v;
		CQtSocket sock(card_sock);

		sock.send
		(
			protocol::base
			(
				CARD_COMMAND_CORRELATION,
				ind
			)
		);

		CTree info = sock.recv(correlation_delay);

		throw_if(info["command"].uint() != CARD_COMMAND_CORRELATION_RESULT_INFO);
		throw_if(info["ind"].uint() != ind);
		throw_if(info["result"].uint() == CARD_CORRELATION_RESULT_FAIL);

		const unsigned result_num = info["result_num"].uint();

		for(v = 0; v < result_num; v++)
		{
			CTree res = sock.recv();

			throw_if(res["command"].uint() != CARD_COMMAND_CORRELATION_RESULT);
			throw_if(res["ind"].uint() != ind);

			printf_TODO("Correlation result %u of %u (%lf%%): ab = %u, cd = %u, fe = %u, dx = %u, dy = %u, num = %u\n",
				(v + 1), info["result_num"].uint(), ((v + 1) * 100.) / info["result_num"].real(),
				res["ab"].uint(), res["cd"].uint(), res["fe"].uint(), res["dx"].uint(), res["dy"].uint(), res["num"].uint());
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

