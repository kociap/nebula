#include "NANDGate.hpp"

NANDGate::NANDGate() : input1(false), input2(false) {}

bool NANDGate::getOutput() const {
    return !(input1 && input2);
}

void NANDGate::setInput(bool input) {
    input1 = input;
}

void NANDGate::setSecondInput(bool input) {
    input2 = input;
}
