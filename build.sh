#!/bin/bash -e
g++ -c -O3 test.cpp
g++ -c -O3 RPi.cpp
g++ -c -O3 RPiGPIO.cpp
g++ -c -O3 RPiTime.cpp
g++ -O3 test.o RPi.o RPiGPIO.o RPiTime.o -o test

g++ -c -O3 qtr_test.cpp
g++ -c -O3 RPiQTRSensors.cpp
g++ -O3 qtr_test.o RPiQTRSensors.o RPi.o RPiGPIO.o RPiTime.o -o qtr_test

# /opt/local/lib libboost_python-mt.dylib
#/opt/local/include/boost python.hpp
        #-I/opt/local/include \
	#-I/System/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7 \

echo "Compile python module..."
g++ -c -O3 RPiQTRSensors-python.cpp \
        -I/usr/include/python2.7 -I/usr/include \
	-fPIC

	#-L /opt/local/lib -lboost_python-mt \
	#-L /System/Library/Frameworks/Python.framework/Versions/2.7/lib -lpython2.7 \

echo "Link python module..."
g++ -shared \
        -Wl,--export-dynamic \
	RPiQTRSensors.o RPi.o RPiGPIO.o RPiTime.o RPiQTRSensors-python.o \
        -L/usr/lib -lboost_python-py27  \
        -L/usr/lib/python2.7/config -lpython2.7 \
	-o RPiQTRSensors.so
