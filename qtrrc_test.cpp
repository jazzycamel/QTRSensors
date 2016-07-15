#include <stdio.h>
#include <wiringPi.h>
#include "QTRSensors.h"

#define NUM_SENSORS 8
#define TIMEOUT 2500
#define EMITTER_PIN 0

int main(int argc, char *argv[]){
  wiringPiSetup();

  unsigned char pins[]={1,2,3,4,5,12,13,6};
  QTRSensorsRC qtrrc(pins, NUM_SENSORS, TIMEOUT, EMITTER_PIN);
  unsigned int sensorValues[NUM_SENSORS];

  // Setup
  delay(500);
  printf("Calibrating");
  fflush(stdout);
  for(int i=0; i<400; i++){
    if(i%10==0){ printf("."); fflush(stdout); }
    qtrrc.calibrate();
  }
  printf("finished!\n");

  printf("Calibration Values:\n");
  for(int i=0; i<NUM_SENSORS; i++)
    printf("%d ",qtrrc.calibratedMinimumOn[i]);
  printf("\n");

  for(int i=0; i<NUM_SENSORS; i++)
    printf("%d ",qtrrc.calibratedMaximumOn[i]);
  printf("\n\n");
  fflush(stdout);
  delay(1000);

  // Loop
  while(1){
    unsigned int position=qtrrc.readLine(sensorValues);
    for(unsigned char i=0; i<NUM_SENSORS; i++)
      printf("%d\t", sensorValues[i]);
    printf("%d\n", position);
    delay(1000);
  }

  return 0;
}
