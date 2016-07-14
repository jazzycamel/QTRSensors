#include <stdlib.h>
#include "RPiQTRSensors.h"

QTRSensors::QTRSensors(){
	_time=new RPiTime;
	_io=new RPiGPIO;
	_io->init();
}

QTRSensors::~QTRSensors(){
    if (_pins) free(_pins);
    if(calibratedMaximumOn) free(calibratedMaximumOn);
    if(calibratedMaximumOff) free(calibratedMaximumOff);
    if(calibratedMinimumOn) free(calibratedMinimumOn);
    if(calibratedMinimumOff) free(calibratedMinimumOff);
}

void QTRSensors::init(unsigned char *pins, unsigned char numSensors, unsigned char emitterPin){
    calibratedMinimumOn=0;
    calibratedMaximumOn=0;
    calibratedMinimumOff=0;
    calibratedMaximumOff=0;

    _lastValue=0; // assume initially that the line is left.

    if (numSensors>MAX_SENSORS) _numSensors=MAX_SENSORS;
    else _numSensors=numSensors;

    if (_pins == 0){
        _pins=(unsigned char*)malloc(sizeof(unsigned char)*_numSensors);
        if (_pins==0) return;
    }

    unsigned char i;
    for (i=0; i<_numSensors; i++) _pins[i] = pins[i];

    _emitterPin = emitterPin;
}

void QTRSensors::read(unsigned int *sensor_values, unsigned char readMode){
    unsigned int off_values[MAX_SENSORS];
    unsigned char i;

    if(readMode==EMITTERS_ON || readMode==EMITTERS_ON_AND_OFF) emittersOn();
    else emittersOff();

    readPrivate(sensor_values);
    emittersOff();

    if(readMode==EMITTERS_ON_AND_OFF){
        readPrivate(off_values);
        for(i=0;i<_numSensors;i++)
            sensor_values[i]+=_maxValue-off_values[i];
    }
}

void QTRSensors::emittersOff(){
    if (_emitterPin==NO_EMITTER_PIN) return;
    _io->pinMode(_emitterPin, OUTPUT);
    _io->digitalWrite(_emitterPin, LOW);
    _time->delayMicroseconds(200);
}

void QTRSensors::emittersOn(){
    if (_emitterPin==NO_EMITTER_PIN) return;
    _io->pinMode(_emitterPin, OUTPUT);
    _io->digitalWrite(_emitterPin, HIGH);
    _time->delayMicroseconds(200);
}

void QTRSensors::resetCalibration(){
    unsigned char i;
    for(i=0;i<_numSensors;i++)
    {
        if(calibratedMinimumOn) calibratedMinimumOn[i]=_maxValue;
        if(calibratedMinimumOff) calibratedMinimumOff[i]=_maxValue;
        if(calibratedMaximumOn) calibratedMaximumOn[i]=0;
        if(calibratedMaximumOff) calibratedMaximumOff[i]=0;
    }
}

void QTRSensors::calibrate(unsigned char readMode){
    if(readMode==EMITTERS_ON_AND_OFF || readMode==EMITTERS_ON)
        calibrateOnOrOff(&calibratedMinimumOn, &calibratedMaximumOn, EMITTERS_ON);

    if(readMode==EMITTERS_ON_AND_OFF || readMode==EMITTERS_OFF)
        calibrateOnOrOff(&calibratedMinimumOff, &calibratedMaximumOff, EMITTERS_OFF);
}

void QTRSensors::calibrateOnOrOff(unsigned int **calibratedMinimum, unsigned int **calibratedMaximum, unsigned char readMode){
    unsigned int sensor_values[16];
    unsigned int max_sensor_values[16];
    unsigned int min_sensor_values[16];

    // Allocate the arrays if necessary.
    if(*calibratedMaximum==0){
        *calibratedMaximum=(unsigned int*)malloc(sizeof(unsigned int)*_numSensors);

        // If the malloc failed, don't continue.
        if(*calibratedMaximum==0) return;

        // Initialize the max and min calibrated values to values that
        // will cause the first reading to update them.

        for(int i=0;i<_numSensors;i++) (*calibratedMaximum)[i]=0;
    }

    if(*calibratedMinimum==0){
        *calibratedMinimum=(unsigned int*)malloc(sizeof(unsigned int)*_numSensors);

        // If the malloc failed, don't continue.
        if(*calibratedMinimum==0) return;

        for(int i=0;i<_numSensors;i++) (*calibratedMinimum)[i]=_maxValue;
    }

    for(int j=0;j<10;j++){
        read(sensor_values,readMode);
        for(int i=0;i<_numSensors;i++){
            // set the max we found THIS time
            if(j==0 || max_sensor_values[i]<sensor_values[i])
                max_sensor_values[i]=sensor_values[i];

            // set the min we found THIS time
            if(j==0 || min_sensor_values[i]>sensor_values[i])
                min_sensor_values[i]=sensor_values[i];
        }
    }

    // record the min and max calibration values
    for(int i=0;i<_numSensors;i++){
        if(min_sensor_values[i]>(*calibratedMaximum)[i])
            (*calibratedMaximum)[i]=min_sensor_values[i];
        if(max_sensor_values[i]<(*calibratedMinimum)[i])
            (*calibratedMinimum)[i]=max_sensor_values[i];
    }
}

void QTRSensors::readCalibrated(unsigned int *sensor_values, unsigned char readMode){
    // if not calibrated, do nothing
    if(readMode==EMITTERS_ON_AND_OFF || readMode==EMITTERS_OFF)
        if(!calibratedMinimumOff || !calibratedMaximumOff) return;
    if(readMode==EMITTERS_ON_AND_OFF || readMode==EMITTERS_ON)
        if(!calibratedMinimumOn || !calibratedMaximumOn) return;

    // read the needed values
    read(sensor_values,readMode);

    for(int i=0;i<_numSensors;i++){
        unsigned int calmin,calmax;
        unsigned int denominator;

        // find the correct calibration
        if(readMode==EMITTERS_ON){
            calmax=calibratedMaximumOn[i];
            calmin=calibratedMinimumOn[i];
        }
        else if(readMode==EMITTERS_OFF){
            calmax=calibratedMaximumOff[i];
            calmin=calibratedMinimumOff[i];
        }
        else { // QTR_EMITTERS_ON_AND_OFF
            if(calibratedMinimumOff[i]<calibratedMinimumOn[i]) // no meaningful signal
                calmin=_maxValue;
            else calmin=calibratedMinimumOn[i]+_maxValue-calibratedMinimumOff[i]; // this won't go past _maxValue

            if(calibratedMaximumOff[i]<calibratedMaximumOn[i]) // no meaningful signal
                calmax=_maxValue;
            else calmax=calibratedMaximumOn[i]+_maxValue-calibratedMaximumOff[i]; // this won't go past _maxValue
        }

        denominator=calmax-calmin;

        signed int x = 0;
        if(denominator != 0) x=(((signed long)sensor_values[i])-calmin)*1000/denominator;
        if(x<0) x=0;
        else if(x>1000) x=1000;
        sensor_values[i]=x;
    }
}

int QTRSensors::readLine(unsigned int *sensor_values, unsigned char readMode, unsigned char white_line){
    unsigned char i, on_line=0;
    unsigned long avg=0; // this is for the weighted total, which is long before division
    unsigned int sum=0; // this is for the denominator which is <= 64000

    readCalibrated(sensor_values, readMode);

    for(i=0;i<_numSensors;i++){
        int value=sensor_values[i];
        if(white_line) value=1000-value;

        // keep track of whether we see the line at all
        if(value>200) on_line=1;

        // only average in values that are above a noise threshold
        if(value>50){
            avg+=(long)(value) * (i * 1000);
            sum+=value;
        }
    }

    if(!on_line){
        // If it last read to the left of center, return 0.
        if(_lastValue<(_numSensors-1)*1000/2) return 0;

        // If it last read to the right of center, return the max.
        else return (_numSensors-1)*1000;
    }

    _lastValue = avg/sum;
    return _lastValue;
}

QTRSensorsRC::QTRSensorsRC(){
    calibratedMinimumOn=0;
    calibratedMaximumOn=0;
    calibratedMinimumOff=0;
    calibratedMaximumOff=0;
    _pins=0;
}

QTRSensorsRC::QTRSensorsRC(unsigned char* pins,
  	unsigned char numSensors, unsigned int timeout, unsigned char emitterPin){
    calibratedMinimumOn=0;
    calibratedMaximumOn=0;
    calibratedMinimumOff=0;
    calibratedMaximumOff=0;
    _pins=0;

    init(pins, numSensors, timeout, emitterPin);
}

void QTRSensorsRC::init(unsigned char* pins,
    unsigned char numSensors, unsigned int timeout, unsigned char emitterPin)
{
    QTRSensors::init(pins, numSensors, emitterPin);
    _maxValue=timeout;
}

void QTRSensorsRC::readPrivate(unsigned int *sensor_values){
    unsigned char i;

    if (_pins==0) return;

    for(i=0; i<_numSensors; i++){
        sensor_values[i]=_maxValue;
        _io->digitalWrite(_pins[i], HIGH);   // make sensor line an output
        _io->pinMode(_pins[i], OUTPUT);      // drive sensor line high
    }

    _time->delayMicroseconds(10);              // charge lines for 10 us

    for(i=0; i<_numSensors; i++)
    {
        _io->pinMode(_pins[i], INPUT);       // make sensor line an input
        _io->digitalWrite(_pins[i], LOW);        // important: disable internal pull-up!
    }

    unsigned long startTime=_time->micros();
    while(_time->micros()-startTime<_maxValue){
        unsigned int time=_time->micros()-startTime;
        for (i=0; i<_numSensors; i++){
            if (_io->digitalRead(_pins[i])==LOW && time<sensor_values[i])
                sensor_values[i] = time;
        }
    }
}
