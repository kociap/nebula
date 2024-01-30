#pragma once

#include <anton/math/vec2.hpp>
#include <anton/math/vec3.hpp>
#include <anton/math/vec4.hpp>

#include <core/types.hpp>

namespace nebula {
  /**
   * @brief Represents a vertex in 3D space.
   *
   * This structure holds information about a vertex, including its position,
   * normal, and texture coordinates.
   */
  struct Vertex {
    math::Vec3 position;
    math::Vec3 normal;
    math::Vec2 uv;
  };
} // namespace nebula
