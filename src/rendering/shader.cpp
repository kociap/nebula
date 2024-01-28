#include <rendering/shader.hpp>

#include <anton/algorithm.hpp>
#include <anton/array.hpp>
#include <anton/assert.hpp>
#include <anton/flat_hash_map.hpp>
#include <anton/format.hpp>
#include <anton/string.hpp>

#include <glad/glad.h>

namespace nebula::rendering {
  /**
   * @brief Represents a shader stage, such as vertex, fragment, or geometry shader.
   */
  struct Shader_Stage {
    anton::String name; /**< Name of the shader stage. */
    Handle<Shader_Stage>
      handle; /**< Handle to uniquely identify the shader stage. */
    u32 gl_handle = 0; /**< OpenGL handle for the shader stage. */
  };

  /**
   * @brief Represents a shader program consisting of multiple shader stages.
   */
  struct Shader {
    anton::Flat_Hash_Map<u64, i32>
      uniform_cache; /**< Cache for storing uniform locations. */
    anton::String name; /**< Name of the shader program. */
    Handle<Shader>
      handle; /**< Handle to uniquely identify the shader program. */
    u32 gl_handle = 0; /**< OpenGL handle for the shader program. */
  };

  /**
   * @brief Builds a uniform cache for a Shader.
   *
   * This function builds a uniform cache for the specified Shader by querying active
   * uniforms and their locations from the OpenGL program. The information is stored in
   * the Shader's uniform cache, associating uniform names with their corresponding locations.
   *
   * @param shader - Pointer to the Shader for which the uniform cache is to be built.
   */
  static void build_uniform_cache(Shader* shader)
  {
    i32 active_uniforms;
    glGetProgramInterfaceiv(shader->gl_handle, GL_UNIFORM, GL_ACTIVE_RESOURCES,
                            &active_uniforms);
    // glGetProgramInterfaceiv outputs the max name length including
    // null-terminator.
    i32 uniform_max_name_length;
    glGetProgramInterfaceiv(shader->gl_handle, GL_UNIFORM, GL_MAX_NAME_LENGTH,
                            &uniform_max_name_length);
    Array<GLchar> name(uniform_max_name_length);
    for(i32 uniform_index = 0; uniform_index < active_uniforms;
        ++uniform_index) {
      // glGetActiveUniformName outputs the number of characters written not
      // including null-terminator.
      i32 name_length;
      glGetActiveUniformName(shader->gl_handle, static_cast<u32>(uniform_index),
                             uniform_max_name_length, &name_length, &name[0]);
      i32 const location = glGetUniformLocation(shader->gl_handle, &name[0]);
      anton::String_View const name_str(&name[0], name_length);
      shader->uniform_cache.emplace(anton::hash(name_str), location);
    }
  }

  static Array<Shader_Stage>* shader_stages;
  static u64 shader_stage_handle_index_counter = 0;
  static Array<Shader>* shaders;
  static u64 shader_handle_index_counter = 0;

  anton::Expected<void, Error> initialise_shaders()
  {
    shader_stages = new_obj<Array<Shader_Stage>>();
    shaders = new_obj<Array<Shader>>();
    return anton::expected_value;
  }

  void teardown_shaders()
  {
    delete_obj(shaders);
    delete_obj(shader_stages);
  }

  /**
   * @brief Finds a Shader_Stage by its handle.
   *
   * This function searches for a Shader_Stage with the specified handle in the global collection
   * of shader stages. If found, a pointer to the Shader_Stage is returned; otherwise, nullptr is returned.
   *
   * @param handle - Handle of the Shader_Stage to find.
   * @return Pointer to the found Shader_Stage, or nullptr if not found.
   */
  [[nodiscard]] static Shader_Stage*
  find_shader_stage(Handle<Shader_Stage> const handle)
  {
    auto r = anton::find_if(
      shader_stages->begin(), shader_stages->end(),
      [handle](Shader_Stage const& stage) { return stage.handle == handle; });
    if(r != shader_stages->end()) {
      return &*r;
    } else {
      return nullptr;
    }
  }

  /**
   * @brief Finds a Shader by its handle.
   *
   * This function searches for a Shader with the specified handle in the global collection
   * of shaders. If found, a pointer to the Shader is returned; otherwise, nullptr is returned.
   *
   * @param handle - Handle of the Shader to find.
   * @return Pointer to the found Shader, or nullptr if not found.
   */
  [[nodiscard]] static Shader* find_shader(Handle<Shader> const handle)
  {
    auto r = anton::find_if(
      shaders->begin(), shaders->end(),
      [handle](Shader const& shader) { return shader.handle == handle; });
    if(r != shaders->end()) {
      return &*r;
    } else {
      return nullptr;
    }
  }

  anton::Expected<Handle<Shader_Stage>, anton::String>
  compile_shader_stage(anton::String name, Shader_Stage_Kind type,
                       anton::String const& source)
  {
    u32 gl_handle = 0;
    switch(type) {
    case Shader_Stage_Kind::vertex: {
      gl_handle = glCreateShader(GL_VERTEX_SHADER);
    } break;
    case Shader_Stage_Kind::fragment: {
      gl_handle = glCreateShader(GL_FRAGMENT_SHADER);
    } break;
    }

    char const* src = source.data();
    glShaderSource(gl_handle, 1, &src, nullptr);

    // Compile shader stage.
    glCompileShader(gl_handle);
    GLint compilation_status;
    glGetShaderiv(gl_handle, GL_COMPILE_STATUS, &compilation_status);
    // Compilation failed.
    if(compilation_status == GL_FALSE) {
      GLint log_size;
      glGetShaderiv(gl_handle, GL_INFO_LOG_LENGTH, &log_size);
      anton::String log{anton::reserve, log_size};
      glGetShaderInfoLog(gl_handle, log_size, &log_size,
                         log.data() + log.size_bytes());
      log.force_size(log_size);
      return {anton::expected_error, anton::format("{}: {}"_sv, name, log)};
    }

    Handle<Shader_Stage> handle{shader_stage_handle_index_counter++};
    shader_stages->push_back(Shader_Stage{ANTON_MOV(name), handle, gl_handle});
    return {anton::expected_value, handle};
  }

  Handle<Shader> create_shader(anton::String name)
  {
    u32 const gl_handle = glCreateProgram();
    Handle<Shader> handle{shader_handle_index_counter++};
    shaders->push_back(Shader({}, ANTON_MOV(name), handle, gl_handle));
    return handle;
  }

  bool attach_shader_stage(Handle<Shader> const shader_handle,
                           Handle<Shader_Stage> const stage_handle)
  {
    Shader* const shader = find_shader(shader_handle);
    if(shader == nullptr) {
      return false;
    }

    Shader_Stage* const stage = find_shader_stage(stage_handle);
    if(stage == nullptr) {
      return false;
    }

    glAttachShader(shader->gl_handle, stage->gl_handle);
    return true;
  }

  anton::Expected<void, anton::String> link_shader(Handle<Shader> const handle)
  {
    Shader* const shader = find_shader(handle);
    if(shader == nullptr) {
      return {anton::expected_error,
              anton::format("invalid shader handle {}"_sv, handle.value)};
    }

    glLinkProgram(shader->gl_handle);
    GLint link_status;
    glGetProgramiv(shader->gl_handle, GL_LINK_STATUS, &link_status);
    // Shader creation failed.
    if(link_status == GL_FALSE) {
      GLint log_length;
      glGetProgramiv(shader->gl_handle, GL_INFO_LOG_LENGTH, &log_length);
      anton::String log{anton::reserve, log_length};
      glGetProgramInfoLog(shader->gl_handle, log_length, &log_length,
                          log.data() + log.size_bytes());
      log.force_size(log_length);
      return {anton::expected_error,
              anton::format("{}: {}"_sv, shader->name, log)};
    }

    build_uniform_cache(shader);

    return anton::expected_value;
  }

  bool bind_shader(Handle<Shader> const handle)
  {
    Shader* const shader = find_shader(handle);
    if(shader == nullptr) {
      return false;
    }

    glUseProgram(shader->gl_handle);
    return true;
  }

  void set_uniform_i32(Handle<Shader> const handle,
                       anton::String_View const name, i32 const v)
  {
    Shader* const shader = find_shader(handle);
    if(shader == nullptr) {
      return;
    }
    u64 const h = anton::hash(name);
    auto iter = shader->uniform_cache.find(h);
    if(iter != shader->uniform_cache.end()) {
      glUniform1i(iter->value, v);
    }
  }

  void set_uniform_u32(Handle<Shader> const handle,
                       anton::String_View const name, u32 const v)
  {
    Shader* const shader = find_shader(handle);
    if(shader == nullptr) {
      return;
    }
    u64 const h = anton::hash(name);
    auto iter = shader->uniform_cache.find(h);
    if(iter != shader->uniform_cache.end()) {
      glUniform1ui(iter->value, v);
    }
  }

  void set_uniform_f32(Handle<Shader> const handle,
                       anton::String_View const name, f32 const v)
  {
    Shader* const shader = find_shader(handle);
    if(shader == nullptr) {
      return;
    }
    u64 const h = anton::hash(name);
    auto iter = shader->uniform_cache.find(h);
    if(iter != shader->uniform_cache.end()) {
      glUniform1f(iter->value, v);
    }
  }

  void set_uniform_vec2(Handle<Shader> const handle,
                        anton::String_View const name, math::Vec2 const vec)
  {
    Shader* const shader = find_shader(handle);
    if(shader == nullptr) {
      return;
    }
    u64 const h = anton::hash(name);
    auto iter = shader->uniform_cache.find(h);
    if(iter != shader->uniform_cache.end()) {
      glUniform2fv(iter->value, 1, &vec.x);
    }
  }

  void set_uniform_vec3(Handle<Shader> const handle,
                        anton::String_View const name, math::Vec3 const vec)
  {
    Shader* const shader = find_shader(handle);
    if(shader == nullptr) {
      return;
    }

    u64 const h = anton::hash(name);
    auto iter = shader->uniform_cache.find(h);
    if(iter != shader->uniform_cache.end()) {
      glUniform3fv(iter->value, 1, &vec.x);
    }
  }

  void set_uniform_vec4(Handle<Shader> const handle,
                        anton::String_View const name, math::Vec4 const vec)
  {
    Shader* const shader = find_shader(handle);
    if(shader == nullptr) {
      return;
    }

    u64 const h = anton::hash(name);
    auto iter = shader->uniform_cache.find(h);
    if(iter != shader->uniform_cache.end()) {
      glUniform4fv(iter->value, 1, &vec.x);
    }
  }

  void set_uniform_mat4(Handle<Shader> const handle,
                        anton::String_View const name, math::Mat4 const& mat)
  {
    Shader* const shader = find_shader(handle);
    if(shader == nullptr) {
      return;
    }

    u64 const h = anton::hash(name);
    auto iter = shader->uniform_cache.find(h);
    if(iter != shader->uniform_cache.end()) {
      glUniformMatrix4fv(iter->value, 1, GL_FALSE, mat.data());
    }
  }
} // namespace nebula::rendering
