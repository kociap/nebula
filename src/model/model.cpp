#include <model/model.hpp>

namespace nebula {
    struct AND_Gate : public Gate {
        AND_Gate() {
            node_kind = Node_Kind::e_gate;
            kind = Gate_Kind::e_and;
        }

        void evaluate() override {
            bool result = in1.bus->get_state() && in2.bus->get_state();
            out.bus->set_state(result);
        }
    };

    struct OR_Gate : public Gate {
        OR_Gate() {
            node_kind = Node_Kind::e_gate;
            kind = Gate_Kind::e_or;
        }

        void evaluate() override {
            bool result = in1.bus->get_state() || in2.bus->get_state();
            out.bus->set_state(result);
        }
    };

    struct XOR_Gate : public Gate {
        XOR_Gate() {
            node_kind = Node_Kind::e_gate;
            kind = Gate_Kind::e_xor;
        }

        void evaluate() override {
            bool result = (in1.bus->get_state() || in2.bus->get_state()) && !(in1.bus->get_state() && in2.bus->get_state());
            out.bus->set_state(result);
        }
    };

    struct NAND_Gate : public Gate {
        NAND_Gate() {
            node_kind = Node_Kind::e_gate;
            kind = Gate_Kind::e_nand;
        }

        void evaluate() override {
            bool result = !(in1.bus->get_state() && in2.bus->get_state());
            out.bus->set_state(result);
        }
    };

    struct NOR_Gate : public Gate {
        NOR_Gate() {
            node_kind = Node_Kind::e_gate;
            kind = Gate_Kind::e_nor;
        }

        void evaluate() override {
            bool result = !(in1.bus->get_state() || in2.bus->get_state());
            out.bus->set_state(result);
        }
    };

    struct XNOR_Gate : public Gate {
        XNOR_Gate() {
            node_kind = Node_Kind::e_gate;
            kind = Gate_Kind::e_xnor;
        }

        void evaluate() override {
            bool result = !((in1.bus->get_state() || in2.bus->get_state()) && !(in1.bus->get_state() && in2.bus->get_state()));
            out.bus->set_state(result);
        }
    };

    struct NOT_Gate : public Gate {
        NOT_Gate() {
            node_kind = Node_Kind::e_gate;
            kind = Gate_Kind::e_not;
        }

        void evaluate() override {
            bool result = !in1.bus->get_state();
            out.bus->set_state(result);
        }
    };

} 
