#pragma once

#include "gc_common.h"

namespace GC {

class Controller {
public:
    Controller(int port, int pin);

    ControllerStatus GetStatus();
    ControllerState GetState();

    // I/O Functions
    void OutputLow();
    void OutputHigh();

    void WriteBit(bool state);
    void WriteSerial(uint8_t* data, uint16_t bytes);
    void ReadSerial(uint8_t* data, uint16_t bytes);

    // Protocol Functions
    void ReadWatermark(ControllerWatermark *mk, bool calibrate = false);
    void ReadOrigin(OriginRequest *og, bool calibrate = false);
    void ReadState();
    void Probe(bool calibrate = false);
    ControllerStatus Connect(bool calibrate = false);

private:
    ControllerStatus m_status;
    ControllerState m_state;
    int m_port;
    int m_data_pin;
};

}