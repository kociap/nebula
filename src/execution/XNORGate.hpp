#pragma once
#include "Gate.hpp"

class XNORGate : public Gate {
private:
    bool input1, input2;

public:
    XNORGate();
    bool getOutput() const override;
    void setInput(bool input) override;
    void setSecondInput(bool input);
};
