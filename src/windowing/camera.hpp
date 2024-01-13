#pragma once

#include <anton/math/mat4.hpp>
#include <anton/math/vec3.hpp>
#include <core/types.hpp>
#include <rendering/shader.hpp>

namespace nebula::windowing {
  struct Camera {
    bool is_moving;

    /**
     * Initialize camera parameters.
     */
    void init();

    /**
     * Pass the camera position and orthographic projection to the shader.
     */
    void setup_projection(Handle<rendering::Shader> shader_wire) const;

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
    void zoom(f32 zl_delta);

    /**
     * Camera borders getter.
     */
    [[nodiscard]] math::Vec4 get_borders();

  private:
    math::Vec3 position; // Camera eye
    math::Vec4 borders;
    f32 zoom_level;

    [[nodiscard]] math::Mat4 get_projection_mat() const;
  };
} // namespace nebula::windowing
