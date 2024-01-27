#include "ORGate.hpp"

ORGate::ORGate() : input1(false), input2(false) {}

bool ORGate::getOutput() const {
    return input1 || input2;
}

void ORGate::setInput(bool input) {
    input1 = input;
}

void ORGate::setSecondInput(bool input) {
    input2 = input;
}
