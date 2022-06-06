#pragma once

#include <netinet/in.h>
#include <string>

class TcpSocket final
{
public:
    TcpSocket();
    TcpSocket(TcpSocket&& other);

    bool bind(const std::string& address, uint16_t port);
    bool listen() const;
    TcpSocket accept() const;
    long send(const void* data, size_t length) const;
    long receive(void* data, size_t length) const;

    void shutdown();
    void close();

    TcpSocket& operator=(TcpSocket&&);

    TcpSocket(const TcpSocket&) = delete;
    TcpSocket& operator=(const TcpSocket&) = delete;

private:
    TcpSocket(int socket);

private:
    sockaddr_in m_addr{};
    int m_socket;
};
