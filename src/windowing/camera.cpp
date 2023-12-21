#include <windowing/camera.hpp>

namespace nebula::windowing {

  void Camera::setup_projection(Handle<rendering::Shader> shader_wire) const
  {
    rendering::set_uniform_vec3(shader_wire, "camera_position", location);
    rendering::set_uniform_mat4(shader_wire, "ortho_projection",
                                ortho_proj_matrix);
  }

  void Camera::move(math::Vec3 offset)
  {
    f32 const camera_speed = 0.00001;
    location.x += offset.x * camera_speed;
    location.y += offset.y * camera_speed;
  }

  void Camera::init()
  {
    // Orthographic projection matrix
    // This values can be adjusted
    f32 const left = -1.0f; // x-coordinate of left plane
    f32 const right = 1.0f; // x-coordinate of right plane
    f32 const bottom = -1.0f; // y-coordinate of bottom plane
    f32 const top = 1.0f; // y-coordinate of top plane
    // Any objects closer to the camera than the near plane will be
    // clipped and not rendered.
    f32 const nearPlane = 0.000000001f;
    // Any objects farther from the camera than the far plane will be
    // clipped and not rendered.
    f32 const farPlane = 100.0f;

    ortho_proj_matrix = {
      {2.0f / (right - left), 0.0f, 0.0f, 0.0f},
      {0.0f, 2.0f / (top - bottom), 0.0f, 0.0f},
      {0.0f, 0.0f, -2.0f / (farPlane - nearPlane), 0.0f},
      {-(right + left) / (right - left), -(top + bottom) / (top - bottom),
       -(farPlane + nearPlane) / (farPlane - nearPlane), 1.0f}};

    // Coordinates
    location = {0.0f, 0.0f, 0.0f};
    is_moving = false;
  }

} // namespace nebula::windowing
