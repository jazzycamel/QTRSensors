#ifndef RPIGPIO_H
#define RPIGPIO_H

#include "RPi.h"

class RPiGPIO {
public:
	enum {
		IN=0,
		OUT=1,
		LOW=0,
		HIGH=1,
	};

	RPiGPIO();
	~RPiGPIO();

	bool init();
	bool isReady() { return _ready; }
	void pinMode(int pin, int mode);
	void digitalWrite(int pin, int value);
	int digitalRead(int pin);

private:
	bool _ready;
};

#endif