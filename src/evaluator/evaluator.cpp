#include <evaluator/evaluator.hpp>

namespace nebula {
  [[nodiscard]] static bool get_input_value(Port* const port)
  {
    if(port->connections.size() == 1) {
      Port* const other = *port->connections.begin();
      return other->gate->evaluation.prev_value;
    } else {
      return false;
    }
  }

  void evaluate(List<Gate>& gates)
  {
    for(Gate& gate: gates) {
      gate.evaluation.prev_value = gate.evaluation.value;
    }

    for(Gate& gate: gates) {
      switch(gate.kind) {
      case Gate_Kind::e_and: {
        ANTON_ASSERT(gate.in_ports.size() == 2, "AND gate ports not equal 2");
        bool const in1 = get_input_value(gate.in_ports[0]);
        bool const in2 = get_input_value(gate.in_ports[1]);
        gate.evaluation.value = in1 && in2;
      } break;

      case Gate_Kind::e_or: {
        ANTON_ASSERT(gate.in_ports.size() == 2, "OR gate ports not equal 2");
        bool const in1 = get_input_value(gate.in_ports[0]);
        bool const in2 = get_input_value(gate.in_ports[1]);
        gate.evaluation.value = in1 || in2;
      } break;

      case Gate_Kind::e_xor: {
        ANTON_ASSERT(gate.in_ports.size() == 2, "XOR gate ports not equal 2");
        bool const in1 = get_input_value(gate.in_ports[0]);
        bool const in2 = get_input_value(gate.in_ports[1]);
        gate.evaluation.value = (in1 && !in2) || (!in1 && in2);
      } break;

      case Gate_Kind::e_nand: {
        ANTON_ASSERT(gate.in_ports.size() == 2, "NAND gate ports not equal 2");
        bool const in1 = get_input_value(gate.in_ports[0]);
        bool const in2 = get_input_value(gate.in_ports[1]);
        gate.evaluation.value = !(in1 && in2);
      } break;

      case Gate_Kind::e_nor: {
        ANTON_ASSERT(gate.in_ports.size() == 2, "NOR gate ports not equal 2");
        bool const in1 = get_input_value(gate.in_ports[0]);
        bool const in2 = get_input_value(gate.in_ports[1]);
        gate.evaluation.value = !(in1 || in2);
      } break;

      case Gate_Kind::e_xnor: {
        ANTON_ASSERT(gate.in_ports.size() == 2, "XNOR gate ports not equal 2");
        bool const in1 = get_input_value(gate.in_ports[0]);
        bool const in2 = get_input_value(gate.in_ports[1]);
        gate.evaluation.value = !(in1 && !in2) && !(!in1 && in2);
      } break;

      case Gate_Kind::e_not: {
        ANTON_ASSERT(gate.in_ports.size() == 1, "NOT gate ports not equal 1");
        bool const in1 = get_input_value(gate.in_ports[0]);
        gate.evaluation.value = !in1;
      } break;

      case Gate_Kind::e_input: {
        // Nothing to do.
      } break;
      case Gate_Kind::e_clock: {
        gate.evaluation.value = !gate.evaluation.value;
        gate.evaluation.prev_value = !gate.evaluation.prev_value;
      } break;
      }
    }
  }
} // namespace nebula
