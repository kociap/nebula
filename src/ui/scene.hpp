#pragma once

#include <core/types.hpp>
#include <ui/movable_gate.hpp>

namespace nebula {
  enum struct Window_Mode {
    none,
    camera_moving,
    gate_moving,
    port_linking,
  };

  struct Scene {
    Window_Mode mode = Window_Mode::none;
    Vec2 last_mouse_position;
    Movable_Gate* currently_moved_gate = nullptr;
    Port* connected_port = nullptr;
  };
} // namespace nebula
