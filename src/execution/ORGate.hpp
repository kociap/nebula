#pragma once
#include "Gate.hpp"

class ORGate : public Gate {
private:
    bool input1, input2;

public:
    ORGate();
    bool getOutput() const override;
    void setInput(bool input) override;
    void setSecondInput(bool input);
};
