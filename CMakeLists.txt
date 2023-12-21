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
  # -Wnewline-eof

  # -ferror-limit=1
)

set(ANTON_BUILD_DEBUG $<CONFIG:Debug>)
set(ANTON_ITERATOR_DEBUG $<CONFIG:Debug>)
set(ANTON_OPTIONAL_CHECK_VALUE $<CONFIG:Debug>)
set(ANTON_UNREACHABLE_ASSERTS ON)

# Add GLAD
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/kociap/glad.git
  GIT_TAG b4d4f69539196fdb08a56b88fd15b7287c838b87
)
FetchContent_MakeAvailable(glad)

# Add anton_core
FetchContent_Declare(
  anton_core
  GIT_REPOSITORY https://github.com/kociap/anton_core.git
  GIT_TAG 2086ba0fba2bb8a724d7b59d0053e0ec999d38b1
)
FetchContent_MakeAvailable(anton_core)

# Add GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3eaf1255b29fdf5c2895856c7be7d7185ef2b241
)
FetchContent_MakeAvailable(glfw)

add_executable(nebula "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
set_target_properties(nebula PROPERTIES CXX_STANDARD 20 CXX_EXTENSIONS OFF)
target_link_libraries(nebula anton_core glad glfw)
target_include_directories(nebula PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_compile_options(nebula PRIVATE ${NEBULA_COMPILE_FLAGS})
target_sources(nebula
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/src/core/error.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/core/handle.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/core/types.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/logging/logging.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/logging/logging.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rendering/framebuffer.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rendering/framebuffer.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rendering/opengl.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rendering/opengl_defs.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rendering/opengl.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rendering/opengl_undefs.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rendering/rendering.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rendering/rendering.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rendering/shader.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rendering/shader.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rendering/vertex.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/movable_rect.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/node_rect.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/windowing/window.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/windowing/camera.cpp"
)
