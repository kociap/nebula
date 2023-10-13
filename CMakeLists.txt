cmake_minimum_required(VERSION 3.22)
project(NEBULA)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -lGL")

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR} include)

file(GLOB SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} app/main.cpp ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} glfw)