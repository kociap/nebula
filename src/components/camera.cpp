#include <components/camera.hpp>

#include <anton/math/transform.hpp>

namespace nebula {
  void Camera::move(Vec2 offset)
  {
    f32 const horizontal_speed = 0.6f;
    f32 const vertical_speed = 0.6f;
    position.x -= offset.x * horizontal_speed;
    position.y -= offset.y * vertical_speed;
  }

  void Camera::zoom(f32 const factor)
  {
    zoom_level = math::clamp(zoom_level * factor, 1.0f, 35184372088832.0f);
  }

  math::Mat4 get_view_matrix(Camera const& camera)
  {
    return math::translate(-(camera.position));
  }

  math::Mat4 get_projection_matrix(Camera const& camera,
                                   Vec2 const viewport_size)
  {
    f32 const near_plane = 0.0f;
    f32 const far_plane = 100.0f;
    f32 const zoom_level = camera.zoom_level;

    Vec2 const extents =
      zoom_level * Vec2(viewport_size.x / viewport_size.y, 1.0f);

    return math::orthographic_rh(-extents.x, extents.x, -extents.y, extents.y,
                                 near_plane, far_plane);
  }

  f32 get_zoom(Camera const& camera)
  {
    return camera.zoom_level;
  }

  static Camera primary_camera;

  Camera& get_primary_camera()
  {
    return primary_camera;
  }
} // namespace nebula
