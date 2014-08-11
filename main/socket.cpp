
#include "socket.hpp"

CQtSocket::CQtSocket(QTcpSocket & sock) :
	CSocket(), __sock(sock)
{
	delay = default_delay;
}

unsigned CQtSocket::raw_recv(uint8_t * buf, const unsigned size)
{
	throw_if(__sock.state() != QAbstractSocket::ConnectedState);

	return __sock.read((char *) buf, size);
}

unsigned CQtSocket::raw_send(const uint8_t * buf, const unsigned size)
{
	throw_if(__sock.state() != QAbstractSocket::ConnectedState);

	const unsigned bytes_written = __sock.write((char *) buf, size);

	throw_if(! __sock.flush());

	return bytes_written;
}

CTree CQtSocket::recv()
{
	return CSocket::recv();
}

CTree CQtSocket::recv(unsigned __delay)
{
	delay = __delay;
	CTree ret = CSocket::recv();
	delay = default_delay;

	return ret;
}

// ############################################################################ 

string protocol::encode_base64(const uint8_t * src, const unsigned size)
{
	string dst;
	QByteArray __src = QByteArray::fromRawData((const char *) src, size);

	dst = __src.toBase64().data();

	return dst;
}

void protocol::decode_base64(const string src, shared_ptr<uint8_t> & dst, unsigned & size)
{
	uint8_t * ptr;
	QByteArray __dst = QByteArray::fromBase64(QByteArray(src.c_str()));

	size = __dst.size();
	dst.reset(new uint8_t[size], std::default_delete<uint8_t[]>());
	throw_null(ptr = dst.get());

	memcpy(ptr, __dst.data(), size);
}

