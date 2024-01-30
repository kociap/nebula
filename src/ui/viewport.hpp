#pragma once

#include <anton/math/mat4.hpp>

#include <core/types.hpp>

namespace nebula {
  // unproject_viewport
  //
  // Unproject a screen space viewport relative point into the world space.
  // The projection must be an orthographic projection.
  //
  // Parameters:
  //          point - the point in screen space coordinates to unproject.
  // inv_projection - the inverse projection matrix.
  //       inv_view - the inverse view matrix.
  //  viewport_size - the size of the viewport in pixels.
  //
  [[nodiscard]] Vec2 unproject_viewport(Vec2 point, math::Mat4 inv_projection,
                                        math::Mat4 inv_view,
                                        Vec2 viewport_size);
} // namespace nebula
