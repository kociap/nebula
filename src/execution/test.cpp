#include "ANDGate.hpp"
#include "ORGate.hpp"
#include "NOTGate.hpp"
#include "XORGate.hpp"
#include "NORGate.hpp"
#include "XNORGate.hpp"
#include "NANDGate.hpp"
#include "Clock.hpp"

int test() {
    ANDGate andGate;
    ORGate orGate;
    NOTGate notGate;
    XORGate xorGate;
    NORGate norGate;
    XNORGate xnorGate;
    NANDGate nandGate;

    Clock clock;

    // connect gates to the clock
    clock.connectGate(&andGate);
    clock.connectGate(&orGate);
    clock.connectGate(&notGate);
    clock.connectGate(&xorGate);
    clock.connectGate(&norGate);
    clock.connectGate(&xnorGate);
    clock.connectGate(&nandGate);

    // connect gates
    andGate.setInput(notGate.getOutput());
    orGate.setInput(andGate.getOutput());
    xorGate.setInput(orGate.getOutput());
    norGate.setInput(xorGate.getOutput());
    xnorGate.setInput(norGate.getOutput());
    nandGate.setInput(xnorGate.getOutput());
    andGate.setSecondInput(true);
    orGate.setSecondInput(true);
    xorGate.setSecondInput(true);
    norGate.setSecondInput(true);
    xnorGate.setSecondInput(true);
    nandGate.setSecondInput(true);

    // start clock for 5 seconds
    clock.start(5);

    return 0;
}
