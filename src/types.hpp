#pragma once

#include <anton/allocator.hpp>
#include <anton/array.hpp>
#include <anton/list.hpp>
#include <anton/types.hpp>

namespace nebula {
  using namespace anton::types;

  template<typename T>
  using Array = anton::Array<T, anton::Polymorphic_Allocator>;

  template<typename T>
  using List = anton::List<T, anton::Polymorphic_Allocator>;
} // namespace nebula
