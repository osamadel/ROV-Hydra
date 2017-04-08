# In this commit
# We set the Arduino to send 0 in order to run main() (see line 86).
# We set the arduino code to send the message it receives.
# The goal is to send a message and gurantee that it reaches
# the arduino without distortion.
# #################################################################################
# IMPORTANT: chaos flushes causes the message to be distorted, which is logical !
# #################################################################################
from nodes import Arduino, Raspberry, Console
import threading
import time
import sys
# NOTE : Try receive the arduino number as arg (0, 1, 2)

def reading(arduino):
    ard = arduino[0]
    # port 8001 receives left sensors
    dashboard = Console("192.168.1.10",8001)
    while True:
        if ard.inWaiting() > 0:
            r = ard.get()
            print r
            dashboard.send(r)

def makemessage(data,seq,ardmsg):
    if data[0] == "PILOT":
            for i in seq:
                ardmsg.append(data[i])

def side(msgseq, port, arduino):
    ard = arduino[0]
    rpi = Raspberry(port,"127.0.0.2")
    re = threading.Thread(target=reading, args=([ard],))
    ardmsg = []
    data = []

    re.start()
    while True:
        #Receive joyStick movements from pilot
        try:
            data = rpi.receive()
        except Exception as e:
            print "Error receiving from PilotConsole"
        makemessage(data,msgseq,ardmsg)
        if ard.isOpen():
            x = ard.putList(ardmsg)    # Here is x, the number of characters sent.
        else :
            return -1
        ardmsg = []



def main():
    ports = []
    if sys.platform.startswith("win"):
        import serial.tools.list_ports
        ports = list(serial.tools.list_ports.comports())
        for i in range(len(ports)): ports[i] = ports[0][0]
    elif sys.platform.startswith("linux"):
        import glob
        ports = glob.glob("/dev/ttyACM*")
    print ports
    # Each main script must have a different index to communicate with
    # different serial port.
    ard = Arduino(ports[sys.argv[0]])  # Must be put before while True not to open the same port twice.
    time.sleep(2)	# This line solved a problem .. it is important.
    while True:
        chk = ard.initConn()
        print "CHECK =", chk
        rtrn = 0
        if chk == "RIGHT":
            port = 5000
            msgseq = [1,    # Xspeed
                      2,    # Rspeed
                      4,    # Zspeed
                      3,    # Yspeed
                      9]    # Turbo
            rtrn = side(msgseq,port, [ard])
        elif chk == "LEFT":
            port = 5001
            msgseq = [1,    # Xspeed
                      2,    # Rspeed
                      4,    # Zspeed
                      6,    # Arm
                      9]    # Turbo
            rtrn = side(msgseq,port, [ard])
        if(rtrn == -1):
            continue
if __name__ == "__main__":
    main()