#include "ResourceManager.h"

#include <fstream>

ResourceManager::ResourceManager(const std::string& basePath)
	: m_basePath(basePath)
{
	assert(m_basePath.empty() == false);
}

std::string ResourceManager::GetFileContent(const char* path)
{
	std::ifstream stream(path);
	std::string buffer;
	std::string line;

	while(stream)
	{
		std::getline(stream, line);
		buffer.append(line);
	}

	stream.close();
	return buffer;
}

std::string ResourceManager::GetFileContent(const std::string& path)
{
	return GetFileContent(path.c_str());
}
