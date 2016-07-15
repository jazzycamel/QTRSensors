#ifndef QTRSensors_h
#define QTRSensors_h

class QTRSensors {
  public:
    enum {
      QTR_EMITTERS_OFF=0,
      QTR_EMITTERS_ON=1,
      QTR_EMITTERS_ON_AND_OFF=2,
      QTR_NO_EMITTER_PIN=255,
      QTR_MAX_SENSORS=16,
    };

    void read(unsigned int *sensor_values, unsigned char readMode=QTR_EMITTERS_ON);
    void emittersOff();
    void emittersOn();
    void calibrate(unsigned char readMode=QTR_EMITTERS_ON);
    void resetCalibration();
    void readCalibrated(unsigned int *sensor_values, unsigned char readMode=QTR_EMITTERS_ON);
    int readLine(unsigned int *sensor_values, unsigned char readMode=QTR_EMITTERS_ON, unsigned char white_line=0);
    unsigned int *calibratedMinimumOn;
    unsigned int *calibratedMaximumOn;
    unsigned int *calibratedMinimumOff;
    unsigned int *calibratedMaximumOff;

    ~QTRSensors();

    //
    QTRSensors(unsigned char* pins, unsigned char numSensors,
          unsigned int timeout=4000, unsigned char emitterPin=QTR_NO_EMITTER_PIN);

    void init(unsigned char* pins, unsigned char numSensors,
          unsigned int timeout=2000, unsigned char emitterPin=QTR_NO_EMITTER_PIN);
    //

  protected:

    //QTRSensors(){};
    //void init(unsigned char *pins, unsigned char numSensors, unsigned char emitterPin);

    unsigned char *_pins;
    unsigned char _numSensors;
    unsigned char _emitterPin;
    unsigned int _maxValue; // the maximum value returned by this function
    int _lastValue;

  private:
    //virtual void readPrivate(unsigned int *sensor_values){};
    void calibrateOnOrOff(unsigned int **calibratedMinimum,
                          unsigned int **calibratedMaximum,
                          unsigned char readMode);

    //
    void readPrivate(unsigned int *sensor_values);
    //
};

// class QTRSensorsRC : public QTRSensors
// {
//   public:
//     QTRSensorsRC();
//     QTRSensorsRC(unsigned char* pins, unsigned char numSensors,
//           unsigned int timeout=4000, unsigned char emitterPin=QTR_NO_EMITTER_PIN);

//     void init(unsigned char* pins, unsigned char numSensors,
//           unsigned int timeout=2000, unsigned char emitterPin=QTR_NO_EMITTER_PIN);

//   private:
//     void readPrivate(unsigned int *sensor_values);
// };
#endif
