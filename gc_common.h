#pragma once

#include <stdint.h>

/*
 * Some useful structures
*/

struct CVec28{
	int8_t X;
	int8_t Y;
};

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
};

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