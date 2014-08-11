
#include "socket.hpp"

CSocket::CSocket()
{
	buf = "";
}

CTree CSocket::recv()
{
	const unsigned __buf_size = 1024;
	unsigned recv_size;
	char __buf[__buf_size];
	string data;

	while(buf.find(CTree::end_tag) == string::npos)
	{
		recv_size = raw_recv((uint8_t *) __buf, __buf_size - 1);
		__buf[recv_size] = '\0';
		buf += __buf;
	}

	size_t from = buf.find(CTree::begin_tag);
	size_t to = buf.find(CTree::end_tag, from + 1);

	throw_if(from == string::npos);
	throw_if(to == string::npos);
	throw_if(from >= to);

	to += CTree::end_tag.size();
	data = buf.substr(from, to - from);
	buf = buf.substr(to, buf.size() - to);

	return CTree::load_from_string(data.c_str());
}

void CSocket::send(const CTree & packet)
{
	const string data = packet.pack();
	const unsigned size = data.length();

	throw_if(raw_send((uint8_t *) data.c_str(), size) != size);
}

