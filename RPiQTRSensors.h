#ifndef RPIQTRSENSORS_H
#define RPIQTRSENSORS_H

#include "RPiTime.h"
#include "RPiGPIO.h"

#define OUTPUT RPiGPIO::OUT
#define INPUT RPiGPIO::IN
#define HIGH RPiGPIO::HIGH
#define LOW RPiGPIO::LOW

class QTRSensors {
public:
	enum {
		EMITTERS_OFF=0,
		EMITTERS_ON=1,
		EMITTERS_ON_AND_OFF=2,
		NO_EMITTER_PIN=255,
		MAX_SENSORS=16,
	};

	~QTRSensors();
	void read(unsigned int *sensor_values, unsigned char readMode=EMITTERS_ON);
    void emittersOff();
    void emittersOn();
    void calibrate(unsigned char readMode=EMITTERS_ON);
    void resetCalibration();
    void readCalibrated(unsigned int *sensor_values, unsigned char readMode=EMITTERS_ON);
	int readLine(unsigned int *sensor_values, unsigned char readMode=EMITTERS_ON, unsigned char white_line=0);

    unsigned int *calibratedMinimumOn;
    unsigned int *calibratedMaximumOn;
    unsigned int *calibratedMinimumOff;
    unsigned int *calibratedMaximumOff;

protected:
	QTRSensors();
	void init(unsigned char *pins, unsigned char numSensors, unsigned char emitterPin);

    RPiTime *_time;
    RPiGPIO *_io;
    unsigned char *_pins;
    unsigned char _numSensors;
    unsigned char _emitterPin;
    unsigned int _maxValue;
    int _lastValue;


private:
    virtual void readPrivate(unsigned int *sensor_values)=0;
    void calibrateOnOrOff(unsigned int **calibratedMinimum,
                          unsigned int **calibratedMaximum,
                          unsigned char readMode);
};

class QTRSensorsRC : public QTRSensors {
public:
	QTRSensorsRC();
	QTRSensorsRC(unsigned char* pins, unsigned char numSensors,
          unsigned int timeout=4000, unsigned char emitterPin=NO_EMITTER_PIN);
	void init(unsigned char* pins, unsigned char numSensors,
          unsigned int timeout=2000, unsigned char emitterPin=NO_EMITTER_PIN);

private:
	void readPrivate(unsigned int *sensor_values);
};

#endif