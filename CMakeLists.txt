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

# Add GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3eaf1255b29fdf5c2895856c7be7d7185ef2b241
)
FetchContent_MakeAvailable(glfw)

# Add anton_core
FetchContent_Declare(
  anton_core
  GIT_REPOSITORY https://github.com/kociap/anton_core.git
  GIT_TAG 3a566d3d2bed07bf2f308e20fb0eb933bb52de7b
)
FetchContent_MakeAvailable(anton_core)

# Dear ImGui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG eb42e164dde71877c894b8fee4a555a738c0406b
)
FetchContent_MakeAvailable(imgui)

# ImGui has no CMakeLists. Create target manually.
set(IMGUI_SRC_DIR "${CMAKE_BINARY_DIR}/_deps/imgui-src")
add_library(dear-imgui)
set_target_properties(dear-imgui PROPERTIES CXX_STANDARD 20 CXX_EXTENSIONS OFF)
target_link_libraries(dear-imgui anton_core glad glfw)
target_include_directories(dear-imgui
  PUBLIC
  "${IMGUI_SRC_DIR}"
  "${IMGUI_SRC_DIR}/backends"
)
target_compile_options(dear-imgui PRIVATE ${NEBULA_COMPILE_FLAGS})
target_sources(dear-imgui
  PRIVATE
  "${IMGUI_SRC_DIR}/imconfig.h"
  "${IMGUI_SRC_DIR}/imgui.cpp"
  "${IMGUI_SRC_DIR}/imgui_demo.cpp"
  "${IMGUI_SRC_DIR}/imgui_draw.cpp"
  "${IMGUI_SRC_DIR}/imgui.h"
  "${IMGUI_SRC_DIR}/imgui_internal.h"
  "${IMGUI_SRC_DIR}/imgui_tables.cpp"
  "${IMGUI_SRC_DIR}/imgui_widgets.cpp"
  "${IMGUI_SRC_DIR}/imstb_rectpack.h"
  "${IMGUI_SRC_DIR}/imstb_textedit.h"
  "${IMGUI_SRC_DIR}/imstb_truetype.h"
  "${IMGUI_SRC_DIR}/backends/imgui_impl_glfw.cpp"
  "${IMGUI_SRC_DIR}/backends/imgui_impl_glfw.h"
  "${IMGUI_SRC_DIR}/backends/imgui_impl_opengl3.cpp"
  "${IMGUI_SRC_DIR}/backends/imgui_impl_opengl3.h"
)

add_executable(nebula "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
set_target_properties(nebula PROPERTIES CXX_STANDARD 20 CXX_EXTENSIONS OFF)
target_link_libraries(nebula anton_core glad glfw dear-imgui)
target_include_directories(nebula PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_compile_options(nebula PRIVATE ${NEBULA_COMPILE_FLAGS})
target_sources(nebula
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/src/components/camera.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/components/camera.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/core/error.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/core/handle.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/core/types.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/evaluator/evaluator.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/evaluator/evaluator.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/logging/logging.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/logging/logging.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/model/gate.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/model/gate.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/model/port.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/model/port.hpp"
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
  "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/compiler.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/compiler.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/scene.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/scene.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/windowing/window.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/components/camera.cpp"
)
