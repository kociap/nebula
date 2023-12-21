#pragma once

#include <anton/math/mat4.hpp>
#include <anton/math/vec3.hpp>
#include <core/types.hpp>
#include <rendering/shader.hpp>

namespace nebula::windowing {
  struct Camera {
    math::Vec3 location; // Camera location
    math::Mat4 ortho_proj_matrix;
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
  };
} // namespace nebula::windowing
