#pragma once
#include "Gate.hpp"

class ANDGate : public Gate {
private:
    bool input1, input2;

public:
    ANDGate();
    bool getOutput() const override;
    void setInput(bool input) override;
    void setSecondInput(bool input);
};
