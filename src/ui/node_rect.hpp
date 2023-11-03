#pragma once

#include <core/types.hpp>

namespace nebula {
  struct Node_Rect {
    /**
     * Coordinates of the bottom left corner of the rect.
     */
    math::Vec2 coordinates;
    /**
     * Dimensions of the rect. x - width, y - height
     */
    math::Vec2 dimensions;

    /**
     * Returns if rect is under given point.
     * @param point - x and y coordinates of point to check
     * @return true if rect is under given point
     */
    [[nodiscard]] bool is_under_point(math::Vec2 point) const;
  };
} // namespace nebula
