#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <thread>

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

    GC::Controller c1(1, 11);
    GC::Controller c2(2, 7);
    GC::Controller c3(3, 5);
    GC::Controller c4(4, 3);

    auto t1 = c1.Start();
    auto t2 = c2.Start();
    auto t3 = c3.Start();
    auto t4 = c4.Start();

    t1.join();
    t2.join();
    t3.join();
    t4.join();


    return 0;
}

void RebootPi(void) {
    system("reboot");
}