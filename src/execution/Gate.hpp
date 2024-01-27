#pragma once

class Gate {
public:
    virtual bool getOutput() const = 0;
    virtual void setInput(bool input) = 0;
};
