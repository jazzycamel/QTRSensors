#ifndef QTRSensors_h
#define QTRSensors_h

class QTRSensorsRC {
  public:
    enum {
      QTR_EMITTERS_OFF=0,
      QTR_EMITTERS_ON=1,
      QTR_EMITTERS_ON_AND_OFF=2,
      QTR_NO_EMITTER_PIN=255,
      QTR_MAX_SENSORS=16,
    };

    QTRSensorsRC(unsigned char* pins, unsigned char numSensors,
          unsigned int timeout=4000, unsigned char emitterPin=QTR_NO_EMITTER_PIN);
    ~QTRSensorsRC();

    void init(unsigned char* pins, unsigned char numSensors,
          unsigned int timeout=2000, unsigned char emitterPin=QTR_NO_EMITTER_PIN);
    void read(unsigned int *sensor_values, unsigned char readMode=QTR_EMITTERS_ON);
    void emittersOff();
    void emittersOn();
    void calibrate(unsigned char readMode=QTR_EMITTERS_ON);
    void resetCalibration();
    void readCalibrated(unsigned int *sensor_values, unsigned char readMode=QTR_EMITTERS_ON);
    int readLine(unsigned int *sensor_values, 
          unsigned char readMode=QTR_EMITTERS_ON, unsigned char white_line=0);

    unsigned int *calibratedMinimumOn;
    unsigned int *calibratedMaximumOn;
    unsigned int *calibratedMinimumOff;
    unsigned int *calibratedMaximumOff;

  private:    
    void calibrateOnOrOff(unsigned int **calibratedMinimum,
          unsigned int **calibratedMaximum, unsigned char readMode);
    void readPrivate(unsigned int *sensor_values);

    unsigned char *_pins;
    unsigned char _numSensors;
    unsigned char _emitterPin;
    unsigned int _maxValue;
    int _lastValue;
};

#endif