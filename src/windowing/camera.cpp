#include <anton/math/transform.hpp>
#include <windowing/camera.hpp>

namespace nebula::windowing {

  void Camera::setup_projection(Handle<rendering::Shader> shader_wire) const
  {
    rendering::set_uniform_f32(shader_wire, "zoom_level", zoom_level);
    rendering::set_uniform_vec3(shader_wire, "camera_position", position);
    rendering::set_uniform_mat4(shader_wire, "ortho_projection",
                                get_projection_mat());
  }

  void Camera::move(math::Vec3 offset)
  {
    f32 const camera_speed = 0.00001;
    position.x -= offset.x * camera_speed;
    position.y -= offset.y * camera_speed;
  }

  void Camera::init()
  {
    borders.x = -1.0f; // x-coordinate of left plane
    borders.y = 1.0f; // x-coordinate of right plane
    borders.w = -1.0f; // y-coordinate of bottom plane
    borders.z = 1.0f; // y-coordinate of top plane

    // Coordinates
    position = {0.0f, 0.0f, 0.0f};
    is_moving = false;
  }

  math::Mat4 Camera::get_projection_mat() const
  {
    // Orthographic projection matrix
    // These values can be adjusted
    // Any objects closer to the camera than the near plane will be
    // clipped and not rendered.
    f32 const near_plane = 0.000000001f;
    // Any objects farther from the camera than the far plane will be
    // clipped and not rendered.
    f32 const far_plane = 100.0f;

    return math::orthographic_rh(borders.x, borders.y, borders.w, borders.z,
                                 near_plane, far_plane);
  }

  void Camera::zoom(f32 zl_delta)
  {
    f32 new_x = borders.x + zl_delta;
    f32 new_y = borders.y - zl_delta;
    f32 new_w = borders.w + zl_delta;
    f32 new_z = borders.z - zl_delta;

    // Ensure it's not too close
    if(new_x < new_y && new_w < new_z) {
      borders.x = new_x;
      borders.y = new_y;
      borders.w = new_w;
      borders.z = new_z;
      zoom_level += zl_delta;
    }
  }

  math::Vec4 Camera::get_borders()
  {
    return borders;
  }

} // namespace nebula::windowing
