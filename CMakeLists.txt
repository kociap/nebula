cmake_minimum_required(VERSION 3.22)
project(NEBULA)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} app/main.cpp ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE include)