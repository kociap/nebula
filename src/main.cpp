#include <anton/filesystem.hpp>
#include <glad/glad.h>

// GLAD must be included first. clang-format sorts includes, hence this comment
// here.

#include <GLFW/glfw3.h>

#include <anton/optional.hpp>

#include <core/types.hpp>
#include <logging/logging.hpp>
#include <rendering/framebuffer.hpp>
#include <rendering/rendering.hpp>
#include <rendering/shader.hpp>

using namespace nebula;

[[nodiscard]] static Optional<String> read_file(String const& path)
{
  fs::Input_File_Stream uv_vert_file(path);
  if(!uv_vert_file.is_open()) {
    return null_optional;
  }

  uv_vert_file.seek(Seek_Dir::end, 0);
  i64 const uv_vert_size = uv_vert_file.tell();
  uv_vert_file.seek(Seek_Dir::beg, 0);
  String uv_vert_source{reserve, uv_vert_size};
  uv_vert_source.force_size(uv_vert_size);
  uv_vert_file.read(uv_vert_source.data(), uv_vert_size);
  return ANTON_MOV(uv_vert_source);
}

int main(int argc, char* argv[])
{
  (void)argc;
  (void)argv;

  //
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Initialize GLFW
  if(!glfwInit()) {
    return 1;
  }

  i64 const width = 800;
  i64 const height = 600;
  // Create a GLFW window
  GLFWwindow* window =
    glfwCreateWindow(800, 600, "Hello GLFW", nullptr, nullptr);

  if(!window) {
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window);

  {
    Expected<void, Error> result = rendering::initialise(width, height);
    if(!result) {
      LOG_FATAL("rendering initialisation failed: {}", result.error());
      return 1;
    }
  }

  fs::Input_File_Stream uv_frag_file(String("shaders/uv.frag"));
  if(!uv_frag_file.is_open()) {
    LOG_FATAL("could not open 'shaders/uv.vert'");
    return 1;
  }

  Optional<String> uv_vert_source = read_file(String("shaders/uv.vert"));
  if(!uv_vert_source) {
    LOG_FATAL("could not open 'shaders/uv.vert'");
    return 1;
  }

  Optional<String> uv_frag_source = read_file(String("shaders/uv.frag"));
  if(!uv_frag_source) {
    LOG_FATAL("could not open 'shaders/uv.frag'");
    return 1;
  }

  Expected<Handle<rendering::Shader_Stage>, String> vertex_result =
    rendering::compile_shader_stage(String("uv.vert"),
                                    rendering::Shader_Stage_Kind::vertex,
                                    uv_vert_source.value());
  if(!vertex_result) {
    LOG_FATAL("compilation of 'uv.vert' failed\n{}", vertex_result.error());
    return 1;
  }
  Expected<Handle<rendering::Shader_Stage>, String> fragment_result =
    rendering::compile_shader_stage(String("uv.frag"),
                                    rendering::Shader_Stage_Kind::fragment,
                                    uv_frag_source.value());
  if(!fragment_result) {
    LOG_FATAL("compilation of 'uv.frag' failed\n{}", fragment_result.error());
    return 1;
  }

  Handle<rendering::Shader> shader_uv = rendering::create_shader(String("uv"));
  bool const vertex_attach_result =
    rendering::attach_shader_stage(shader_uv, vertex_result.value());
  if(!vertex_attach_result) {
    LOG_FATAL("vertex attach failed");
    return 1;
  }
  bool const fragment_attach_result =
    rendering::attach_shader_stage(shader_uv, fragment_result.value());
  if(!fragment_attach_result) {
    LOG_FATAL("fragment attach failed");
    return 1;
  }

  Expected<void, Error> link_result = rendering::link_shader(shader_uv);
  if(!link_result) {
    LOG_FATAL("linking of uv shader failed\n{}", link_result.error());
    return 1;
  }

  bool const bind_result = rendering::bind_shader(shader_uv);
  if(!bind_result) {
    LOG_FATAL("could not bind uv shader");
    return 1;
  }

  rendering::bind_draw_buffers();
  rendering::bind_transient_geometry_buffers();

  Vertex triangle[] = {
    Vertex{
      .position = {0.288675f, 0.25f, 0.0f}, .normal = {}, .uv = {1.0f, 1.0f}},
    Vertex{
      .position = {-0.288675f, 0.25f, 0.0f}, .normal = {}, .uv = {0.0f, 1.0f}},
    Vertex{.position = {0.0f, -0.25f, 0.0f}, .normal = {}, .uv = {1.0f, 0.0f}},
  };
  u32 indices[] = {0, 1, 2};
  rendering::Draw_Elements_Command cmd =
    rendering::write_geometry(triangle, indices);
  cmd.instance_count = 1;
  rendering::bind_default_framebuffer();
  glClearColor(0.0, 0.0, 0.0, 0.0);
  // Main loop
  while(!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    rendering::add_draw_command(cmd);
    rendering::commit_draw();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}
