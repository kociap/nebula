#pragma once

#include <anton/expected.hpp>
#include <anton/math/mat4.hpp>
#include <anton/math/vec2.hpp>
#include <anton/math/vec3.hpp>
#include <anton/math/vec4.hpp>
#include <anton/string.hpp>
#include <anton/string_view.hpp>

#include <core/handle.hpp>
#include <core/types.hpp>

namespace nebula::rendering {
  enum struct Shader_Stage_Kind {
    vertex,
    fragment,
  };

  struct Shader_Stage;

  [[nodiscard]] Expected<Handle<Shader_Stage>, String>
  compile_shader_stage(String name, Shader_Stage_Kind type,
                       String const& source);

  struct Shader;

  [[nodiscard]] Handle<Shader> create_shader(String name);
  [[nodiscard]] bool attach_shader_stage(Handle<Shader> shader,
                                         Handle<Shader_Stage> stage);
  [[nodiscard]] Expected<void, String> link_shader(Handle<Shader> shader);

  // bind_shader
  // Binds shader for use during the following draw operations.
  //
  [[nodiscard]] bool bind_shader(Handle<Shader> const handle);

  void set_uniform_i32(Handle<Shader> handle, String_View name, i32 v);
  void set_uniform_u32(Handle<Shader> handle, String_View name, u32 v);
  void set_uniform_f32(Handle<Shader> handle, String_View name, f32 v);
  void set_uniform_vec2(Handle<Shader> handle, String_View name,
                        math::Vec2 vec);
  void set_uniform_vec3(Handle<Shader> handle, String_View name,
                        math::Vec3 vec);
  void set_uniform_vec4(Handle<Shader> handle, String_View name,
                        math::Vec4 vec);
  void set_uniform_mat4(Handle<Shader> handle, String_View name,
                        math::Mat4 const& mat);
} // namespace nebula::rendering
