from __future__ import print_function
from sys import stdout
from time import sleep
from QTRSensors import QTRSensorsRC

qtrrc=QTRSensorsRC([1,2,3,4,5,12,13,6], 2500, 0)
sleep(.5)
print("Calibrating",end='')
for i in range(400):
    if i%10==0: 
        print('.',end='')
        stdout.flush()
    qtrrc.calibrate()
print("Finished")

for v in qtrrc.calibratedMinimumOn():
    print(v,end='\t')
print()

sv=[0]*8
while(1):
    pos=qtrrc.readLine(sv)
    print(sv,pos)
    sleep(1)
