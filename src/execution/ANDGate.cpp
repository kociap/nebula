#include "ANDGate.hpp"

ANDGate::ANDGate() : input1(false), input2(false) {}

bool ANDGate::getOutput() const {
    return input1 && input2;
}

void ANDGate::setInput(bool input) {
    input1 = input;
}

void ANDGate::setSecondInput(bool input) {
    input2 = input;
}
