#include "RPiGPIO.h"
#include "RPiTime.h"
//#include "RPi.h"

int main(int argc, char *argv[]){
	// RPiTime t;

 //    if(map_peripheral(&gpio)==-1){
 //        printf("Failed to map registers!\n");
 //        return -1;
 //    }

	// INP_GPIO(17);
	// t.delay(1000);
	// while(1){
	// 	printf("GPIO17: %d\n", GPIO_READ(17));
	// 	t.delay(1000);
	// }

	RPiTime t;
	RPiGPIO io;
	if(!io.init()){
		printf("Failed to initialise GPIO!\n");
		return -1;
	}

	// io.pinMode(17, RPiGPIO::OUT);
	// io.digitalWrite(17, RPiGPIO::HIGH);
	// io.digitalWrite(17, RPiGPIO::LOW);
	
	io.pinMode(17, RPiGPIO::IN);
	t.delay(1000);
	while(1){
		printf("GPIO17: %d\n", io.digitalRead(17));
		t.delay(1000);
	}

	return 0;
}
