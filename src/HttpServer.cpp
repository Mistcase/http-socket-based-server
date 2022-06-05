#include "HttpServer.h"

#include "ResourceManager.h"
#include <sstream>

HttpServer::HttpServer(const std::string& config, const std::string contentPackage)
{
	// Read config
	const auto configData = m_res.getFileContent(config);
	// ...
	// ...
	// ...
	// ...

	// Set up manager to content package
	m_res.setRoot(contentPackage);
}

bool HttpServer::start()
{
	if (m_listeningSocket.bind("127.0.0.1", 8080) == false)
	{
		assert(!"Cannot bind socket");
		return false;
	}

	if (m_listeningSocket.listen() == false)
	{
		assert(!"Cannot start listening");
		return false;
	}

	// Move it to separate thread
	m_isActive = true;
	while (m_isActive)
	{
		auto socket = m_listeningSocket.accept();
		handleRequest(socket);
	}

	return true;
}

void HttpServer::stop()
{
	m_isActive = false;
	m_listeningSocket.close();
}

void HttpServer::handleRequest(TcpSocket& socket)
{
	const std::string response_body = m_res.getFileContent("index.html");

	std::stringstream response;
	response << "HTTP/1.1 200 OK\r\n"
			 << "Version: HTTP/1.1\r\n"
			 << "Content-Type: text/html; charset=utf-8\r\n"
			 << "Content-Length: " << response_body.length()
			 << "\r\n\r\n"
			 << response_body;

	const auto strResponse = response.str();
	const auto responseLength = strResponse.length();

	socket.send(strResponse.c_str(), responseLength);
	socket.shutdown();
}
