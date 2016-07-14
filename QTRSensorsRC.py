from time import sleep
from QTRSensors import QTRSensorsRC

qtrrc=QTRSensorsRC([1,2,3,4,5,12,13,6], 2500, 0)
sleep(.5)
print("Calibrating")
for i in range(400):
    qtrrc.calibrate(1)
print("Finished")

sv=[0]*8
while(1):
    pos=qtrrc.readLine(sv)
    print(sv,pos)
    sleep(1)
