#!/bin/bash -e
echo "Compile C++ module"
g++ -c QTRSensors.cpp
exit
echo "Compile C++ test"
g++ -c qtrrc_test.cpp

echo "Link C++ test"
g++ -o qtrrc_test qtrrc_test.o QTRSensors.o -lwiringPi

echo "Compile python module..."
g++ -c -O3 QTRSensors-python.cpp -I/usr/include/python2.7 -I/usr/include -fPIC

echo "Link python module..."
g++ -shared -Wl,--export-dynamic \
	QTRSensors.o QTRSensors-python.o \
	-l wiringPi \
        -L /usr/lib/arm-linux-gnueabihf -lboost_python-py27 \
        -L/usr/lib/python2.7/config -lpython2.7 \
	-o QTRSensors.so
