#include "TcpSocket.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

TcpSocket::TcpSocket()
    : m_socket(::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))
{
    assert(m_socket != -1);
}

TcpSocket::TcpSocket(TcpSocket&& other)
{
    m_socket = other.m_socket;
    m_addr = other.m_addr;
}

void TcpSocket::shutdown()
{
    ::shutdown(m_socket, SHUT_RDWR);
}

void TcpSocket::close()
{
    ::close(m_socket);
}

TcpSocket::TcpSocket(int socket)
    : m_socket(socket)
{
}

bool TcpSocket::bind(const std::string& address, uint16_t port)
{
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(address.c_str());
    m_addr.sin_port = htons(port);

    return ::bind(m_socket, reinterpret_cast<const sockaddr*>(&m_addr),
                  sizeof(m_addr))
        == 0;
}

bool TcpSocket::listen() const
{
    return ::listen(m_socket, SOMAXCONN) == 0;
}

TcpSocket TcpSocket::accept() const
{
    const auto socket = ::accept(m_socket, nullptr, nullptr);
    assert(socket != -1);

    return TcpSocket(socket);
}

long TcpSocket::send(const void* data, size_t length) const
{
    return ::send(m_socket, data, length, 0);
}

long TcpSocket::receive(void* data, size_t length) const
{
    return ::recv(m_socket, data, length, 0);
}

TcpSocket& TcpSocket::operator=(TcpSocket&& other)
{
    new (this) TcpSocket(std::move(other));
    return *this;
}
