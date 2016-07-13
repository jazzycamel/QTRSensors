#!/bin/bash -e
echo "Compile simple test"
g++ -c -O3 test.cpp
g++ -c -O3 RPi.cpp
g++ -c -O3 RPiGPIO.cpp
g++ -c -O3 RPiTime.cpp
echo "Link simple test"
g++ -O3 test.o RPi.o RPiGPIO.o RPiTime.o -o test

echo "Compile C++ test"
g++ -c -O3 qtr_test.cpp
g++ -c -O3 RPiQTRSensors.cpp
echo "Link C++ test"
g++ -O3 qtr_test.o RPiQTRSensors.o RPi.o RPiGPIO.o RPiTime.o -o qtr_test

echo "Compile python module..."
g++ -c -O3 RPiQTRSensors-python.cpp -I/usr/include/python2.7 -I/usr/include -fPIC
echo "Link python module..."
g++ -shared -Wl,--export-dynamic \
	RPiQTRSensors.o RPi.o RPiGPIO.o RPiTime.o RPiQTRSensors-python.o \
    -L/usr/lib -lboost_python-py27  \
    -L/usr/lib/python2.7/config -lpython2.7 \
	-o RPiQTRSensors.so
