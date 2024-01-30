#pragma once

#include <core/types.hpp>
#include <rendering/shader.hpp>

namespace nebula {
  /**
   * @brief Compiles a shader program from vertex and fragment shader source
   * code.
   *
   * This function takes vertex and fragment shader source code as input,
   * compiles them into a shader program, and returns a handle to the compiled
   * shader.
   *
   * @param vertex The source code of the vertex shader.
   * @param fragment The source code of the fragment shader.
   * @param name The name of the shader for identification purposes.
   * @return A handle to the compiled shader if compilation is successful.
   */
  [[nodiscard]] Handle<rendering::Shader> compile_shader(String const& vertex,
                                                         String const& fragment,
                                                         String const& name);
} // namespace nebula
