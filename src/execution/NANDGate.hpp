#pragma once
#include "Gate.hpp"

class NANDGate : public Gate {
private:
    bool input1, input2;

public:
    NANDGate();
    bool getOutput() const override;
    void setInput(bool input) override;
    void setSecondInput(bool input);
};
