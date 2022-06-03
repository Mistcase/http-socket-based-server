#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <sstream>

bool IsServerAvailable = false;
uint32_t ConnectionCounter = 0u;

int main()
{
	const auto listeningSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Open TCP socket
	if (listeningSocket == -1)
	{
		std::cout << "Error: cannot create a socket\n";
		return EXIT_FAILURE;
	}

	sockaddr_in sockAddr; // Struct for opening TCP socket
    memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	sockAddr.sin_port = htons(8080);

	const auto bind = ::bind(listeningSocket, reinterpret_cast<sockaddr*>(&sockAddr), sizeof(sockAddr));
	if (bind != 0)
	{
		std::cout << "Cannot bind socket to it's address!\n";
		close(listeningSocket);
		return EXIT_FAILURE;
	}
	else
	{
		IsServerAvailable = true;
	}

	::listen(listeningSocket, SOMAXCONN);

	while (IsServerAvailable)
	{
		const auto communicationSocket = ::accept(listeningSocket, nullptr, nullptr);
		std::cout << "New connection: " << ConnectionCounter++ << std::endl;

		const int max_client_buffer_size = 1024;
		char buf[max_client_buffer_size];

		std::stringstream response; // сюда будет записываться ответ клиенту
		std::stringstream response_body; // тело ответа

		response_body << "<title>Test C++ HTTP Server</title>\n"
					  << "<h1>Test page</h1>\n"
					  << "<p>This is body of the test page...</p>\n"
					  << "<h2>Request headers</h2>\n"
					  << "<pre>" << buf << "</pre>\n"
					  << "<em><small>Test C++ Http Server</small></em>\n";

		// Формируем весь ответ вместе с заголовками
		response << "HTTP/1.1 200 OK\r\n"
				 << "Version: HTTP/1.1\r\n"
				 << "Content-Type: text/html; charset=utf-8\r\n"
				 << "Content-Length: " << response_body.str().length()
				 << "\r\n\r\n"
				 << response_body.str();

		// Отправляем ответ клиенту с помощью функции send
		const auto strResponse = response.str();
		const auto responseLength = strResponse.length();

		const auto result = ::send(communicationSocket, strResponse.c_str(), responseLength, 0);
		assert(result == responseLength);

		if (result == -1)
		{
			std::cout << "Failed to send respoonse\n";
		}

		shutdown(communicationSocket, SHUT_RDWR);
	}

	shutdown(listeningSocket, SHUT_RDWR);
	close(listeningSocket);

	return 0;
}
