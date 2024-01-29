#include <model/gate.hpp>

namespace nebula {
  Gate::Gate(math::Vec2 const _dimensions, math::Vec2 const _coordinates,
             Gate_Kind const _kind)
    : coordinates(_coordinates), dimensions(_dimensions), kind(_kind)
  {
    i32 const out_count = 1;
    i32 const in_count = kind == Gate_Kind::e_not ? 1 : 2;

    // Distance between IN ports
    f32 const in_space = dimensions.y / static_cast<f32>(in_count);
    for(u8 i = 0; i < in_count; i++) {
      // Create IN ports along the left edge of the gate
      f32 const voffset = (i + 0.5f) * in_space;
      Vec2 port_coordinates = {coordinates.x, coordinates.y + voffset};
      in_ports.emplace_back(new Port(port_coordinates, Port_Kind::in, this));
    }

    // Distance between OUT ports
    f32 const out_space = dimensions.y / static_cast<f32>(out_count);
    for(u8 i = 0; i < out_count; i++) {
      // Create OUT ports along the right edge of the gate
      f32 const voffset = (i + 0.5f) * out_space;
      Vec2 port_coordinates = {coordinates.x + dimensions.x,
                               coordinates.y + voffset};
      out_ports.emplace_back(new Port(port_coordinates, Port_Kind::out, this));
    }
  }

  void Gate::move(math::Vec2 const offset)
  {
    coordinates.x += offset.x;
    coordinates.y += offset.y;
    // Move ports
    for(Port* p: in_ports) {
      p->move(offset);
    }
    for(Port* p: out_ports) {
      p->move(offset);
    }
  }

  bool test_hit(Gate const& gate, math::Vec2 const point)
  {
    Vec2 const coordinates = gate.coordinates;
    Vec2 const dimensions = gate.dimensions;
    bool const left = point.x >= coordinates.x;
    bool const right = point.x <= coordinates.x + dimensions.x;
    bool const top = point.y >= coordinates.y;
    bool const bottom = point.y <= coordinates.y + dimensions.y;
    return left && right && top && bottom;
  }

  rendering::Draw_Elements_Command prepare_draw(Gate const& gate)
  {
    math::Vec3 const green{0.498f, 1.0f, 0.0f};
    math::Vec3 const red{1.0f, 0.0f, 0.2235f};
    math::Vec3 const color = gate.evaluation.value ? green : red;
    Vertex vert[] = {
      Vertex{.position = {gate.coordinates.x + gate.dimensions.x,
                          gate.coordinates.y + gate.dimensions.y, 0.0f},
             .normal = color,
             .uv = {1.0f, 1.0f}},
      Vertex{.position = {gate.coordinates.x,
                          gate.coordinates.y + gate.dimensions.y, 0.0f},
             .normal = color,
             .uv = {0.0f, 1.0f}},
      Vertex{.position = {gate.coordinates.x + gate.dimensions.x,
                          gate.coordinates.y, 0.0f},
             .normal = color,
             .uv = {1.0f, 0.0f}},
      Vertex{.position = {gate.coordinates.x, gate.coordinates.y, 0.0f},
             .normal = color,
             .uv = {0.0f, 0.0f}},
    };
    u32 indices[] = {
      0, 1, 2, 1, 3, 2,
    };
    rendering::Draw_Elements_Command cmd =
      rendering::write_geometry(vert, indices);
    cmd.instance_count = 1;
    return cmd;
  }
} // namespace nebula
