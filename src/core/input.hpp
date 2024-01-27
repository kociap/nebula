#pragma once

namespace nebula {
  enum struct Key {
    mouse_left = 0,
    mouse_right = 1,
    mouse_3,
    mouse_4,
    mouse_5,
    key_r = 82,
    key_left_control = 341,
    key_right_control = 345,
  };

  enum struct Input_Action {
    release,
    press,
    repeat,
  };
} // namespace nebula
