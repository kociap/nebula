#include <ui/movable_rect.hpp>

namespace nebula {
  Movable_Rect::Movable_Rect(void (*render_function)(Node_Rect rect),
                             math::Vec2 const rectangle_dimensions,
                             math::Vec2 const window_dimensions)
  {
    // Center the rectangle on the screen
    math::Vec2 coordinates;
    // Half of window width - half of rectangle width = x
    coordinates.x =
      (window_dimensions.x / (f32)2.0) - (rectangle_dimensions.x / (f32)2.0);
    // Half of window height + half of rectangle height = y
    coordinates.y =
      (window_dimensions.y / (f32)2.0) + (rectangle_dimensions.y / (f32)2.0);

    rect = {coordinates, rectangle_dimensions};
    this->render_function = render_function;
  }

  void Movable_Rect::Movable_Rect::render() const
  {
    render_function(rect);
  }

  bool Movable_Rect::is_under_mouse(math::Vec2 const mouse_location) const
  {
    return rect.is_under_point(mouse_location);
  }

  void Movable_Rect::move(math::Vec2 const offset)
  {
    rect.coordinates.x += offset.x;
    rect.coordinates.y += offset.y;
  }

} // namespace nebula
