#include <wiringPi.h>
#include <stdlib.h>
#include "QTRSensors.h"

QTRSensorsRC::QTRSensorsRC(unsigned char* pins, unsigned char numSensors,
        unsigned int timeout, unsigned char emitterPin){
    calibratedMinimumOn=0;
    calibratedMaximumOn=0;
    calibratedMinimumOff=0;
    calibratedMaximumOff=0;
    _pins=0;

    init(pins, numSensors, timeout, emitterPin);
}

QTRSensorsRC::~QTRSensorsRC(){
    if (_pins) free(_pins);
    if(calibratedMaximumOn) free(calibratedMaximumOn);
    if(calibratedMaximumOff) free(calibratedMaximumOff);
    if(calibratedMinimumOn) free(calibratedMinimumOn);
    if(calibratedMinimumOff) free(calibratedMinimumOff);
}

void QTRSensorsRC::init(unsigned char* pins, unsigned char numSensors,
        unsigned int timeout, unsigned char emitterPin){
    calibratedMinimumOn=0;
    calibratedMaximumOn=0;
    calibratedMinimumOff=0;
    calibratedMaximumOff=0;
    _lastValue=0;

    if (numSensors>QTR_MAX_SENSORS) _numSensors=QTR_MAX_SENSORS;
    else _numSensors=numSensors;

    if (_pins==0){
        _pins=(unsigned char*)malloc(sizeof(unsigned char)*_numSensors);
        if (_pins==0) return;
    }

    for (unsigned char i=0; i<_numSensors; i++) _pins[i]=pins[i];

    _maxValue=timeout;
    _emitterPin=emitterPin;
}

void QTRSensorsRC::read(unsigned int *sensor_values, unsigned char readMode) {
    unsigned int off_values[QTR_MAX_SENSORS];

    if(readMode==QTR_EMITTERS_ON || readMode==QTR_EMITTERS_ON_AND_OFF)
        emittersOn();
    else emittersOff();

    readPrivate(sensor_values);
    emittersOff();

    if(readMode==QTR_EMITTERS_ON_AND_OFF){
        readPrivate(off_values);

        for(unsigned char i=0; i<_numSensors; i++)
            sensor_values[i]+=_maxValue-off_values[i];
    }
}

void QTRSensorsRC::emittersOff(){
    if (_emitterPin==QTR_NO_EMITTER_PIN) return;
    pinMode(_emitterPin, OUTPUT);
    digitalWrite(_emitterPin, LOW);
    delayMicroseconds(200);
}

void QTRSensorsRC::emittersOn(){
    if (_emitterPin==QTR_NO_EMITTER_PIN) return;
    pinMode(_emitterPin, OUTPUT);
    digitalWrite(_emitterPin, HIGH);
    delayMicroseconds(200);
}

void QTRSensorsRC::calibrate(unsigned char readMode){
    if(readMode==QTR_EMITTERS_ON_AND_OFF || readMode==QTR_EMITTERS_ON)
        calibrateOnOrOff(&calibratedMinimumOn, &calibratedMaximumOn, QTR_EMITTERS_ON);

    if(readMode==QTR_EMITTERS_ON_AND_OFF || readMode==QTR_EMITTERS_OFF)
        calibrateOnOrOff(&calibratedMinimumOff, &calibratedMaximumOff, QTR_EMITTERS_OFF);
}

void QTRSensorsRC::resetCalibration(){
    for(unsigned char i=0;i<_numSensors;i++){
        if(calibratedMinimumOn) calibratedMinimumOn[i]=_maxValue;
        if(calibratedMinimumOff) calibratedMinimumOff[i]=_maxValue;
        if(calibratedMaximumOn) calibratedMaximumOn[i]=0;
        if(calibratedMaximumOff) calibratedMaximumOff[i]=0;
    }
}

void QTRSensorsRC::readCalibrated(unsigned int *sensor_values, unsigned char readMode){
    if(readMode==QTR_EMITTERS_ON_AND_OFF || readMode==QTR_EMITTERS_OFF)
        if(!calibratedMinimumOff || !calibratedMaximumOff) return;
    if(readMode==QTR_EMITTERS_ON_AND_OFF || readMode==QTR_EMITTERS_ON)
        if(!calibratedMinimumOn || !calibratedMaximumOn) return;

    read(sensor_values,readMode);

    for(int i=0;i<_numSensors;i++){
        unsigned int calmin, calmax, denominator;

        if(readMode==QTR_EMITTERS_ON){
            calmax=calibratedMaximumOn[i];
            calmin=calibratedMinimumOn[i];
        }
        else if(readMode==QTR_EMITTERS_OFF){
            calmax=calibratedMaximumOff[i];
            calmin=calibratedMinimumOff[i];
        }
        else {// QTR_EMITTERS_ON_AND_OFF
            if(calibratedMinimumOff[i]<calibratedMinimumOn[i]) calmin=_maxValue;
            else calmin=calibratedMinimumOn[i] + _maxValue-calibratedMinimumOff[i];

            if(calibratedMaximumOff[i]<calibratedMaximumOn[i]) calmax=_maxValue;
            else calmax=calibratedMaximumOn[i] + _maxValue-calibratedMaximumOff[i];
        }

        denominator=calmax-calmin;

        signed int x=0;
        if(denominator != 0) x=(((signed long)sensor_values[i])-calmin)*1000/denominator;
        if(x<0) x=0;
        else if(x>1000) x=1000;
        sensor_values[i]=x;
    }
}

int QTRSensorsRC::readLine(unsigned int *sensor_values,
        unsigned char readMode, unsigned char white_line){
    unsigned char on_line=0;
    unsigned long avg=0, sum=0;

    readCalibrated(sensor_values, readMode);

    for(unsigned char i=0; i<_numSensors; i++){
        int value=sensor_values[i];
        if(white_line) value=1000-value;

        if(value>200) on_line=1;
        if(value>50){
            avg+=(long)(value)*(i*1000);
            sum+=value;
        }
    }

    if(!on_line){
        if(_lastValue<(_numSensors-1)*1000/2) return 0;
        else return (_numSensors-1)*1000;
    }

    _lastValue=avg/sum;
    return _lastValue;
}

void QTRSensorsRC::calibrateOnOrOff(unsigned int **calibratedMinimum,
        unsigned int **calibratedMaximum, unsigned char readMode){
    unsigned int sensor_values[16], max_sensor_values[16], min_sensor_values[16];

    if(*calibratedMaximum==0) {
        *calibratedMaximum=(unsigned int*)malloc(sizeof(unsigned int)*_numSensors);

        if(*calibratedMaximum==0) return;
        for(int i=0;i<_numSensors;i++) (*calibratedMaximum)[i]=0;
    }

    if(*calibratedMinimum==0){
        *calibratedMinimum=(unsigned int*)malloc(sizeof(unsigned int)*_numSensors);
        if(*calibratedMinimum==0) return;
        for(int i=0;i<_numSensors;i++) (*calibratedMinimum)[i]=_maxValue;
    }

    for(int j=0;j<10;j++){
        read(sensor_values,readMode);
        for(int i=0;i<_numSensors;i++){
            if(j==0 || max_sensor_values[i]<sensor_values[i])
                max_sensor_values[i]=sensor_values[i];
            if(j==0 || min_sensor_values[i]>sensor_values[i])
                min_sensor_values[i]=sensor_values[i];
        }
    }

    for(int i=0;i<_numSensors;i++){
        if(min_sensor_values[i]>(*calibratedMaximum)[i])
            (*calibratedMaximum)[i]=min_sensor_values[i];
        if(max_sensor_values[i]<(*calibratedMinimum)[i])
            (*calibratedMinimum)[i]=max_sensor_values[i];
    }
}

void QTRSensorsRC::readPrivate(unsigned int *sensor_values){
    if (_pins==0) return;

    for(unsigned int i=0; i<_numSensors; i++){
        sensor_values[i]=_maxValue;
        digitalWrite(_pins[i], HIGH);
        pinMode(_pins[i], OUTPUT);
    }

    delayMicroseconds(10);

    for(unsigned int i=0; i<_numSensors; i++){
        pinMode(_pins[i], INPUT);
        digitalWrite(_pins[i], LOW);
    }

    unsigned long startTime=micros();
    while (micros()-startTime<_maxValue){
        unsigned int time=micros()-startTime;
        for (unsigned int i=0; i<_numSensors; i++){
            if (digitalRead(_pins[i])==LOW && time<sensor_values[i])
                sensor_values[i]=time;
        }
    }
}