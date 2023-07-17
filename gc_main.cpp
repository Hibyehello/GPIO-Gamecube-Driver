#include <stdio.h>

#include "gc_protocol.h"

bool connectController(bool calibrate = false);

int main() {
    ControllerStatus status = ControllerStatus::DISCONNECTED;
    ControllerState state;
    while(true) {
        if(status != ControllerStatus::CONNECTED) {
            status = connectController();
            continue;
        }

        status = ReadState(&state);
        if(status == ControllerStatus::CONNECTED)
            printf("buttonsA: %d, buttonsB: %d", state.buttonsA, state.buttonsB);

    }
}

ControllerStatus connectController(bool calibrate) {
    ControllerStatus st = Probe();
    if(st != ControllerStatus::CONNECTED) return st;

    OriginRequest request;
    
    ReadOrigin(&request, calabrate);
    if(
        request.state.analogStick.X != 127 &&
        request.state.analogStick.Y != 127 &&
        request.state.cStick.X != 127 &&
        request.state.cStick.Y != 127 &&
        request.state.triggers.X != 0 &&
        request.state.triggers.Y != 0
    )
        connectController(true);

    return st;
}