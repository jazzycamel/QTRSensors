#include "RPiQTRSensors.h"
#include "RPiTime.h"

#define NUM_SENSORS 8
#define TIMEOUT 2500
#define EMITTER_PIN 17

RPiTime _time;

QTRSensorsRC qtrcc(
	(unsigned char[]){18,21,22,23,24,10,9,25},
	NUM_SENSORS,
	TIMEOUT,
	EMITTER_PIN
);
unsigned int sensorValues[NUM_SENSORS];

void setup(){
        printf("Setup\n");
        fflush(stdout);
	_time.delay(500);

	printf("Calibrating...");
        fflush(stdout);
	for(int i=0; i<400; i++)
		qtrcc.calibrate();

        printf("finished!\n\nCalibration Data:\n");
	for(int i=0; i<NUM_SENSORS; i++)
		printf("%d ", qtrcc.calibratedMinimumOn[i]);
	printf("\n");

	for(int i=0; i<NUM_SENSORS; i++)
		printf("%d ", qtrcc.calibratedMaximumOn[i]);
	printf("\n\nSetup Complete!\n\n");
        fflush(stdout);

	_time.delay(1000);
}

void loop(){
	unsigned int position=qtrcc.readLine(sensorValues);
	for(unsigned char i=0; i<NUM_SENSORS; i++)
		printf("%d\t", sensorValues[i]);
	printf("%d\n", position);
	_time.delay(1000);
}

int main(int argc, char *argv[]){
    printf("QTR Test\n");
    setup();
    while(1) loop();
    return 0;
}
