#include "ResourceManager.h"

#include <fstream>

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
	std::ifstream stream(m_root + file);
	assert(stream.is_open());

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

std::string ResourceManager::getFileContent(const std::string& file) const
{
	return getFileContent(file.c_str());
}
