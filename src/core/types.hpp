#pragma once

#include <anton/allocator.hpp>
#include <anton/array.hpp>
#include <anton/list.hpp>
#include <anton/math/vec2.hpp>
#include <anton/string.hpp>
#include <anton/types.hpp>

namespace nebula {
  using namespace anton;
  using namespace anton::types;
  using namespace anton::literals;

  template<typename T>
  using List = anton::List<T, anton::Polymorphic_Allocator>;
} // namespace nebula
