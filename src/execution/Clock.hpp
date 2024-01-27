#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "Gate.hpp"

class Clock {
private:
    bool signal;
    std::vector<Gate*> connectedGates;

public:
    Clock();
    void connectGate(Gate* gate);
    void start(int durationSeconds);
};
