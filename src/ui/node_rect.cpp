#include <ui/node_rect.hpp>

namespace nebula {
  bool Node_Rect::is_under_point(math::Vec2 const point) const
  {
    bool const fits_on_left = point.x >= coordinates.x;
    bool const fits_on_right = point.x <= coordinates.x + dimensions.x;
    bool const fits_on_top = point.y >= coordinates.y - dimensions.y;
    bool const fits_on_bottom = point.y <= coordinates.y;

    return fits_on_left && fits_on_bottom && fits_on_right && fits_on_top;
  }
} // namespace nebula
