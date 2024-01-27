#pragma once
#include "Gate.hpp"

class NORGate : public Gate {
private:
    bool input1, input2;

public:
    NORGate();
    bool getOutput() const override;
    void setInput(bool input) override;
    void setSecondInput(bool input);
};
