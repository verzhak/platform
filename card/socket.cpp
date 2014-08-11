
#include "socket.hpp"

CCardSocket::CCardSocket(const int sock) :
	__sock(sock)
{
}

unsigned CCardSocket::raw_recv(uint8_t * buf, const unsigned size)
{
	return ::recv(__sock, buf, size, 0);
}

unsigned CCardSocket::raw_send(const uint8_t * buf, const unsigned size)
{
	return ::send(__sock, buf, size, 0);
}

