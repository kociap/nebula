#include <cstdlib>
#include "movable_rect.hpp"

struct UI_Movable_Rect_S{
    void (*render_function)(anton::Rect_f32 position);
    anton::Rect_f32 position;
};

UI_Movable_Rect_S* UI_Movable_Rect_Init(void (*render_function)(anton::Rect_f32 position))
{
    if(render_function == nullptr)
        return nullptr;
    auto* mr = static_cast<UI_Movable_Rect_S *>(malloc(sizeof(UI_Movable_Rect_S)));
    mr->render_function = render_function;

    anton::Rect_f32 position = {-0.1f, 0.1f, 0.1f, -0.1f};
    mr->position = position;

    return mr;
}

void UI_Movable_Rect_Render(UI_Movable_Rect_S* mr)
{
    if(mr == nullptr)
        return;
    mr->render_function(mr->position);
}

bool UI_Movable_Rect_Is_Under_Mouse(UI_Movable_Rect_S* mr, f32 x, f32 y)
{
    if(mr == nullptr)
        return false;
    if(x >= (mr->position.left * 600.0f) + 600.0f && x <= (mr->position.right* 600.0f) + 600.0f &&
        y <= (mr->position.bottom * -400.0f) + 400.0f && y >= (mr->position.top* -400.0f) + 400.0f)
        return true;

    return false;
}

void UI_Movable_Rect_Move(UI_Movable_Rect_S* mr, f32 x_offset, f32 y_offset)
{
    if(mr == nullptr)
        return;

    const f32 horizontal_sensitivity = 2.0f;
    const f32 vertical_sensitivity = 2.0f;

    mr->position.left += (x_offset / 1200.0f) * horizontal_sensitivity;
    mr->position.right += (x_offset / 1200.0f) * horizontal_sensitivity;
    mr->position.top -= (y_offset / 800.0f) * vertical_sensitivity;
    mr->position.bottom -=  (y_offset / 800.0f) * vertical_sensitivity;
}

void UI_Movable_Rect_Destroy(UI_Movable_Rect_S* mr)
{
    free(mr);
}
