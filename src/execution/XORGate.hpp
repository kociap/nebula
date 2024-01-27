#pragma once
#include "Gate.hpp"

class XORGate : public Gate {
private:
    bool input1, input2;

public:
    XORGate();
    bool getOutput() const override;
    void setInput(bool input) override;
    void setSecondInput(bool input);
};
