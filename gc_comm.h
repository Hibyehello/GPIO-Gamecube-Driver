// Written by Rainbain

#pragma once
#include <stdint.h>

#include <wiringPi.h>

#define CONTROLLER_PIN 0

void Initalize();
void OutputLow();
void OutputHigh();


void Initalize() {
	// Setup wiring pi
	wiringPiSetup();
	wiringPiSetupGpio();
	
	pinMode(CONTROLLER_PIN, INPUT); // We dont need to drive it yet.
	digitalWrite(CONTROLLER_PIN, LOW); // Keep it low, even before we do anything to be safe
	pullUpDnControl(CONTROLLER_PIN, PUD_UP); // Internal pull up resistor enabled.
}

/*
 * GPIO Controlls
*/

void OutputLow(){
	// Override the pull down pulling it low
	pinMode(CONTROLLER_PIN, OUTPUT);
}

void OutputHigh(){
	// Let the pull down drive it high
	pinMode(CONTROLLER_PIN, INPUT);
}


/*
 * Serial
*/

void WriteBit(bool state){
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

void WriteSerial(uint8_t* data, uint16_t bytes){
	for(uint16_t i = 0; i < data; i++){
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

void ReadSerial(uint8_t* data, uint16_t bytes) {
	// Wait for start
	while(digitalRead(CONTROLLER_PIN) == true);
	
	// Read Offset
	delayMicroseconds(2);
	
	// Download our data
	for(uint16_t i = 0; i < data; i++){
		// Download a byte
		uint8_t byte = 0;
		for(uint8_t j = 0; j < 8; j++){
			if(digitalRead(CONTROLLER_PIN)) byte |= (0x80>>i);
		}
		delayMicroseconds(4);
		
		data[i] = byte;
	}
}