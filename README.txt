Build (Release mode):
    1. git submodule update --init
    2. mkdir build && cd build && cmake ../ && cmake --build . --config Release && cd ..

Usage:
    1. cd build/bin/
    2. Run executable ("./Server" or "Server.exe" in dependent of platform)
    3. Open browser and type "localhost:8080"

Server is configurable by config file located in "res/config.json".

Project is just a demonstration of web communication via http, using C++ and sockets API.
