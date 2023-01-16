Build (Release mode):
    1. git submodule update --init
    2. mkdir build && cd build && cmake ../ && cmake --build . --config Release && cd ..

Usage:
    1. Run executable ("./build/bin/Server")
    2. Open browser and type "localhost:8080"

Server is configurable by config file located in "res/config.json".

Project is just a demonstration of web communication via http, using C++ and sockets API.
