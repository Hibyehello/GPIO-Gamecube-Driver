// Written by Rainbain

#pragma once

#include <stdint.h>
#include "gc_comm.cpp"

/*
 * Some useful structures
*/

struct CVec28{
	int8_t X;
	int8_t Y;
}

enum class ControllerStatus {
	DISCONNECTED,
	CONNECTED,
	ERROR
};


/*
 * Return Packets
*/

struct ControllerWatermark {
	uint8_t mkA;
	uint8_t mkB;
	uint8_t mkC;
}

struct ControllerState {
	uint8_t buttonsA;
	uint8_t buttonsB;
	CVec28 analogStick;
	CVec28 cStick;
	CVec28 triggers;
};

struct OriginRequest {
	ControllerState state;
	uint8_t reservedA;
	uint8_t reservedB;
};

void ReadWatermark(ControllerWatermark *mk, bool calabrate){
	uint8_t cmd[] = {0x00};
	
	if(calabrate) cmd[0] = 0xFF;
	
	WriteSerial(cmd, sizeof(cmd));
	
	ReadSerial((uint8_t*)mk, sizeof(ControllerWatermark));
}

void ReadOrigin(OriginRequest *og, bool calabrate = false){
	uint8_t cmd[] = {0x41};
	
	if(calabrate) cmd[0] = 0x42;
	
	WriteSerial(cmd, sizeof(cmd));
	
	ReadSerial((uint8_t*)og, sizeof(OriginRequest));
}

ControllerStatus ReadState(ControllerState *st){
	uint8_t cmd[] = {0x40, 0x03, 0x03};
	
	WriteSerial(cmd, sizeof(cmd));
	
	ReadSerial((uint8_t*)st, sizeof(ControllerState));

	return ControllerStatus::CONNECTED;
}

ControllerStatus Probe(bool calabrate = false){
	ControllerWatermark watermark;
	
	ReadWatermark(&watermark, calabrate);
	
	if(watermark.mkA == 0x09 &
	   watermark.mkB == 0x00 &
	   watermark.mkC == 0x03 ) return ControllerStatus::CONNECTED;
	   
	// Make sure when a timeout is reached, read serial just returns FF, FF, FF
	// As that would be the expected functionality for a bus of this type
	if(watermark.mkA == 0xFF &
	   watermark.mkB == 0xFF &
	   watermark.mkC == 0xFF ) return ControllerStatus::DISCONNECTED;
	
	// If we reach this the controllers connected, just not in a state we expect.
	return ControllerStatus::ERROR;
}
