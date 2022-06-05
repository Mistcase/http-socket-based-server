#pragma once

#include "ResourceManager.h"
#include "TcpSocket.h"
#include <cstdint>
#include <string>

class HttpServer final
{
public:
	HttpServer(const std::string& config, const std::string contentPackage);
	bool start();
	void stop();

private:
	void handleRequest(TcpSocket& socket);

private:
	ResourceManager m_res;
	TcpSocket m_listeningSocket;

	bool m_isActive = false;
};
