#pragma once

#include <core/types.hpp>
#include <rendering/shader.hpp>

namespace nebula {
  [[nodiscard]] Handle<rendering::Shader> compile_shader(String const& vertex,
                                                         String const& fragment,
                                                         String const& name);
} // namespace nebula
