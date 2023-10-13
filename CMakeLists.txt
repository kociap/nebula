cmake_minimum_required(VERSION 3.22)
project(Nebula)

set(CMAKE_CXX_STANDARD 20)
# Change to ON/OFF depending on whether you want to use clang instead of g++
option(USE_CLANG "Use clang instead of g++" OFF)
if(USE_CLANG)
    message(STATUS "Using clang as compiler")
    set(CMAKE_CXX_COMPILER "clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Weverything -Wno-c++98-compat")
else()
    # Use g++ by default
    message(STATUS "Using g++ as compiler")
    set(CMAKE_CXX_COMPILER "g++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Wextra -pedantic")
endif()

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

set(HEADERS_DIR include)
include_directories(${OPENGL_INCLUDE_DIR} ${HEADERS_DIR})

# List the source files explicitly
set(SOURCE_FILES
        )
add_executable(${PROJECT_NAME} app/main.cpp ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} glfw)