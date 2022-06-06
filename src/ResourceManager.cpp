#include "ResourceManager.h"

#include <fstream>

namespace
{
    enum class ResourceType
    {
        Text,
        Binary,

        Count
    };

    const char* TextExtensions[] = { ".html", ".css", ".js" };

    const char* BinaryExtensions[] = { ".png" };

    ResourceType GetResourceType(const std::string& resource)
    {
        for (const auto extension : TextExtensions)
        {
            if (resource.find(extension) != std::string::npos)
            {
                return ResourceType::Text;
            }
        }

        for (const auto extension : BinaryExtensions)
        {
            if (resource.find(extension) != std::string::npos)
            {
                return ResourceType::Binary;
            }
        }

        return ResourceType::Count;
    }

} // namespace

std::string ResourceManager::m_basePath;

void ResourceManager::InitializeEnvironment(const std::string& executablePath)
{
    const auto index = executablePath.find_last_of("/\\");
    assert(index != std::string::npos);

    // Windows?
    m_basePath = executablePath.substr(0, index) + "/res/";
    assert(m_basePath.empty() == false);
}

ResourceManager::ResourceManager(const std::string& root)
{
    setRoot(root);
}

void ResourceManager::setRoot(const std::string& root)
{
    assert(m_basePath.empty() == false);
    m_root = m_basePath + root + "/";
}

std::string ResourceManager::getFileContent(const char* file) const
{
    const auto resourceType = GetResourceType(file);
    switch (resourceType)
    {
    case ResourceType::Text:
        return getTextResource(file);
        break;

    case ResourceType::Binary:
        return getBinaryResource(file);
        break;

    default:
        break;
    }

    return "";
}

std::string ResourceManager::getFileContent(const std::string& file) const
{
    return getFileContent(file.c_str());
}

std::string ResourceManager::getTextResource(const char* file) const
{
    std::ifstream stream(m_root + file);
    if (stream.is_open() == false)
    {
        return "";
    }

    std::string content;
    std::string line;
    while (stream)
    {
        std::getline(stream, line);
        content.append(line);
    }

    stream.close();
    return content;
}

std::string ResourceManager::getBinaryResource(const char* file) const
{
    std::ifstream stream(m_root + file,
                         std::ifstream::binary | std::ifstream::in);

    const auto fileSize = getFileSize(stream);

    std::string content(fileSize, 0);
    stream.read((char*)content.data(), fileSize);

    return content;
}

size_t ResourceManager::getFileSize(std::ifstream& stream) const
{
    if (stream.is_open() == false)
    {
        return 0;
    }

    stream.seekg(0, std::ios::end);

    const auto length = stream.tellg();

    stream.seekg(0, std::ios::beg);
    return length;
}
