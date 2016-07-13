from RPiQTRSensors import QTRSensorsRC

qtrrc=QTRSensorsRC([1,3,2,4,6,5,8,7],2500,1);
qtrrc.calibrate(1)

sv=[0,0,0,0,0,0,0,0]
print("ReadLine:",qtrrc.readLine(sv))
print("SV:",sv)
