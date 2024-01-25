#pragma once

#include <anton/math/mat4.hpp>
#include <anton/math/vec3.hpp>
#include <core/types.hpp>
#include <rendering/shader.hpp>

namespace nebula {
  struct Camera {
    math::Vec3 position;
    f32 zoom_level = 1.0f;
    /**
     * Initialize camera parameters.
     */
    void init();

    /**
     * Changes cameras location
     * @param offset - vector, offset from previous mouse x and y location
     */
    void move(math::Vec3 offset);

    /**
     * Adjusts the camera's zoom level.
     * This function modifies the camera's zoom level based on the provided zoom speed.
     * @param zl_delta Zoom level delta.
     */
    void zoom(f32 factor);
  };

  [[nodiscard]] math::Mat4 get_view_matrix(Camera const& camera);
  [[nodiscard]] math::Mat4 get_projection_matrix(Camera const& camera,
                                                 Vec2 viewport_size);
  [[nodiscard]] f32 get_zoom(Camera const& camera);

  Camera& get_primary_camera();
} // namespace nebula
