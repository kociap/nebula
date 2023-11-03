#pragma once

#include <anton/math/vec2.hpp>
#include <anton/math/vec3.hpp>
#include <anton/math/vec4.hpp>

#include <core/types.hpp>

namespace nebula {
  struct Vertex {
    math::Vec3 position;
    math::Vec3 normal;
    math::Vec2 uv;
  };
} // namespace nebula
