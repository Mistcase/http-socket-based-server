#pragma once

#include "ResourceManager.h"
#include "TcpSocket.h"
#include <cstdint>

class HttpServer final
{
public:
    enum class RequestType
    {
        Get,
        Post,

        Count
    };

public:
    HttpServer(const std::string& config, const std::string& contentPackage);
    bool start();
    void stop();

private:
    void allocateBuffer();

    template <RequestType T>
    std::string createResponse(TcpSocket& socket,
                               const std::string& request) const;

    void handleNewConnection(TcpSocket& socket);
    void handleRequest(TcpSocket& socket, const std::string& request) const;

private:
    ResourceManager m_res;
    TcpSocket m_listeningSocket;

    char* m_buffer = nullptr;

    size_t m_bufferLength;
    size_t m_offset = 0;
    bool m_isActive = false;
};
