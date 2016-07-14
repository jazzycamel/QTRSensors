#!/bin/bash -e
GPIO_BASE=`cat /proc/iomem | grep gpio | cut -f 1 -d "-"`
if [ "$GPIO_BASE"=="20200000" ]; then
    echo "Raspbery Pi Model 1 or Zero"
else
    echo "Raspberry Pi Model 2 or 3"
fi
echo "Raspberry Pi Peripheral Base: ${GPIO_BASE}"
CFLAGS="-DGPIO_BASE=0x${GPIO_BASE}"

echo "Compile simple test"
g++ -c -O3 test.cpp $CFLAGS
g++ -c -O3 RPi.cpp $CFLAGS
g++ -c -O3 RPiGPIO.cpp $CFLAGS
g++ -c -O3 RPiTime.cpp $CFLAGS
echo "Link simple test"
g++ -O3 test.o RPi.o RPiGPIO.o RPiTime.o -o test
exit

echo "Compile C++ raw test"
g++ -c -O3 raw_test.cpp $CFLAGS
g++ -c -O3 RPiQTRSensors.cpp $CFLAGS
echo "Link C++ raw test"
g++ -O3 raw_test.o RPiQTRSensors.o RPi.o RPiGPIO.o RPiTime.o -o raw_test

exit
echo "Compile C++ test"
g++ -c -O3 qtr_test.cpp $CFLAGS
g++ -c -O3 RPiQTRSensors.cpp $CFLAGS
echo "Link C++ test"
g++ -O3 qtr_test.o RPiQTRSensors.o RPi.o RPiGPIO.o RPiTime.o -o qtr_test

exit
echo "Compile python module..."
g++ -c -O3 RPiQTRSensors-python.cpp -I/usr/include/python2.7 -I/usr/include -fPIC
echo "Link python module..."
g++ -shared -Wl,--export-dynamic \
	RPiQTRSensors.o RPi.o RPiGPIO.o RPiTime.o RPiQTRSensors-python.o \
    -L/usr/lib -lboost_python-py27  \
    -L/usr/lib/python2.7/config -lpython2.7 \
	-o RPiQTRSensors.so
