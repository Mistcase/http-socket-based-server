#include "HttpServer.h"

#include "ResourceManager.h"
#include "Helpers.h"
#include <nlohmann/json.hpp>
#include <sstream>

#include <iostream>

namespace
{
	const std::string EndOfHttpRequest = "\r\n\r\n";

	const std::string RequestTypeMappings[] = {
		"GET",
		"POST"
	};

	HttpServer::RequestType GetRequestType(const std::string& request)
	{
		for (size_t i = 0; i < static_cast<size_t>(HttpServer::RequestType::Count); i++)
		{
			if (request.find(RequestTypeMappings[i]) != std::string::npos)
			{
				return static_cast<HttpServer::RequestType>(i);
			}
		}

		return HttpServer::RequestType::Count;
	}

	static_assert(helpers::GetArraySize(RequestTypeMappings) == static_cast<size_t>(HttpServer::RequestType::Count));
}

HttpServer::HttpServer(const std::string& config, const std::string& contentPackage)
{
	// Read config
	const auto configData = m_res.getFileContent(config);
	const auto json = nlohmann::json::parse(configData);

	if (json.is_object() == false)
	{
		assert(!"Json is not valid");
		return;
	}

	const auto address = json["address"].get<std::string>();
	const auto port = json["port"].get<uint16_t>();
	if (m_listeningSocket.bind(address, port) == false)
	{
		assert(!"Cannot bind socket");
	}

	m_bufferLength = json["request_buffer_size"].get<size_t>();
	allocateBuffer(m_bufferLength);

	m_res.setRoot(contentPackage);
}

bool HttpServer::start()
{
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
		handleNewConnection(socket);
	}

	return true;
}

void HttpServer::stop()
{
	m_isActive = false;
	m_listeningSocket.close();
}

void HttpServer::allocateBuffer(size_t length)
{
	if (m_buffer != nullptr)
	{
		delete m_buffer;
	}

	assert(length > 0);
	m_buffer = new char[length]{};
}

template <>
std::string HttpServer::createResponse<HttpServer::RequestType::Get>(TcpSocket& socket, const std::string& request) const
{
	// Find resource

	const std::string response_body = m_res.getFileContent("index.html");
	std::stringstream response;
	response << "HTTP/1.1 200 OK\r\n"
			 << "Version: HTTP/1.1\r\n"
			 << "Content-Type: text/html; charset=utf-8\r\n"
			 << "Content-Length: " << response_body.length()
			 << "\r\n\r\n"
			 << response_body;

	return response.str();
}

template <>
std::string HttpServer::createResponse<HttpServer::RequestType::Post>(TcpSocket& socket, const std::string& request) const
{
	assert(!"Post request is not supported");
}

void HttpServer::handleNewConnection(TcpSocket& socket)
{
	// Identify requested resource
	while (true) // !!!!!
	{
		const auto received = socket.receive(m_buffer + m_offset, m_bufferLength);
		if (received == -1)
		{
			socket.shutdown();
		}

		m_offset += received;
		std::string_view bufferView{ m_buffer };

		const auto requestLength = bufferView.find(EndOfHttpRequest);
		if (requestLength != std::string::npos)
		{
			// Handle requestLength == 0
			assert(requestLength > 0);

			const std::string request(bufferView.substr(0, requestLength));
			const auto endPatternLength = EndOfHttpRequest.length();
			const auto totalRequestLength = requestLength + endPatternLength;

			handleRequest(socket, request);

			auto middle = m_buffer + totalRequestLength;
			auto end = m_buffer + m_bufferLength;
			std::rotate(m_buffer, middle, end);
			std::fill(middle, end, 0);

			m_offset = 0;
			break;
		}
	}
}

void HttpServer::handleRequest(TcpSocket& socket, const std::string& request) const
{
	std::cout << request << std::endl << std::endl;

	// Identify request type
	const auto requestType = GetRequestType(request); // Maybe it is enough to pass the first word
	assert(requestType != RequestType::Count);

	std::string response;
	switch (requestType)
	{
	case RequestType::Get:
		response = createResponse<RequestType::Get>(socket, request);
		break;

	case RequestType::Post:
		response = createResponse<RequestType::Post>(socket, request);
		break;

	default:
		assert(!"Unsupported request type");
		break;
	}

	assert(response.empty() == false);

	socket.send(response.c_str(), response.length());
	socket.shutdown();
}
