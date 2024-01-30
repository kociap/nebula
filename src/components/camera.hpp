#pragma once

#include <anton/math/mat4.hpp>
#include <core/types.hpp>
#include <rendering/shader.hpp>

/**
 * @brief Represents a camera in a 3D scene.
 *
 * The Camera structure encapsulates the properties and behavior of a camera,
 * including its position in 3D space and functions to manipulate its position
 * and zoom level.
 */
namespace nebula {
  struct Camera {
    math::Vec3 position; /**< The position of the camera in 3D space. */
    f32 zoom_level = 3.0f; /**< The zoom level of the camera. Default is 3.0. */

    /**
     * @brief Changes the camera's location based on the provided offset.
     *
     * This function adjusts the camera's position based on the given offset,
     * derived from the difference between the current and previous
     * mouse coordinates.
     *
     * @param offset - Vector representing the offset from the previous mouse
     *                x and y location.
     */
    void move(Vec2 offset);

    /**
     * @brief Adjusts the camera's zoom level.
     *
     * This function modifies the camera's zoom level based on the provided zoom speed.
     *
     * @param factor - Zoom level delta. A positive value zooms in, and a negative
     *                value zooms out. The actual magnitude of the factor
     * determines the zoom speed.
     */
    void zoom(f32 factor);

    /**
     * @brief Returns the coordinates in the scene located at the clicked location in the window.
     *
     * This function maps the given window position to scene coordinates based on the
     * window size and viewport size.
     *
     * @param win_position - Position on the screen where the click occurred.
     * @param viewport_size - Size of the viewport within the window.
     * @param framebuffer_size - Size of the framebuffer
     *
     * @return Scene coordinates corresponding to the clicked location in the window.
     */
    [[nodiscard]] Vec2 window_to_scene_position(Vec2 win_position,
                                                Vec2 viewport_size,
                                                Vec2 framebuffer_size) const;
  };

  /**
   * @brief Gets the view matrix for a given camera.
   *
   * This function calculates and returns the view matrix for the specified camera.
   * The view matrix is used in rendering to transform world coordinates
   * to camera-relative coordinates.
   *
   * @param camera - Reference to the camera for which the view matrix is calculated.
   *
   * @return The view matrix representing the transformation from world to camera space.
   */
  [[nodiscard]] math::Mat4 get_view_matrix(const Camera& camera);

  /**
   * @brief Gets the projection matrix for a given camera and viewport size.
   *
   * This function calculates and returns the projection matrix for the specified
   * camera and viewport size. The projection matrix is used in rendering
   * to define the perspective or orthographic projection of the scene.
   *
   * @param camera - Reference to the camera for which the projection matrix is calculated.
   * @param viewport_size - Size of the viewport in which the scene is being rendered.
   *
   * @return The projection matrix representing the transformation from camera space
   *         to normalized device coordinates (NDC).
   */
  [[nodiscard]] math::Mat4 get_projection_matrix(const Camera& camera,
                                                 Vec2 viewport_size);

  /**
   * @brief Gets the zoom level of a given camera.
   *
   * This function retrieves and returns the zoom level of the specified camera.
   * The zoom level indicates the scale factor applied to the view, affecting the
   * field of view or size of objects in the rendered scene.
   *
   * @param camera - Reference to the camera for which the zoom level is retrieved.
   *
   * @return The zoom level of the camera.
   */
  [[nodiscard]] f32 get_zoom(const Camera& camera);

  /**
   * @brief Gets a reference to the primary camera.
   *
   * This function returns a reference to the primary camera, allowing direct
   * access and modification of its properties.
   *
   * @return Reference to the primary camera.
   */
  Camera& get_primary_camera();
} // namespace nebula
