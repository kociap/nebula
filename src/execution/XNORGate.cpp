#include "XNORGate.hpp"

XNORGate::XNORGate() : input1(false), input2(false) {}

bool XNORGate::getOutput() const {
    return (input1 && input2) || (!input1 && !input2);
}

void XNORGate::setInput(bool input) {
    input1 = input;
}

void XNORGate::setSecondInput(bool input) {
    input2 = input;
}
