#pragma once

#include <core/types.hpp>
#include <rendering/rendering.hpp>

namespace nebula {
  enum struct port_t { in, out };

  struct Port {
  public:
    List<Port*> connections;
    Vec2 coordinates;
    f32 radius;
    port_t type;
    /**
     * Initializes a circle shaped port with specified coordinates and type.
     * @param coordinates - x and y coordinates of the center
     * @param type - ports type IN/OUT
     */
    Port(Vec2 coordinates, port_t type);

    /**
     * Moves the port by the given offset.
     * @param offset - vector, offset from previous gate position
     */
    void move(Vec2 offset);

    /**
     * Connects the port to another specified port.
     * @param new_port - pointer to the port to be connected
     */
    void add_connection(Port* new_port);

    /**
     * Disconnects the port from a specified connected port.
     * @param old_port - pointer to the port to be disconnected
     */
    void remove_connection(Port* old_port);

    /**
     * Disconnects the port from all connected ports.
     */
    void remove_all_connections();

    /**
     * Renders the circle shaped port visually.
     */
    void add_to_render_loop() const;

    /**
     * @return Ports coordinates.
     */
    [[nodiscard]] Vec2 get_coordinates() const;

    [[nodiscard]] bool is_under_mouse(Vec2 mouse_position) const;
  };

  [[nodiscard]] rendering::Draw_Elements_Command prepare_draw(Port const& port);
  [[nodiscard]] rendering::Draw_Elements_Command
  prepare_draw_connection(Vec2 cords_1, Vec2 cords_2);
} // namespace nebula
