#include "gc_controller.h"

#include <wiringPi.h>
#include <stdio.h>
#include <stdarg.h>

#define SERIAL_MAX_LOOP 100

std::mutex print_mutex;

namespace GC {
    Controller::Controller(int port, int pin) {
        m_port = port;
        m_data_pin = pin;

        pinMode(m_data_pin, INPUT); // We dont need to drive it yet.
	    digitalWrite(m_data_pin, LOW); // Keep it low, even before we do anything to be safe
	    pullUpDnControl(m_data_pin, PUD_UP); // Internal pull up resistor enabled.
    }

    std::thread Controller::Start() {
        return std::thread(&Controller::Run, this);
    }

    void Controller::Run() {
        while(true) {
            DebugPrint("Probing port: %d\n", m_port);
            Connect();
            if(m_status != ControllerStatus::CONNECTED)
                continue;

            ReadState();
            if(GetStatus() == ControllerStatus::CONNECTED)
                printf("buttonsA: %d, buttonsB: %d\n", GetState().buttonsA, GetState().buttonsB);
        }
    }

    void Controller::DebugPrint(const char* fmt, ...) {
        std::lock_guard<std::mutex> guard(print_mutex);
        va_list arglist;
        va_start(arglist, fmt);

        vprintf(fmt, arglist);

        va_end(arglist);
    }

    ControllerStatus Controller::GetStatus() {
        return m_status;
    }

    ControllerState Controller::GetState() {
        return m_state;
    }

    void Controller::OutputLow() {
        // Override the pull down pulling it low
	    pinMode(m_data_pin, OUTPUT);
    }

    void Controller::OutputHigh() {
        // Override the pull down pulling it low
	    pinMode(m_data_pin, INPUT);
    }

    void Controller::WriteBit(bool state){
        if(state) { // Write a 1
            OutputLow();
            delayMicroseconds(3);
            OutputHigh();
            delayMicroseconds(1);
        } else {    // Write a 0
            OutputLow();
            delayMicroseconds(1);
            OutputHigh();
            delayMicroseconds(3);
        }
    }

    void Controller::WriteSerial(uint8_t* data, uint16_t bytes){
        for(uint16_t i = 0; i < bytes && i < SERIAL_MAX_LOOP; i++){
            // Fast and easy way.
            WriteBit(((data[i]) & 0b10000000) > 0);
            WriteBit(((data[i]) & 0b01000000) > 0);
            WriteBit(((data[i]) & 0b00100000) > 0);
            WriteBit(((data[i]) & 0b00010000) > 0);
            WriteBit(((data[i]) & 0b00001000) > 0);
            WriteBit(((data[i]) & 0b00000100) > 0);
            WriteBit(((data[i]) & 0b00000010) > 0);
            WriteBit(((data[i]) & 0b00000001) > 0);
        }
        
        // Stop bit
        WriteBit(true);
    }

    void Controller::ReadSerial(uint8_t* data, uint16_t bytes) {
        // Wait for start
        int i = 0;
        while(digitalRead(m_data_pin) == true) {
            if(i > SERIAL_MAX_LOOP) {
                DebugPrint("Wait for start timed out, port: %d\n", m_port);
                return;
            }
            i++;
        }
        
        // Read Offset
        delayMicroseconds(2);
        
        // Download our data
        for(uint16_t i = 0; i < bytes && i < SERIAL_MAX_LOOP; i++){
            // Download a byte
            uint8_t byte = 0;
            for(uint8_t j = 0; j < 8; j++){
                if(digitalRead(m_data_pin)) byte |= (0x80>>i);
            }
            delayMicroseconds(4);
            
            data[i] = byte;
        }
    }

    void Controller::ReadWatermark(ControllerWatermark *mk, bool calibrate){
        uint8_t cmd[] = {0x00};
        
        if(calibrate) cmd[0] = 0xFF;
        
        WriteSerial(cmd, sizeof(cmd));
        
        ReadSerial((uint8_t*)mk, sizeof(ControllerWatermark));
    }

    void Controller::ReadOrigin(OriginRequest *og, bool calibrate){
        uint8_t cmd[] = {0x41};
        
        if(calibrate) cmd[0] = 0x42;
        
        WriteSerial(cmd, sizeof(cmd));
        
        ReadSerial((uint8_t*)og, sizeof(OriginRequest));
    }

    void Controller::ReadState(){
        uint8_t cmd[] = {0x40, 0x03, 0x03};
        
        WriteSerial(cmd, sizeof(cmd));
        
        ReadSerial((uint8_t*)&m_state, sizeof(ControllerState));
    }

    void Controller::Probe(bool calibrate){
        ControllerWatermark watermark;
        
        ReadWatermark(&watermark, calibrate);
        
        if(watermark.mkA == 0x09 &
        watermark.mkB == 0x00 &
        watermark.mkC == 0x03 ) m_status = ControllerStatus::CONNECTED;
        
        // Make sure when a timeout is reached, read serial just returns FF, FF, FF
        // As that would be the expected functionality for a bus of this type
        if(watermark.mkA == 0xFF &
        watermark.mkB == 0xFF &
        watermark.mkC == 0xFF ) m_status = ControllerStatus::DISCONNECTED;
        
        // If we reach this the controllers connected, just not in a state we expect.
        m_status = ControllerStatus::ERROR;
    }

    ControllerStatus Controller::Connect(bool calibrate) {
        Probe();
        if(m_status != ControllerStatus::CONNECTED) { 
            return m_status;
        } 

        OriginRequest request;
    
        ReadOrigin(&request, calibrate);
        if(
            request.state.analogStick.X != 127 &&
            request.state.analogStick.Y != 127 &&
            request.state.cStick.X != 127 &&
            request.state.cStick.Y != 127 &&
            request.state.triggers.X != 0 &&
            request.state.triggers.Y != 0
        )
        Connect(true);

        return m_status;
    }

}
