#include "HttpServer.h"
#include "ResourceManager.h"
#include <filesystem>

int main(int argc, char** argv)
{
    ResourceManager::InitializeEnvironment(argv[0]);

    HttpServer httpServer("config.json", "Content");
    httpServer.start();

    return 0;
}
