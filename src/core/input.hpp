#pragma once

namespace nebula {
  enum struct Key {
    mouse_left = 0,
    mouse_right = 1,
    mouse_3,
    mouse_4,
    mouse_5,
    key_r = 82,
    key_lctrl = 341,
    key_rctrl = 345,
  };

  enum struct Input_Action {
    release,
    press,
    repeat,
  };
} // namespace nebula
