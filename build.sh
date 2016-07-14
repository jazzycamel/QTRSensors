#!/bin/bash -e
g++ -c QTRSensors.cpp
g++ -c qtrrc_test.cpp
g++ -o qtrrc_test qtrrc_test.o QTRSensors.o -lwiringPi
exit

echo "Compile python module..."
g++ -c -O3 RPiQTRSensors-python.cpp -I/usr/include/python2.7 -I/usr/include -fPIC
echo "Link python module..."
g++ -shared -Wl,--export-dynamic \
	RPiQTRSensors.o RPi.o RPiGPIO.o RPiTime.o RPiQTRSensors-python.o \
    -L/usr/lib -lboost_python-py27  \
    -L/usr/lib/python2.7/config -lpython2.7 \
	-o RPiQTRSensors.so
