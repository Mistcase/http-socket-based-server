cmake_minimum_required(VERSION 3.20.3 FATAL_ERROR)

project(WebServer)

set(CMAKE_CXX_STANDART 14)
set(CMAKE_CXX_STANDART_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(EXECUTABLE_NAME exec)
add_executable(${EXECUTABLE_NAME}
    src/main.cpp
    src/ResourceManager.h
    src/ResourceManager.cpp)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/res)
