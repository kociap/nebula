#include <ui/viewport.hpp>

#include <anton/math/mat4.hpp>

namespace nebula {
  Vec2 unproject_viewport(Vec2 const point, math::Mat4 const inv_projection,
                          math::Mat4 const inv_view, Vec2 const viewport_size)
  {
    Vec2 normalised = 2.0f * (point / viewport_size) - 1.0f;
    math::Vec4 const result =
      inv_view * (inv_projection * math::Vec4(normalised, 0.0f, 1.0f));
    return Vec2(result);
  }
} // namespace nebula
