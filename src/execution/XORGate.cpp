#include "XORGate.hpp"

XORGate::XORGate() : input1(false), input2(false) {}

bool XORGate::getOutput() const {
    return (input1 || input2) && !(input1 && input2);
}

void XORGate::setInput(bool input) {
    input1 = input;
}

void XORGate::setSecondInput(bool input) {
    input2 = input;
}
