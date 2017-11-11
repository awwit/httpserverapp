 
#include "ClientProtocol.h"

namespace HttpClient
{
	ClientProtocol::ClientProtocol(Socket::Adapter *sock) : sock(sock) {}

	Socket::Adapter *ClientProtocol::getSocket() noexcept {
		return this->sock;
	}

	void ClientProtocol::close() noexcept {
		this->sock->close();
	}
}
