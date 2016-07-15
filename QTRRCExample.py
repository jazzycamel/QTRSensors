# -*- coding: latin-1 -*-
from __future__ import print_function
from sys import stdout
from time import sleep
from QTRSensors import QTRSensorsRC

if __name__=="__main__":
	# Pin numbering follows the wiringPi convention 
	# as detailed here: http://wiringpi.com/pins/
	pins=[1,2,3,4,5,12,13,6] # Pins to be used for sensors 1 thru 8
	timeout=2500             # Waits for 2500µs for snensor output to go low
	emitterPin=0             # Pin to be used to switch on the IR LEDs

	qtrrc=QTRSensorsRC(pins, timeout, emitterPin)
	sleep(.5)

	# Make the calibration take ~10s and print progress
	print("Calibrating",end='')
	for i in range(400):
	    if i%10==0: 
	        print('.',end='')
	        stdout.flush()
	    qtrrc.calibrate() # Read all sensors 10 times at 2500µs per read (~25ms)
	print("Finished")

	# Print out all the calibration data
	print("\nCalibration Data (min/max):")
	for v in qtrrc.calibratedMinimumOn():
	    print(v,end='\t')
	print()

	for v in qtrrc.calibratedMaximumOn():
	    print(v,end='\t')
	print()
	
	#sv=[0]*8 # List to receive the sensor values
	while(1):
		# read calibrated sensor values and obtain a measure of the line position from 0 to 7000	
	    pos,sv=qtrrc.readLine()
	    print(sv,pos)
	    sleep(1)
