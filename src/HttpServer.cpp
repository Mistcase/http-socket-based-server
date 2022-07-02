#include "HttpServer.h"

#include "Helpers.h"
#include "ResourceManager.h"
#include <nlohmann/json.hpp>
#include <sstream>

namespace
{
    const std::string EndOfHttpRequest = "\r\n\r\n";

    const std::string RequestTypeMappings[] = {
        "GET",
        "POST"
    };

    HttpServer::RequestType GetRequestType(const std::string_view& request)
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

    namespace resources
    {
        const char* DefaultPage = "/";

        std::string GetContentType(const std::filesystem::path& resource)
        {
            static const std::unordered_map<std::string, std::string> ContentTypeMappings = {
                { ".html", "text/html" },
                { ".css", "text/css" },
                { ".js", "text/javascript" },
                { ".png", "image/png" },
            };

            const auto iterator = ContentTypeMappings.find(resource.extension());
            return iterator != ContentTypeMappings.cend()
                ? iterator->second
                : "";
        }

    } // namespace resources

} // namespace

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
    allocateBuffer();

    m_res.setRoot(contentPackage);
}

HttpServer::~HttpServer()
{
    freeBuffer();
}

bool HttpServer::start()
{
    if (m_listeningSocket.listen() == false)
    {
        assert(!"Cannot start listening");
        return false;
    }

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

void HttpServer::allocateBuffer()
{
    if (m_buffer != nullptr)
    {
        delete m_buffer;
    }

    assert(m_bufferLength > 0);
    m_buffer = new char[m_bufferLength]{};
}

void HttpServer::freeBuffer()
{
    if (m_buffer != nullptr)
    {
        delete[] m_buffer;
    }

    m_buffer = nullptr;
}

std::string HttpServer::createResponse(TcpSocket& socket, const std::string_view& request) const
{
    // Only Get requests are supported for now

    const auto& mapping = RequestTypeMappings[static_cast<size_t>(RequestType::Get)];
    assert(request.find(mapping) == 0);

    std::string resource;
    auto isIdentificationStarted = false;
    for (size_t i = mapping.length(); i < request.length(); i++)
    {
        const auto symbol = request[i];
        if (symbol == ' ')
        {
            if (isIdentificationStarted)
            {
                break;
            }

            isIdentificationStarted = true;
        }
        else
        {
            resource += symbol;
        }
    }

    if (resource == resources::DefaultPage)
    {
        resource = "index.html";
    }

    const std::string response_body = m_res.getFileContent(resource);
    std::stringstream response;
    response << "HTTP/1.1 200 OK\r\n"
             << "Version: HTTP/1.1\r\n"
             << "Content-Type: " << resources::GetContentType(resource) << "; charset=utf-8\r\n"
             << "Content-Length: " << response_body.length()
             << "\r\n\r\n"
             << response_body;

    return response.str();
}

void HttpServer::handleNewConnection(TcpSocket& socket)
{
    while (true)
    {
        const auto received = socket.receive(m_buffer + m_offset, m_bufferLength);
        if (received == -1)
        {
            socket.shutdown();
        }

        m_offset += received;
        std::string_view bufferView(m_buffer);

        const auto requestLength = bufferView.find(EndOfHttpRequest);
        if (requestLength != std::string::npos)
        {
            assert(requestLength > 0);
            if (requestLength == 0)
            {
                socket.shutdown();
                socket.close();
                return;
            }

            const auto request(bufferView.substr(0, requestLength));
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

void HttpServer::handleRequest(TcpSocket& socket, const std::string_view& request) const
{
    const auto requestType = GetRequestType(request);
    assert(requestType != RequestType::Count);

    std::string response = createResponse(socket, request);
    if (response.empty())
    {
        socket.shutdown();
        socket.close();
        return;
    }

    socket.send(response.c_str(), response.length());
    socket.shutdown();
}
