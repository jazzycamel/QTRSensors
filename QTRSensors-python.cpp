#include <wiringPi.h>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using namespace boost::python;

#include "QTRSensors.h"

class WrapperFuncs{
public:
    static boost::shared_ptr<QTRSensorsRC> init(
	        list _pins, unsigned int timeout, unsigned char emitterPin){
        wiringPiSetup();
        unsigned char __pins[QTRSensorsRC::QTR_MAX_SENSORS];
        for(int i=0; i<len(_pins); i++)
            __pins[i]=extract<int>(_pins[i]);
        return boost::shared_ptr<QTRSensorsRC>(
            new QTRSensorsRC(__pins, (unsigned char)len(_pins), timeout, emitterPin) 
        );
    }

    static unsigned int readLine(QTRSensorsRC &qtrrc, list sensorValues,
            unsigned char readMode=QTRSensorsRC::QTR_EMITTERS_ON, unsigned char white_line=0){
        unsigned int _sv[QTRSensorsRC::QTR_MAX_SENSORS];
        for(int i=0; i<len(sensorValues); i++)
            _sv[i]=extract<unsigned int>(sensorValues[i]);

        int result=qtrrc.readLine(_sv, readMode, white_line);

        for(int i=0; i<len(sensorValues); i++)
            sensorValues[i]=_sv[i];
        return result;
    }

    static void read(QTRSensorsRC &qtrrc, list sensorValues,
            unsigned char readMode=QTRSensorsRC::QTR_EMITTERS_ON){
        unsigned int _sv[QTRSensorsRC::QTR_MAX_SENSORS];
        for(int i=0; i<len(sensorValues); i++)
            _sv[i]=extract<unsigned int>(sensorValues[i]);

        qtrrc.read(_sv, readMode);

        for(int i=0; i<len(sensorValues); i++)
            sensorValues[i]=_sv[i];
    }

    static void readCalibrated(QTRSensorsRC &qtrrc, list sensorValues,
            unsigned char readMode=QTRSensorsRC::QTR_EMITTERS_ON){
        unsigned int _sv[QTRSensorsRC::QTR_MAX_SENSORS];
        for(int i=0; i<len(sensorValues); i++)
            _sv[i]=extract<unsigned int>(sensorValues[i]);

        qtrrc.readCalibrated(_sv, readMode);

        for(int i=0; i<len(sensorValues); i++)
            sensorValues[i]=_sv[i];
    }

    static list calibratedMinimumOn(QTRSensorsRC& qtrrc){
        list values;
        for(unsigned char i=0; i<qtrrc.numSensors(); i++)
            values.append(qtrrc.calibratedMinimumOn[i]);
        return values;
    }

    static list calibratedMaximumOn(QTRSensorsRC& qtrrc){
        list values;
        for(unsigned char i=0; i<qtrrc.numSensors(); i++)
            values.append(qtrrc.calibratedMaximumOn[i]);
        return values;
    }

    static list calibratedMinimumOff(QTRSensorsRC& qtrrc){
        list values;
        for(unsigned char i=0; i<qtrrc.numSensors(); i++)
            values.append(qtrrc.calibratedMinimumOff[i]);
        return values;
    }

    static list calibratedMaximumOff(QTRSensorsRC& qtrrc){
        list values;
        for(unsigned char i=0; i<qtrrc.numSensors(); i++)
            values.append(qtrrc.calibratedMaximumOff[i]);
        return values;
    }
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(
    calibrate_overloads, QTRSensorsRC::calibrate, 0, 1
)

BOOST_PYTHON_FUNCTION_OVERLOADS(
    read_overloads, WrapperFuncs::read, 2, 3
)

BOOST_PYTHON_FUNCTION_OVERLOADS(
    readCalibrated_overloads, WrapperFuncs::readCalibrated, 2, 3
)

BOOST_PYTHON_FUNCTION_OVERLOADS(
    readLine_overloads, WrapperFuncs::readLine, 2, 4
)

const char *classDocStr=""
    "This library is for using Pololu QTR reflectance\n"
    "sensors and reflectance sensor arrays: QTR-1RC and\n"
    "QTR-8RC. Simply specify in the constructor which\n"
    "Raspberry Pi pins are connected to a QTR sensor, and the read() method\n"
    "will obtain reflectance measurements for those sensors.  Smaller sensor\n"
    "values correspond to higher reflectance (e.g. white) while larger\n"
    "sensor values correspond to lower reflectance (e.g. black or a void).\n\n"
    "* QTRSensorsRC should be used for QTR-1RC and QTR-8RC sensors.\n"
;

const char *initDocStr=""
    "The list 'pins' contains the Raspberry Pi pin number for each sensor.\n"
    "This library uses the pin number convention defined by WiringPi which \n"
    "can be found at http://wiringpi.com/pins/.\n\n"

    "'timeout' specifies the length of time in microseconds beyond\n"
    "which you consider the sensor reading completely black.  That is to say,\n"
    "if the pulse length for a pin exceeds 'timeout', pulse timing will stop\n"
    "and the reading for that pin will be considered full black.\n"
    "It is recommended that you set timeout to be between 1000 and\n"
    "3000 us, depending on things like the height of your sensors and\n"
    "ambient lighting.  Using timeout allows you to shorten the\n"
    "duration of a sensor-reading cycle while still maintaining\n"
    "useful analog measurements of reflectance.\n\n"

    "'emitterPin' is the Raspberry Pi pin that controls the IR LEDs on the 8RC\n"
    "modules.  If you are using a 1RC (i.e. if there is no emitter pin),\n"
    "or if you just want the emitters on all the time and don't want to\n"
    "use an I/O pin to control it, use a value of 255 (QTR_NO_EMITTER_PIN).\n"
;

const char *calibrateDocStr=""
    "Reads the sensors for calibration.  The sensor values are\n"
    "not returned; instead, the maximum and minimum values found\n"
    "over time are stored internally and used for the\n"
    "readCalibrated() method.\n\n"
    "'readMode' determines if the IR LEDs should be on\n"
    "(QTRSensorsRC.QTR_EMITTERS_ON, default) during calibration or off\n"
    "(QTRSensorsRC.QTR_EMITTERS_OFF)."
;

const char *emittersOnDocStr=""
    "Turn the IR LEDs on. This is mainly for use by the\n"
    "read method, and calling this function before or\n"
    "after reading the sensors will have no effect on the\n"
    "readings, but you may wish to use it for testing purposes.\n"
;

const char *emittersoffDocStr=""
    "Turn the IR LEDs off. This is mainly for use by the\n"
    "read method, and calling this function before or\n"
    "after reading the sensors will have no effect on the\n"
    "readings, but you may wish to use it for testing purposes.\n"
;

const char *resetCalibrationDocStr="Resets all calibration that has been done.\n";

const char *readDocStr=""
    "Reads the sensor values into a list.\n\n"
    "Example usage:\n"
    "\tsensor_values=[0,0,0,0,0,0,0,0]\n"
    "\tsensors.read(sensor_values)\n\n"
    "The values returned are a measure of the reflectance in abstract units,\n"
    "with higher values corresponding to lower reflectance (e.g. a black\n"
    "surface or a void).\n"
    "If 'readMode' is QTRSensorsRC.QTR_EMITTERS_ON_AND_OFF, measures the values\n"
    "with the emitters on AND off and returns on - (timeout - off). If this\n"
    "value is less than zero, it returns zero.\n"
    "This method will call readPrivate().\n"
;

const char *readCalibratedDocStr=""
    "Returns values calibrated to a value between 0 and 1000, where\n"
    "0 corresponds to the minimum value read by calibrate() and 1000\n"
    "corresponds to the maximum value.  Calibration values are\n"
    "stored separately for each sensor, so that differences in the\n"
    "sensors are accounted for automatically.\n"
;

const char *readLineDocStr=""
    "Operates the same as read calibrated, but also returns an\n"
    "estimated position of the robot with respect to a line. The\n"
    "estimate is made using a weighted average of the sensor indices\n"
    "multiplied by 1000, so that a return value of 0 indicates that\n"
    "the line is directly below sensor 0, a return value of 1000\n"
    "indicates that the line is directly below sensor 1, 2000\n"
    "indicates that it's below sensor 2000, etc.  Intermediate\n"
    "values indicate that the line is between two sensors.  The\n"
    "formula is:\n\n"
    
    "   0*value0 + 1000*value1 + 2000*value2 + ...\n"
    "  --------------------------------------------\n"
    "        value0  +  value1  +  value2 + ...\n\n"
    
    "By default, this function assumes a dark line (high values)\n"
    "surrounded by white (low values).  If your line is light on\n"
    "black, set the optional third argument white_line to True.  In\n"
    "this case, each sensor value will be replaced by (1000-value)\n"
    "before the averaging.\n"
;

const char *calibratedMinimumDocStr=""
    "Calibrated minumum values. These start at 1000 and\n"
    "0, respectively, so that the very first sensor reading will\n"
    "update both of them.\n\n"
    
    "The values are meaningless until calibrate() is called.\n"
    "Depending on the readMode argument to calibrate, only the On or Off values may\n"
    "be allocated, as required.\n\n"
    
    "These variables are made public so that you can use them for\n"
    "your own calculations and do things like performing sanity checking, etc.\n"
;

const char *calibratedMaximumDocStr=""
    "Calibrated maximum values. These start at 1000 and\n"
    "0, respectively, so that the very first sensor reading will\n"
    "update both of them.\n\n"
    
    "The values are meaningless until calibrate() is called.\n"
    "Depending on the readMode argument to calibrate, only the On or Off values may\n"
    "be allocated, as required.\n\n"
    
    "These variables are made public so that you can use them for\n"
    "your own calculations and do things like performing sanity checking, etc.\n"
;

BOOST_PYTHON_MODULE(QTRSensors){
    scope the_scope=class_<QTRSensorsRC, boost::shared_ptr<QTRSensorsRC> >
        ("QTRSensorsRC", classDocStr, no_init)
        .def("__init__", make_constructor(WrapperFuncs::init, default_call_policies(), args("_pins","timeout","emitterPin")), initDocStr)

        // 'Normal' methods
        .def("calibrate", &QTRSensorsRC::calibrate, calibrate_overloads((arg("self"),arg("readeMode")=1), calibrateDocStr))
        .def("emittersOff", &QTRSensorsRC::emittersOff, args("self"), emittersoffDocStr)
        .def("emittersOn", &QTRSensorsRC::emittersOn, args("self"), emittersOnDocStr)
        .def("resetCalibration", &QTRSensorsRC::resetCalibration, args("self"), resetCalibrationDocStr)

        // Wrapped methods
        .def("read", &WrapperFuncs::read, read_overloads((arg("sensorValues"),arg("readMode")=1), readDocStr))
        .def("readCalibrated", &WrapperFuncs::readCalibrated, readCalibrated_overloads((arg("sensorValues"),arg("readMode")=1), readCalibratedDocStr))
        .def("readLine", &WrapperFuncs::readLine, readLine_overloads((arg("sensorValues"),arg("readMode")=1,arg("white_line")=0), readLineDocStr))

        // Getter Methods (wrap exposed array pointers)
        .def("calibratedMinimumOn", &WrapperFuncs::calibratedMinimumOn, calibratedMinimumDocStr)
        .def("calibratedMaximumOn", &WrapperFuncs::calibratedMaximumOn, calibratedMaximumDocStr)
        .def("calibratedMinimumOff", &WrapperFuncs::calibratedMinimumOff, calibratedMinimumDocStr)
        .def("calibratedMaximumOff", &WrapperFuncs::calibratedMaximumOff, calibratedMaximumDocStr)
    ;

    enum_<QTRSensorsRC::QTR>
        ("QTR")
        .value("QTR_EMITTERS_OFF", QTRSensorsRC::QTR_EMITTERS_OFF)
        .value("QTR_EMITTERS_ON", QTRSensorsRC::QTR_EMITTERS_ON)
        .value("QTR_EMITTERS_ON_AND_OFF", QTRSensorsRC::QTR_EMITTERS_ON_AND_OFF)
        .value("QTR_NO_EMITTER_PIN", QTRSensorsRC::QTR_NO_EMITTER_PIN)
        .value("QTR_MAX_SENSORS", QTRSensorsRC::QTR_MAX_SENSORS)
        .export_values()
    ;
}
