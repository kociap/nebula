#pragma once

#include <anton/allocator.hpp>
#include <anton/array.hpp>
#include <anton/list.hpp>
#include <anton/math/vec2.hpp>
#include <anton/types.hpp>

namespace nebula {
  using namespace anton::types;

  using Vec2 = anton::math::Vec2;

  template<typename T>
  using Array = anton::Array<T>;

  template<typename T>
  using List = anton::List<T, anton::Polymorphic_Allocator>;
} // namespace nebula
