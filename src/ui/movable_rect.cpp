#include <memory>
#include <ui/movable_rect.hpp>

using namespace nebula;

struct UI_Movable_Rect_S {
  void (*render_function)(anton::Rect_f32 position);
  anton::Rect_f32 position;
  f32 window_height;
  f32 window_width;
};

namespace movable_rect {
  UI_Movable_Rect_S* init(void (*render_function)(anton::Rect_f32 position),
                          i32 window_width, i32 window_height)
  {
    if(render_function == nullptr) {
      return nullptr;
    }
    std::allocator<UI_Movable_Rect_S> alloc;
    UI_Movable_Rect_S* mr = alloc.allocate(1);
    mr->render_function = render_function;

    anton::Rect_f32 position = {-0.1f, 0.1f, 0.1f, -0.1f};
    mr->position = position;
    mr->window_height = static_cast<f32>(window_height);
    mr->window_width = static_cast<f32>(window_width);

    return mr;
  }

  void render(UI_Movable_Rect_S* mr)
  {
    if(mr == nullptr) {
      return;
    }
    mr->render_function(mr->position);
  }

  bool is_under_mouse(UI_Movable_Rect_S* mr, f32 x, f32 y)
  {
    if(mr == nullptr) {
      return false;
    }
    if(x >= (mr->position.left * (mr->window_width / 2.0f)) +
              (mr->window_width / 2.0f) &&
       x <= (mr->position.right * (mr->window_width / 2.0f)) +
              (mr->window_width / 2.0f) &&
       y <= (mr->position.bottom * -(mr->window_height / 2.0f)) +
              (mr->window_height / 2.0f) &&
       y >= (mr->position.top * -(mr->window_height / 2.0f)) +
              (mr->window_height / 2.0f))
      return true;

    return false;
  }

  void move(UI_Movable_Rect_S* mr, f32 x_offset, f32 y_offset)
  {
    if(mr == nullptr) {
      return;
    }

    const f32 horizontal_sensitivity = 2.0f;
    const f32 vertical_sensitivity = 2.0f;

    mr->position.left += (x_offset / mr->window_width) * horizontal_sensitivity;
    mr->position.right +=
      (x_offset / mr->window_width) * horizontal_sensitivity;
    mr->position.top -= (y_offset / mr->window_height) * vertical_sensitivity;
    mr->position.bottom -=
      (y_offset / mr->window_height) * vertical_sensitivity;
  }

  void destroy(UI_Movable_Rect_S* mr)
  {
    std::allocator<UI_Movable_Rect_S> alloc;
    alloc.deallocate(mr, 1);
  }
} // namespace movable_rect