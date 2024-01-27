#include "NORGate.hpp"

NORGate::NORGate() : input1(false), input2(false) {}

bool NORGate::getOutput() const {
    return !(input1 || input2);
}

void NORGate::setInput(bool input) {
    input1 = input;
}

void NORGate::setSecondInput(bool input) {
    input2 = input;
}
