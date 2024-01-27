#include "Clock.hpp"

Clock::Clock() : signal(false) {}

void Clock::connectGate(Gate* gate) {
    connectedGates.push_back(gate);
}

void Clock::start(int durationSeconds) {
    for (int i = 0; i < durationSeconds; ++i) {
        signal = !signal;

        // update gate connections in each cycle
        for (Gate* gate : connectedGates) {
            gate->setInput(signal);
        }

        // display results
        std::cout << "Clock Signal: " << signal << std::endl;
        for (size_t i = 0; i < connectedGates.size(); ++i) {
            std::cout << "Gate " << i + 1 << " Output: " << connectedGates[i]->getOutput() << std::endl;
        }

        std::cout << "---------------------" << std::endl;

        // wait 1 second
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
