#include "RPiGPIO.h"

RPiGPIO::RPiGPIO() : _ready(false) {

}

RPiGPIO::~RPiGPIO(){
	unmap_peripheral(&gpio);
}

bool RPiGPIO::init(){
	if(map_peripheral(&gpio)==0) _ready=true;
	return _ready;
}

void RPiGPIO::pinMode(int pin, int mode){
	if(!_ready) return;

	INP_GPIO(pin);
	switch(mode){
		case IN: break;
		case OUT: OUT_GPIO(pin);
	}
}

void RPiGPIO::digitalWrite(int pin, int value){
	if(!_ready) return;
	switch(value){
		case HIGH:
			GPIO_SET=1<<pin;
			break;
		case LOW: GPIO_CLR=1<<pin;
	}
}

int RPiGPIO::digitalRead(int pin){
	if(!_ready) return LOW;
	return GPIO_READ(pin)==0 ? LOW : HIGH;
}