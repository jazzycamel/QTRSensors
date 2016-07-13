#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using namespace boost::python;

#include "RPiQTRSensors.h"

class WrapperFuncs{
public:
	static boost::shared_ptr<QTRSensorsRC> init(
		 	list _pins,
                        unsigned int timeout,
		 	unsigned char emitterPin
		 )
	{
		unsigned char __pins[QTRSensors::MAX_SENSORS];
		for(int i=0; i<len(_pins); i++)
                    __pins[i]=extract<int>(_pins[i]);
		return boost::shared_ptr<QTRSensorsRC>( new QTRSensorsRC(__pins, (unsigned char)len(_pins), timeout, emitterPin) );
	}

	static unsigned int readLine(QTRSensorsRC &qtrrc, list sensorValues){
		unsigned int _sv[QTRSensors::MAX_SENSORS];
		for(int i=0; i<len(sensorValues); i++)
			_sv[i]=extract<unsigned int>(sensorValues[i]);
        	int result=qtrrc.readLine(_sv);
		for(int i=0; i<len(sensorValues); i++)
			sensorValues[i]=_sv[i];
                return result;
	}
};



BOOST_PYTHON_MODULE(RPiQTRSensors){
	//class_<QTRSensorsRC>("QTRSensorsRC", init<unsigned char*, unsigned char, unsigned int, unsigned char>())
	//	.def("calibrate", &QTRSensorsRC::calibrate)
	//	.def("readLine", &QTRSensorsRC::readLine)
	//	;
	class_<QTRSensorsRC, boost::shared_ptr<QTRSensorsRC> >
		("QTRSensorsRC", no_init)
		.def("__init__", make_constructor(WrapperFuncs::init))
		.def("calibrate", &QTRSensorsRC::calibrate)
		.def("readLine", &WrapperFuncs::readLine)
	;
}
