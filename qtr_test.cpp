#include "RPiQTRSensors.h"
#include "RPiTime.h"

#define NUM_SENSORS 8
#define TIMEOUT 2500
#define EMITTER_PIN 2

RPiTime _time;

QTRSensorsRC qtrcc(
	(unsigned char[]){1,2,3,4,5,6,7,8},
	NUM_SENSORS,
	TIMEOUT,
	EMITTER_PIN
);
unsigned int sensorValues[NUM_SENSORS];

void setup(){
	_time.delay(500);

	printf("Calibrating...\n");
	for(int i=0; i<400; i++)
		qtrcc.calibrate();

	for(int i=0; i<NUM_SENSORS; i++)
		printf("%d ", qtrcc.calibratedMinimumOn[i]);
	printf("\n");

	for(int i=0; i<NUM_SENSORS; i++)
		printf("%d ", qtrcc.calibratedMaximumOn[i]);
	printf("\n");	

	_time.delay(1000);
}

void loop(){
	unsigned int position=qtrcc.readLine(sensorValues);
	for(unsigned char i=0; i<NUM_SENSORS; i++)
		printf("%d\t", sensorValues[i]);
	printf("%d\n", position);
	_time.delay(250);
}

int main(int argc, char *argv[]){
	setup();
	while(1) loop();
	return 0;
}