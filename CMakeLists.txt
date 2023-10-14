cmake_minimum_required(VERSION 3.22)
project(Nebula)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

set(HEADERS_DIR include)
include_directories(${OPENGL_INCLUDE_DIR} ${HEADERS_DIR})

# List the source files explicitly
set(SOURCE_FILES
        )
add_executable(${PROJECT_NAME} app/main.cpp ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} glfw)