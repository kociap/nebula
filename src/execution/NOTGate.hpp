#pragma once
#include "Gate.hpp"

class NOTGate : public Gate {
private:
    bool input;

public:
    NOTGate();
    bool getOutput() const override;
    void setInput(bool input) override;
};
