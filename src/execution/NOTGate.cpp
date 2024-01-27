#include "NOTGate.hpp"

NOTGate::NOTGate() : input(false) {}

bool NOTGate::getOutput() const {
    return !input;
}

void NOTGate::setInput(bool input) {
    this->input = input;
}
