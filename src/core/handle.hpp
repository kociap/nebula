#pragma once

#include <core/types.hpp>

namespace nebula {
  template<typename T>
  struct Handle {
    u64 value = -1;

    [[nodiscard]] operator bool() const
    {
      return value != (u64)-1;
    }
  };

  template<typename T>
  [[nodiscard]] bool operator==(Handle<T> const& lhs, Handle<T> const& rhs)
  {
    return lhs.value == rhs.value;
  }

  template<typename T>
  [[nodiscard]] bool operator!=(Handle<T> const& lhs, Handle<T> const& rhs)
  {
    return lhs.value != rhs.value;
  }
} // namespace nebula
