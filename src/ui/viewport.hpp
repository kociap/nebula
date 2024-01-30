#pragma once

#include <anton/math/mat4.hpp>

#include <core/types.hpp>

namespace nebula {
  [[nodiscard]] Vec2 unproject_viewport(Vec2 point, math::Mat4 inv_projection,
                                        math::Mat4 inv_view,
                                        Vec2 viewport_size);
}
