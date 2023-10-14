cmake_minimum_required(VERSION 3.22)

include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

project(Nebula)


set(NEBULA_COMPILE_FLAGS
  ${CMAKE_CXX_FLAGS}
  -Wall
  -Wextra
  -Wpedantic

  -fno-char8_t
  -fno-rtti
  -fno-exceptions

  -Werror=return-type
  -Werror=uninitialized

  -Wnon-virtual-dtor
  -Wnewline-eof
)

set(ANTON_BUILD_DEBUG $<CONFIG:Debug>)
set(ANTON_ITERATOR_DEBUG $<CONFIG:Debug>)
set(ANTON_OPTIONAL_CHECK_VALUE $<CONFIG:Debug>)
set(ANTON_UNREACHABLE_ASSERTS ON)

# Add anton_core
FetchContent_Declare(
  anton_core
  GIT_REPOSITORY https://github.com/kociap/anton_core.git
  GIT_TAG b0f2d7901838ab343da8d66407c0f5c62686d4d6
)
FetchContent_MakeAvailable(anton_core)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

set(HEADERS_DIR include)

add_executable(nebula "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
set_target_properties(nebula PROPERTIES CXX_STANDARD 20 CXX_EXTENSIONS OFF)
target_link_libraries(nebula anton_core ${OPENGL_LIBRARIES} glfw)
target_include_directories(nebula PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" ${OPENGL_INCLUDE_DIR})
target_compile_options(nebula PRIVATE ${NEBULA_COMPILE_FLAGS})
target_sources(nebula
  PRIVATE

)
