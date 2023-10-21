#pragma once

#include <model/fwd.hpp>
#include <types.hpp>

namespace nebula {
  enum struct Node_Kind : u8 {
    e_bus,
    e_junction,
    e_bus_splitter,
    e_bus_combiner,
    e_gate,
    e_component,
  };

  struct Node {
    Node_Kind node_kind;
  };

  struct IO_Connector {
    Node* parent;
    Bus* bus;
  };

  struct Bus: public Node {
    IO_Connector* in;
    IO_Connector* out;
  };

  struct Junction: public Node {
    List<IO_Connector> connections;
  };

  struct Bus_Splitter: public Node {
    List<IO_Connector> outputs;
    IO_Connector input;
  };

  struct Bus_Combiner: public Node {
    List<IO_Connector> inputs;
    IO_Connector output;
  };

  struct Model {
    List<IO_Connector> inputs;
    List<IO_Connector> outputs;
    List<Node> evaluables;
    List<Node> buses;
  };

  enum struct Gate_Kind : u8 {
    // Two input gates.
    e_and,
    e_or,
    e_xor,
    e_nand,
    e_nor,
    e_xnor,
    // One input gates.
    e_not,
  };

  struct Gate: public Node {
    IO_Connector in1;
    IO_Connector in2;
    IO_Connector out;
    Gate_Kind kind;
  };

  struct Component: public Node {
    List<IO_Connector> inputs;
    List<IO_Connector> outputs;
    Model model;
  };
} // namespace nebula
