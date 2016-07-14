#include "RPiQTRSensors.h"

#define NUM_SENSORS 1
#define TIMEOUT 4000
#define EMITTER_PIN 17

QTRSensorsRC qtrrc((unsigned char[]){},
  NUM_SENSORS, TIMEOUT, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];
RPiTime _time;

int main(int argc, char *argv[]){
  while(1){
    qtrrc.read(sensorValues);
    for(int i=0; i<NUM_SENSORS; i++){
      printf("%d\t", sensorValues[i]);
    }
    printf("\n");
    _time.delay(1000);
  }
  return 0;
}
