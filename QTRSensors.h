#ifndef QTRSensors_h
#define QTRSensors_h

#define QTR_EMITTERS_OFF 0
#define QTR_EMITTERS_ON 1
#define QTR_EMITTERS_ON_AND_OFF 2

#define QTR_NO_EMITTER_PIN  255

#define QTR_MAX_SENSORS 16

class QTRSensors
{
  public:


    void read(unsigned int *sensor_values, unsigned char readMode = QTR_EMITTERS_ON);
    void emittersOff();
    void emittersOn();
    void calibrate(unsigned char readMode = QTR_EMITTERS_ON);
    void resetCalibration();
    void readCalibrated(unsigned int *sensor_values, unsigned char readMode = QTR_EMITTERS_ON);
    int readLine(unsigned int *sensor_values, unsigned char readMode = QTR_EMITTERS_ON, unsigned char white_line = 0);
    unsigned int *calibratedMinimumOn;
    unsigned int *calibratedMaximumOn;
    unsigned int *calibratedMinimumOff;
    unsigned int *calibratedMaximumOff;

    ~QTRSensors();

  protected:

    QTRSensors()
    {

    };

    void init(unsigned char *pins, unsigned char numSensors, unsigned char emitterPin);

    unsigned char *_pins;
    unsigned char _numSensors;
    unsigned char _emitterPin;
    unsigned int _maxValue; // the maximum value returned by this function
    int _lastValue;

  private:

    virtual void readPrivate(unsigned int *sensor_values) = 0;
    void calibrateOnOrOff(unsigned int **calibratedMinimum,
                          unsigned int **calibratedMaximum,
                          unsigned char readMode);
};

class QTRSensorsRC : public QTRSensors
{
  public:

    QTRSensorsRC();
    QTRSensorsRC(unsigned char* pins, unsigned char numSensors,
          unsigned int timeout = 4000, unsigned char emitterPin = 255);

    void init(unsigned char* pins, unsigned char numSensors,
          unsigned int timeout = 2000, unsigned char emitterPin = QTR_NO_EMITTER_PIN);

  private:
    void readPrivate(unsigned int *sensor_values);
};
#endif