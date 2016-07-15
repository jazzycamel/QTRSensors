#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using namespace boost::python;

#include "QTRSensors.h"
#include <wiringPi.h>

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

    //static unsigned int readLine(QTRSensorsRC &qtrrc, list sensorValues){
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

    static void read(QTRSensorsRC &qtrrc, list sensorValues, unsigned char readMode=QTRSensorsRC::QTR_EMITTERS_ON){
        unsigned int _sv[QTRSensorsRC::QTR_MAX_SENSORS];
        for(int i=0; i<len(sensorValues); i++)
            _sv[i]=extract<unsigned int>(sensorValues[i]);

        qtrrc.read(_sv, readMode);

        for(int i=0; i<len(sensorValues); i++)
            sensorValues[i]=_sv[i];
    }

    static void readCalibrated(QTRSensorsRC &qtrrc, list sensorValues, unsigned char readMode=QTRSensorsRC::QTR_EMITTERS_ON){
        unsigned int _sv[QTRSensorsRC::QTR_MAX_SENSORS];
        for(int i=0; i<len(sensorValues); i++)
            _sv[i]=extract<unsigned int>(sensorValues[i]);

        qtrrc.readCalibrated(_sv, readMode);

        for(int i=0; i<len(sensorValues); i++)
            sensorValues[i]=_sv[i];
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

BOOST_PYTHON_MODULE(QTRSensors){
    class_<QTRSensorsRC, boost::shared_ptr<QTRSensorsRC> >
       ("QTRSensorsRC", no_init)
       .def("__init__", make_constructor(WrapperFuncs::init))

        // 'Normal' methods
        .def("calibrate", &QTRSensorsRC::calibrate, calibrate_overloads())
        .def("emittersOff", &QTRSensorsRC::emittersOff)
        .def("emittersOn", &QTRSensorsRC::emittersOn)
        .def("resetCalibration", &QTRSensorsRC::resetCalibration)

        // Wrapped methods
        .def("read", &WrapperFuncs::read, read_overloads())
        .def("readCalibrated", &WrapperFuncs::readCalibrated, readCalibrated_overloads())        
        .def("readLine", &WrapperFuncs::readLine, readLine_overloads())
    ;
}
