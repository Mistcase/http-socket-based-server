cmake_minimum_required(VERSION 3.20.3 FATAL_ERROR)

project(WebServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)

include_directories(${PROJECT_SOURCE_DIR}/json/single_include)
message(${PROJECT_SOURCE_DIR}/json/single_include)

set(EXECUTABLE_NAME exec)
add_executable(${EXECUTABLE_NAME}
    src/main.cpp
    src/ResourceManager.h
    src/ResourceManager.cpp
    src/HttpServer.h
    src/HttpServer.cpp
    src/TcpSocket.h
    src/TcpSocket.cpp)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/res)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE nlohmann_json::nlohmann_json)
