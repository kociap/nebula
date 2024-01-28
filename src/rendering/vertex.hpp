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
    math::Vec3 position; /**< Position of the vertex in 3D space. */
    math::Vec3 normal; /**< Normal vector at the vertex. */
    math::Vec2 uv; /**< Texture coordinates of the vertex. */
  };
} // namespace nebula
