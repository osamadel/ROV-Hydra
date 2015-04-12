# In this commit
# We set the Arduino to send 0 in order to run main() (see line 86).
# We set the arduino code to send the message it receives.
# The goal is to send a message and gurantee that it reaches
# the arduino without distortion.
# #################################################################################
# IMPORTANT: chaos flushes causes the message to be distorted, which is logical !
# #################################################################################
from nodes import Arduino, Raspberry
import threading
import time


def reading():
        while True:
            if ard.inWaiting() > 0:
                r = ard.get()
                print r     # x is the number of chars sent to Arduino.
                            # put to check if the distorted messages are
                            # sent distorted in the first place or not.

def makemessage(data,seq,ardmsg):
    if data[0] == "PILOT":
            for i in seq:
                ardmsg.append(data[i])

def main():
    rpi = Raspberry(5000,"127.0.0.2")
    re = threading.Thread(target=reading)
    ardmsg = []
    data = []
    msgseq = [1,    # Xspeed
              2,    # Rspeed
              4,    # Zspeed
              3,    # Yspeed
              9]    # Turbo
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


def main2():
    rpi = Raspberry(5001,"192.168.1.6")
    # re = threading.Thread(target=reading)
    ardmsg = []
    data = []
    msgseq = [1,    # Xspeed
              2,    # Rspeed
              4,    # Zspeed
              6,    # Arm
              9]    # Turbo
    # re.start()
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

if __name__ == "__main__":
    ard = Arduino("COM5")   # Must be put before while True not to open the same port twice.
    time.sleep(2)	# This line solved a problem .. it is important.
    while True:
        chk = ard.initConn()
        print "CHECK =", chk
        rtrn = 0
        if chk == "2":
            rtrn = main()
        elif chk == "1":
            rtrn = main2()
        if(rtrn == -1):
            continue