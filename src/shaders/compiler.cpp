#include <shaders/compiler.hpp>

#include <anton/filesystem.hpp>
#include <anton/optional.hpp>

#include <logging/logging.hpp>

namespace nebula {
  /**
   * @brief Reads the content of a file and returns it as a string.
   *
   * This function opens the file specified by the given path, reads its content, and returns
   * the content as a string. If the file cannot be opened or an error occurs during the reading
   * process, an empty optional string is returned.
   *
   * @param path The path to the file to be read.
   * @return An optional string containing the content of the file, or an empty optional if
   *         the file cannot be opened or an error occurs.
   */
  [[nodiscard]] static Optional<String> read_file(String const& path)
  {
    fs::Input_File_Stream file(path);
    if(!file.is_open()) {
      return null_optional;
    }

    file.seek(Seek_Dir::end, 0);
    i64 const uv_vert_size = file.tell();
    file.seek(Seek_Dir::beg, 0);
    String uv_vert_source{reserve, uv_vert_size};
    uv_vert_source.force_size(uv_vert_size);
    file.read(uv_vert_source.data(), uv_vert_size);
    return ANTON_MOV(uv_vert_source);
  }

  Handle<rendering::Shader> compile_shader(String const& vertex,
                                           String const& fragment,
                                           String const& name)
  {
    LOG_INFO("compiling shader '{}'", vertex);
    Optional<String> vert_source = read_file(vertex);
    if(!vert_source) {
      LOG_ERROR("could not open '{}'", vertex);
      return {};
    }

    Expected<Handle<rendering::Shader_Stage>, String> vertex_result =
      rendering::compile_shader_stage(
        vertex, rendering::Shader_Stage_Kind::vertex, vert_source.value());
    if(!vertex_result) {
      LOG_ERROR("compilation of '{}' failed\n{}", vertex,
                vertex_result.error());
      return {};
    }

    LOG_INFO("compiling shader '{}'", fragment);
    Optional<String> frag_source = read_file(fragment);
    if(!frag_source) {
      LOG_ERROR("could not open '{}'", fragment);
      return {};
    }

    Expected<Handle<rendering::Shader_Stage>, String> fragment_result =
      rendering::compile_shader_stage(
        fragment, rendering::Shader_Stage_Kind::fragment, frag_source.value());
    if(!fragment_result) {
      LOG_ERROR("compilation of '{}' failed\n{}", fragment,
                fragment_result.error());
      return {};
    }

    Handle<rendering::Shader> shader = rendering::create_shader(name);
    bool const vertex_attach_result =
      rendering::attach_shader_stage(shader, vertex_result.value());
    if(!vertex_attach_result) {
      LOG_ERROR("attach of '{}' to '{}' failed", vertex, name);
      return {};
    }
    bool const fragment_attach_result =
      rendering::attach_shader_stage(shader, fragment_result.value());
    if(!fragment_attach_result) {
      LOG_ERROR("attach of '{}' to '{}' failed", fragment, name);
      return {};
    }

    Expected<void, Error> link_result = rendering::link_shader(shader);
    if(!link_result) {
      LOG_ERROR("linking of '{}' failed\n{}", name, link_result.error());
      return {};
    }

    return shader;
  }
} // namespace nebula