#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

#include "gc_controller.h"
#include "gc_common.h"

void RebootPi(void);

int main() {
    // Setup wiring pi
	wiringPiSetup();
	wiringPiSetupGpio();

    wiringPiISR(21, INT_EDGE_FALLING, &RebootPi);
	pinMode(21, INPUT);
	pullUpDnControl(21, PUD_UP);

    GC::Controller port_1(1, 11);

    while(true) {
        if(port_1.Connect() != ControllerStatus::CONNECTED)
            continue;

        port_1.ReadState();
        if(port_1.GetStatus() == ControllerStatus::CONNECTED)
            printf("buttonsA: %d, buttonsB: %d", port_1.GetState().buttonsA, port_1.GetState().buttonsB);
    }
}

void RebootPi(void) {
    system("reboot");
}