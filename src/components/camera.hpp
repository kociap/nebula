#pragma once

#include <anton/math/mat4.hpp>
#include <core/types.hpp>
#include <rendering/shader.hpp>

namespace nebula {
  struct Camera {
    math::Vec3 position;
    f32 zoom_level = 1.0f;

    /**
     * Changes cameras location
     * @param offset - vector, offset from previous mouse x and y location
     */
    void move(Vec2 offset);

    /**
     * Adjusts the camera's zoom level.
     * This function modifies the camera's zoom level based on the provided zoom speed.
     * @param factor Zoom level delta.
     */
    void zoom(f32 factor);

    /**
     * Returns the coordinates in the scene located at the clicked
     * location in the window.
     * @param win_position - position on the screen
     * @param win_size - window width and height
     * @return scene coordinates
     */
    [[nodiscard]] Vec2 window_to_scene_position(Vec2 win_position,
                                                Vec2 win_size,
                                                Vec2 viewport_size) const;
  };

  [[nodiscard]] math::Mat4 get_view_matrix(Camera const& camera);
  [[nodiscard]] math::Mat4 get_projection_matrix(Camera const& camera,
                                                 Vec2 viewport_size);
  [[nodiscard]] f32 get_zoom(Camera const& camera);

  Camera& get_primary_camera();
} // namespace nebula
