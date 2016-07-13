from enum import IntEnum
import logging
from time import sleep, time

try:
    import RPi.GPIO as IO
except ImportError:
    logging.warning("RPi.GPIO not installed, using stub instead.")

    class IO(object):
        IN=0
        OUT=1
        LOW=0
        HIGH=1

        def setup(*args, **kwargs): pass
        def input(*args, **kwargs): return 0
        def output(*args, **kwargs): pass
        def cleanup(): pass

class QTR(IntEnum):
    EMITTERS_OFF        = 0
    EMITTERS_ON         = 1
    EMITTERS_ON_AND_OFF = 2
    NO_EMITTER_PIN      = 255
    MAX_SENSORS         = 16

class QTRSensors(object):
    def __init__(self):
        self._pins=[]
        self._numSensors=0
        self._emitterPin=-1
        self._maxValue=0
        self._lastValue=0

        self.calibratedMinimumOn=[0]*self._numSensors
        self.calibratedMaximumOn=[0]*self._numSensors
        self.calibratedMinimumOff=[0]*self._numSensors
        self.calibratedMaximumOff=[0]*self._numSensors
        self._calibrated=False

    def init(self, pins, emitterPin):
        self._pins=pins
        self._emitterPin=emitterPin
        self._numSensors=len(self._pins)        
        assert self._numSensors<=QTR.MAX_SENSORS

    def _readPrivate(self): raise NotImplementedError

    def _calibrateOnOrOff(self, readMode):
        cMax=[0]*self._numSensors
        cMin=[self._maxValue]*self._numSensors
        svMax=[0]*self._numSensors
        svMin=[self._maxValue]*self._numSensors

        for j in range(10):
            sensorValues=self.read(readMode)
            for i in range(self._numSensors):
                if j==0 or svMax[i]<sensorValues[i]:
                    svMax[i]=sensorValues[i]

                if j==0 or svMin[i]>sensorValues[i]:
                    svMin[i]=sensorValues[i]

        for i in range(self._numSensors):
            if svMin[i]>cMax[i]: cMax[i]=svMin[i]
            if svMax[i]<cMin[i]: cMin[i]=svMax[i]

        return cMin,cMax

    def read(self, readMode=QTR.EMITTERS_ON):
        if readMode in (QTR.EMITTERS_ON, QTR.EMITTERS_ON_AND_OFF):
            self.emittersOn()
        else: self.emittersOff()

        sensorValues=self.readPrivate()
        self.emittersOff()

        if readMode==QTR.EMITTERS_ON_AND_OFF:
            offValues=self.readPrivate()
            for i in range(self._numSensors):
                sensorValues[i]=self._maxValue-offValues[i]

        return sensorValues

    def emittersOff(self):
        if self._emitterPin==QTR.NO_EMITTER_PIN: return

        IO.setup(self._emitterPin, IO.OUT)
        IO.output(self._emitterPin, IO.LOW)
        sleep(200./10e6) # 200 microseconds

    def emittersOn(self):
        if self._emitterPin==QTR.NO_EMITTER_PIN: return

        IO.setup(self._emitterPin, IO.OUT)
        IO.output(self._emitterPin, IO.HIGH)
        sleep(200/10e6) # 200 microseconds

    def calibrate(self, readMode=QTR.EMITTERS_ON):
        if readMode in (QTR.EMITTERS_ON,QTR.EMITTERS_ON_AND_OFF):
            self._calibratedMinimumOn, self._calibratedMaximumOn=self._calibrateOnOrOff(QTR.EMITTERS_ON)
        if readMode in (QTR.EMITTERS_OFF,QTR.EMITTERS_ON_AND_OFF):
            self._calibratedMinimumOff, self._calibratedMaximumOff=self._calibrateOnOrOff(QTR.EMITTERS_OFF)
        self._calibrated=True

    def resetCalibration(self):
        for i in range(self._numSensors):
            self.calibratedMinimumOn[i]=self._maxValue
            self.calibratedMinimumOff[i]=self._maxValue
            self.calibratedMaximumOn[i]=0
            self.calibratedMaximumOff[i]=0

    def readCalibrated(self, readMode=QTR.EMITTERS_ON):
        if not self._calibrated: return

        sensorValues=self.read(readMode)
        for i in range(self._numSensors):
            if readMode==QTR.EMITTERS_ON:
                calMax=self._calibratedMaximumOn[i]
                calMin=self._calibratedMinimumOn[i]
            elif readMode==QTR.EMITTERS_OFF:
                calMax=self._calibratedMaximumOff[i]
                calMin=self._calibratedMinimumOff[i]
            else: # QTR.EMITTERS_ON_AND_OFF
                if self._calibratedMinimumOff[i]<self._calibratedMinimumOn: calMin=self._maxValue
                else: calMin=self._calibratedMinimumOn[i]+self._maxValue-self._calibratedMinimumOff[i]

                if self._calibratedMaximumOff[i]<sef._calibratedMaximumOn[i]: calMax=self._maxValue
                else: calMax=self._calibratedMaximumOn[i]+self._maxValue-self._calibratedMaximumOff[i]

            denominator=calMax-calMin
            x=0
            if denominator!=0:
                x=(sensorValues[i]-calMin)*1000/denominator
            if x<0: x=0
            elif x>1000: x=1000
            sensorValues[i]=x

        return sensorValues

    def readLine(self, readMode=QTR.EMITTERS_ON, whiteLine=0):
        sensorValues=self.readCalibrated(readMode)
        _avg=_sum=_online=0

        for i in range(self._numSensors):
            v=sensorValues[i]
            if whiteLine: v=1000-v
            if v>200: _online=1
            if v>50:
                _avg+=v*i*1000
                _sum+=v

        if not _online:
            if self._lastValue<((self._numSensors-1)*1000/2): return 0
            else: return (self._numSensors-1)*1000

        self._lastValue=_avg/_sum

        return self._lastValue, sensorValues

class QTRSensorsRC(QTRSensors):
    def __init__(self, pins, timeout=4000, emitterPin=QTR.NO_EMITTER_PIN):
        super().__init__()

        self.init(pins, timeout, emitterPin)

    def init(self, pins, timeout, emitterPin):
        super().init(pins, emitterPin)
        self._maxValue=timeout

    def readPrivate(self):
        sensorValues=[]
        for i in range(self._numSensors):
            sensorValues.append(self._maxValue)
            IO.setup(self._pins[i], IO.OUT, initial=IO.HIGH)

        sleep(10./10e6) # 10 microseconds

        for i in range(self._numSensors):
            IO.output(self._pins[i], IO.LOW)
            IO.setup(self._pins[i], IO.IN)

        startTime=time()
        t=0
        while t<self._maxValue:
            t=(time()-startTime)*10e6
            for i in range(self._numSensors):
                if IO.input(self._pins)==IO.LOW and t<sensorValues[i]:
                    sensorValues[i]=t

        return sensorValues

if __name__=="__main__":
    import atexit

    @atexit.register
    def cleanup():
        print("Cleanup", flush=True)
        IO.cleanup()

    qtrrc=QTRSensorsRC([1,2,3,4,5,6,7,8], timeout=2500, emitterPin=9)

    print("Calibrating", end="")
    for i in range(400):
        if i%8==0: print('.',end="", flush=True)
        qtrrc.calibrate()
    print()
    print("readLine:",qtrrc.readLine())