#pragma once

#include <anton/expected.hpp>
#include <anton/math/mat4.hpp>
#include <anton/math/vec2.hpp>
#include <anton/math/vec3.hpp>
#include <anton/math/vec4.hpp>
#include <anton/string.hpp>
#include <anton/string_view.hpp>

#include <core/error.hpp>
#include <core/handle.hpp>
#include <core/types.hpp>

namespace nebula::rendering {
  /**
   * @brief Enumeration representing the kinds of shader stages.
   */
  enum struct Shader_Stage_Kind {
    vertex,
    fragment,
  };

  /**
   * @brief Represents a shader program consisting of multiple shader stages.
   */
  struct Shader;
  /**
   * @brief Represents an individual shader stage, such as vertex or fragment.
   */
  struct Shader_Stage;

  /**
   * @brief Initializes the shader subsystem.
   *
   * This function initializes the shader subsystem, preparing it for shader compilation
   * and usage.
   *
   * @return Expected<void, Error> indicating success or failure.
   */
  anton::Expected<void, Error> initialise_shaders();
  /**
   * @brief Tears down the shader subsystem.
   *
   * This function releases resources and cleans up the shader subsystem.
   */
  void teardown_shaders();

  /**
   * @brief Compiles a shader stage from source code.
   *
   * This function compiles a shader stage with the given name, type, and source
   * code.
   *
   * @param name - Name of the shader stage.
   * @param type - Type of the shader stage (e.g., vertex or fragment).
   * @param source - Source code of the shader stage.
   * @return Expected<Handle<Shader_Stage>, String> containing the handle to the
   *         compiled shader stage on success, or an error message on failure.
   */
  [[nodiscard]] Expected<Handle<Shader_Stage>, String>
  compile_shader_stage(String name, Shader_Stage_Kind type,
                       String const& source);
  /**
   * @brief Creates a shader program.
   *
   * This function creates a shader program with the given name.
   *
   * @param name - Name of the shader program.
   * @return Handle<Shader> representing the handle to the created shader
   * program.
   */
  [[nodiscard]] Handle<Shader> create_shader(String name);
  /**
   * @brief Attaches a shader stage to a shader program.
   *
   * This function attaches a shader stage to a shader program.
   *
   * @param shader - Handle to the shader program.
   * @param stage - Handle to the shader stage.
   * @return True if the attachment is successful, false otherwise.
   */
  [[nodiscard]] bool attach_shader_stage(Handle<Shader> shader,
                                         Handle<Shader_Stage> stage);
  /**
   * @brief Links a shader program.
   *
   * This function links a shader program, finalizing the compilation process.
   *
   * @param shader - Handle to the shader program.
   * @return Expected<void, String> containing an error message on failure or
   * success otherwise.
   */
  [[nodiscard]] Expected<void, String> link_shader(Handle<Shader> shader);

  /**
   * @brief Binds a shader for use during draw operations.
   *
   * This function binds the specified shader for use during subsequent draw
   * operations.
   *
   * @param handle - Handle to the shader program.
   * @return True if the binding is successful, false otherwise.
   */
  [[nodiscard]] bool bind_shader(Handle<Shader> handle);

  /**
   * @brief Sets an integer uniform in the shader.
   *
   * This function sets the value of an integer uniform in the specified shader
   * program.
   *
   * @param handle - Handle to the shader program.
   * @param name - Name of the uniform.
   * @param v - Integer value to set.
   */
  void set_uniform_i32(Handle<Shader> handle, String_View name, i32 v);
  /**
   * @brief Sets an unsigned integer uniform in the shader.
   *
   * This function sets the value of an unsigned integer uniform in the
   * specified shader program.
   *
   * @param handle - Handle to the shader program.
   * @param name - Name of the uniform.
   * @param v - Unsigned integer value to set.
   */
  void set_uniform_u32(Handle<Shader> handle, String_View name, u32 v);
  /**
   * @brief Sets a floating-point uniform in the shader.
   *
   * This function sets the value of a floating-point uniform in the specified
   * shader program.
   *
   * @param handle - Handle to the shader program.
   * @param name - Name of the uniform.
   * @param v - Floating-point value to set.
   */
  void set_uniform_f32(Handle<Shader> handle, String_View name, f32 v);
  /**
   * @brief Sets a 2D vector uniform in the shader.
   *
   * This function sets the value of a 2D vector uniform in the specified shader
   * program.
   *
   * @param handle - Handle to the shader program.
   * @param name - Name of the uniform.
   * @param vec - 2D vector to set.
   */
  void set_uniform_vec2(Handle<Shader> handle, String_View name,
                        math::Vec2 vec);
  /**
   * @brief Sets a 3D vector uniform in the shader.
   *
   * This function sets the value of a 3D vector uniform in the specified shader
   * program.
   *
   * @param handle - Handle to the shader program.
   * @param name - Name of the uniform.
   * @param vec - 3D vector to set.
   */
  void set_uniform_vec3(Handle<Shader> handle, String_View name,
                        math::Vec3 vec);
  /**
   * @brief Sets a 4D vector uniform in the shader.
   *
   * This function sets the value of a 4D vector uniform in the specified shader
   * program.
   *
   * @param handle - Handle to the shader program.
   * @param name - Name of the uniform.
   * @param vec - 4D vector to set.
   */
  void set_uniform_vec4(Handle<Shader> handle, String_View name,
                        math::Vec4 vec);
  /**
   * @brief Sets a 4x4 matrix uniform in the shader.
   *
   * This function sets the value of a 4x4 matrix uniform in the specified
   * shader program.
   *
   * @param handle - Handle to the shader program.
   * @param name - Name of the uniform.
   * @param mat - 4x4 matrix to set.
   */
  void set_uniform_mat4(Handle<Shader> handle, String_View name,
                        math::Mat4 const& mat);
} // namespace nebula::rendering
