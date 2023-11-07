#pragma once

#include <types.hpp>

namespace nebula {
  struct Node_Rect {
    /**
     * Coordinates of the bottom left corner of the rect.
     */
    Vec2 coordinates;
    /**
     * Dimensions of the rect. x - width, y - height
     */
    Vec2 dimensions;

    /**
     * Returns if rect is under given point.
     * @param point - x and y coordinates of point to check
     * @return true if rect is under given point
     */
    [[nodiscard]] bool is_under_point(Vec2 point) const;
  };
} // namespace nebula
