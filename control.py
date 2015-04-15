# THIS SCRIPT DOES THE FOLLOWING:
#   1. RECEIVE PILOT MESSAGE.
#   2. SPLIT PILOT MESSAGE TO RIGHT MESSAGE AND LEFT MESSAGE (AND MAYBE SERVO MESSAGE).
#   3. OPEN TWO SERIAL PORTS : RIGHT ARDUINO , LEFT ARDUINO (AND MAYBE ARDUINO NANO).
#   4. SEND CORRESPONDING MESSAGES TO CORRESPONDING SERIAL PORTS.
#   5. RECEIVE SENSOR READINGS FROM EACH ARDUINO PORT.
#   6. MERGE SENSOR READINGS IN ONE MESSAGE.
#   7. SEND SENSOR READINGS MESSAGE TO DASHBOARD.
import threading
import time
import serial.tools.list_ports
from nodes import Arduino, Raspberry, Console


class Sensors(threading.Thread):
    def __init__(self, arduino):
        threading.Thread.__init__(self)
        self.arduino = arduino[0]

    def run(self):
        while True:
            if self.arduino.inWaiting() > 0:
                r = self.arduino.get()
                # print "%s : %s" %(self.name, r)     # CODE TO PASS THE MESSAGE TO THE ROOT THREAD.



class Side(threading.Thread):
    def __init__(self, port, rpi, lock, wlock, connected, rate=115200):
        threading.Thread.__init__(self)
        self.rpi = rpi[0]
        self.seq = []
        self.lock = lock
        self.wlock = wlock
        self.connected = connected
        self.arduino = Arduino(port, rate)
        self.sensors = Sensors([self.arduino])  # SENSOR READING THREAD.
        time.sleep(2)   # THIS IS AN IMPORTANT STEP.



    def makemessage(self, data, seq, msg):
        if data[0] == "PILOT":
                for i in seq:
                    msg.append(data[i])

    def main(self, seq):
        self.connected.append(True)
        ardmsg = []
        data = []
        self.sensors.start()
        while True:
            # ERRORS EXPECTED. INJECT LOCKING MECHANISM ON SHARED RESOURCES.
            self.lock.acquire()
            data = self.rpi.data[:]
            # print "DATA LOCKED BY %s : %s" %(self.name, str(data))
            self.lock.release()
            if len(data) >= len(self.seq):
                self.makemessage(data, seq, ardmsg)
            if self.arduino.isOpen():
                self.wlock.acquire()
                print "%s : %s" %(self.name, str("*"+",".join(ardmsg)))
                x = self.arduino.putList(ardmsg)    # Here is x, the number of characters sent.
                self.wlock.release()
            else :
                return -1
            ardmsg = []

    def run(self):
        while True:
            chk = self.arduino.initConn()
            print "CHECK =", chk
            rtrn = 0
            if chk == "RIGHT":
                # RIGHT sequence.
                self.seq = [1,    # Xspeed
                            2,    # Rspeed
                            4,    # Zspeed
                            3,    # Yspeed
                            9]    # Turbo
                rtrn = self.main(self.seq)
            elif chk == "LEFT":
                # LEFT sequence.
                self.seq = [1,    # Xspeed
                            2,    # Rspeed
                            4,    # Zspeed
                            6,    # Arm
                            9]    # Turbo
                rtrn = self.main(self.seq)
            if(rtrn == -1):
                continue

def main():
    rpi = Raspberry(5000, "127.0.0.2")
    connectedFlag = []
    datalock = threading.RLock()
    writelock = threading.Lock()
    usbports = list(serial.tools.list_ports.comports())
    arduinos = []

    for arduino in usbports:
        connectedFlag.append([])
        arduinos.append(Side(arduino[0], [rpi], datalock,writelock, connectedFlag[-1]))
    print "THERE ARE %d ARDUINOS OPEN" %len(arduinos)
    for arduino in arduinos:
        arduino.start()

    while not all(connectedFlag): pass
    while True:
        # Receive joyStick movements from pilot
        datalock.acquire()
        # print "DATA LOCKED BY %s" %threading.current_thread().name
        try:
            data = rpi.receive()
            # print "RASPBERRY : %s" %str(data)
        except Exception as e:
            print "Error receiving from PilotConsole", e.message
        datalock.release()
    dashboard = Console("192.168.1.10", 8001)
        # CODE TO MERGE THE SENSOR READINGS.
        # CODE TO SEND MERGED MESSAGE TO DASHBOARD.

if __name__ == "__main__":
    main()