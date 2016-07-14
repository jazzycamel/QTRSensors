#include "RPiGPIO.h"
#include "RPiTime.h"

int main(int argc, char *argv[]){
	RPiTime t;
	RPiGPIO io;
	io.init();

	//io.pinMode(17, RPiGPIO::OUT);
	//io.digitalWrite(17, RPiGPIO::HIGH);
	//io.digitalWrite(17, RPiGPIO::LOW);
	
	io.pinMode(17, RPiGPIO::IN);
	while(1){
		printf("GPIO4: %d\n", io.digitalRead(17));
		t.delay(1000);
	}

	return 0;
}
