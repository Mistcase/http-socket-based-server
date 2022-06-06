#pragma once

#include <string>

class ResourceManager final
{
public:
	static void InitializeEnvironment(const std::string& executablePath);
	ResourceManager(const std::string& root = "");

	ResourceManager(const ResourceManager&) = delete;
	ResourceManager(const ResourceManager&&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&&) = delete;

	void setRoot(const std::string& root);

	std::string getFileContent(const char* file) const;
	std::string getFileContent(const std::string& file) const;

private:
	std::string getTextResource(const char* file) const;
	std::string getBinaryResource(const char* file) const;

	size_t getFileSize(std::ifstream& stream) const;

private:
	static std::string m_basePath;
	std::string m_root;
};
