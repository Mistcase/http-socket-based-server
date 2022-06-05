#pragma once

#include <string>

class ResourceManager final
{
public:
	ResourceManager(const std::string& basePath);

	ResourceManager(const ResourceManager&) = delete;
	ResourceManager(const ResourceManager&&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&&) = delete;

	std::string GetFileContent(const char* path);
	std::string GetFileContent(const std::string& path);

private:
	const std::string m_basePath;
};
