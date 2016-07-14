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
        unsigned char __pins[QTRSensors::QTR_MAX_SENSORS];
        for(int i=0; i<len(_pins); i++)
            __pins[i]=extract<int>(_pins[i]);
        return boost::shared_ptr<QTRSensorsRC>(
            new QTRSensorsRC(__pins, (unsigned char)len(_pins), timeout, emitterPin) 
        );
    }

    static unsigned int readLine(QTRSensorsRC &qtrrc, list sensorValues){
        unsigned int _sv[QTRSensors::QTR_MAX_SENSORS];
        for(int i=0; i<len(sensorValues); i++)
            _sv[i]=extract<unsigned int>(sensorValues[i]);

        int result=qtrrc.readLine(_sv);

        for(int i=0; i<len(sensorValues); i++)
            sensorValues[i]=_sv[i];
        return result;
    }

    static void read(QTRSensorsRC &qtrrc, list sensorValues, unsigned char readMode=QTR_EMITTERS_ON){
        unsigned int _sv[QTRSensors::QTR_MAX_SENSORS];
        for(int i=0; i<len(sensorValues); i++)
            _sv[i]=extract<unsigned int>(sensorValues[i]);

        qtrrc.read(_sv, readMode);

        for(int i=0; i<len(sensorValues); i++)
            sensorValues[i]=_sv[i];
    }

    static void readCalibrated(QTRSensorsRC &qtrrc, list sensorValues, unsigned char readMode=QTR_EMITTERS_ON){
        unsigned int _sv[QTRSensors::QTR_MAX_SENSORS];
        for(int i=0; i<len(sensorValues); i++)
            _sv[i]=extract<unsigned int>(sensorValues[i]);

        qtrrc.readCalibrated(_sv, readMode);

        for(int i=0; i<len(sensorValues); i++)
            sensorValues[i]=_sv[i];
    }    
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(
    calibrate_overloads, QTRSensors::calibrate, 0, 1
)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(
    read_overloads, WrapperFuncs::read, 1, 2
)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(
    readCalibrated_overloads, WrapperFuncs::readCalibrated, 1, 2
)

BOOST_PYTHON_MODULE(QTRSensors){
    class_<QTRSensors>
        ("QTRSensors", no_init)

        // 'Normal' methods
        .def("calibrate", &QTRSensors::calibrate, calibrate_overloads())
        .def("emittersOff", &QTRSensors::emittersOff)
        .def("emittersOn", &QTRSensors::emittersOn)
        .def("resetCalibration", &QTRSensors::resetCalibration)
    ;

    class_<QTRSensorsRC, bases<QTRSensors>, boost::shared_ptr<QTRSensorsRC> >
        ("QTRSensorsRC", no_init)
        .def("__init__", make_constructor(WrapperFuncs::init))

        // Wrapped methods
        .def("read", &WrapperFuncs::read, read_overloads())
        .def("readCalibrated", &WrapperFuncs::readCalibrated, readCalibrated_overloads())        
        .def("readLine", &WrapperFuncs::readLine)
    ;
}
