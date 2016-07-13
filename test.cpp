// #include "RPi.h"
#include "RPiGPIO.h"
#include "RPiTime.h"

int main(int argc, char *argv[]){
	// if(map_peripheral(&gpio)==-1){
	// 	printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
	// 	return -1;
	// }

	// INP_GPIO(4);
	// OUT_GPIO(4);

	// while(1){
	// 	GPIO_SET=1<<4;
	// 	sleep(1);

	// 	GPIO_CLR=1<<4;
	// 	sleep(1);
	// }

	RPiTime t;

	RPiGPIO io;
	io.init();
	io.pinMode(4, RPiGPIO::OUT);
	io.digitalWrite(4, RPiGPIO::HIGH);
	io.digitalWrite(4, RPiGPIO::LOW);
	io.pinMode(4, RPiGPIO::IN);
	printf("GPIO4: %d\n", io.digitalRead(4));

	return 0;
}