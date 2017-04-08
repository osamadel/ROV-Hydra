import serial
import threading
import time


class Side(threading.Thread):
    def __init__(self, port, rate=115200, wtout=0.01):
        threading.Thread.__init__(self)
        self.serialport = serial.Serial()
        self.serialport.port = port
        self.serialport.baudrate = 115200
        try:
            self.serialport.open()
        except serial.SerialException as e:
            print "Cannot open %s" % port
        time.sleep(2)


    def run(self):
        if not self.serialport.isOpen(): self.serialport.open()
        else:
            try:
                self.serialport.write('2')
            except serial.SerialException as e:
                pass    # Continue
            print "WROTE TO ARDUINO"
            #time.sleep(0.01)
            if self.serialport.inWaiting() > 0:
                print "READ FROM ARDUINO"
        while True:
            if self.serialport.inWaiting() > 0:
                r = self.serialport.readline().rstrip("\n\r")
                print "%s : %s" %(self.name, r)     # x is the number of chars sent to Arduino.
                            # put to check if the distorted messages are
                            # sent distorted in the first place or not.




def main():
    rightSide = Side("COM7")
    leftSide = Side("COM3")
    rightSide.start()
    leftSide.start()
    rightSide.join()
    leftSide.join()



if __name__ == '__main__':
    main()