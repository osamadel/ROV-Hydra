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
def main():
    # print ("main")
    x = 0
    rpi = Raspberry(5000,"192.168.1.6")
    def reading():
        while True:
            if ard.s.inWaiting() > 0:
                # print "reading .. inWaiting"
                r = ard.s.readline()
                # print "After readline"
                r = r.strip('\r\n')
                print r, x  # x is the number of chars sent to Arduino.
                            # put to check if the distorted messages are
                            # sent distorted in the first place or not.
                # ard.s.flushInput()
                # time.sleep(0.1)

    re = threading.Thread(target=reading)
    ardmsg = []
    data = []
    # ard.flush()
    re.start()
    while True:
    #Receive joyStick movements from pilot
        try:
            data = rpi.receive()
        except Exception as e:
            print ("Nothing to receive")
        # print data
    #Save the msg
        if data[0] == "PILOT":
            ardmsg.append(data[1])
            ardmsg.append(data[2])
            ardmsg.append(data[4])
            ardmsg.append(data[3])
            ardmsg.append(data[11])
            ardmsg.append(data[7])
            ardmsg.append(data[8])
        if ard.s.isOpen():
            x = ard.send(ardmsg)    # Here is x, the number of characters sent.
            # ard.s.flushOutput()     # flush output
        else :
            return -1


        ardmsg = []        




def main2():
    print ("main2")
    rpi = Raspberry(5001,"192.168.1.6")
    ardmsg = []
    data = []
    while True:
    #Receive joyStick movements from pilot
        try:
            data = rpi.receive()
        except Exception as e:
            print ("Nothing to receive")
    #Save the msg
        if data[0] == "PILOT":
            ardmsg.append(data[1])
            ardmsg.append(data[2])
            ardmsg.append(data[4])
            ardmsg.append(data[6])
            ardmsg.append(data[11]) 
            ardmsg.append(data[8]) 
            ardmsg.append(data[7])
        if ard.s.isOpen():
            ard.send(ardmsg)
            time.sleep(0.01)
            ard.flush()
        else :
            return -1
        ardmsg = []        

if __name__ == "__main__":
    ard = Arduino("/dev/ttyACM0")   # Must be put before while True not to open the same port twice.
    # ard.s.baudrate = 112
    global x
    x = 0
    while True:
        time.sleep(2)	# This line solved a problem .. it is important.
        chk = "-1"
        if not ard.s.isOpen():
            ard.s.open()
        else:
#            chk = ard.s.write("255")
            ard.s.write('2')
            print chk
        print "Written"
        if ard.s.inWaiting() > 0:
            chk = ard.s.readline()
            print "Read"
            chk = chk.strip('\n\r')
            rtrn = 0
            if chk == "2":
                rtrn = main()
            elif chk == "1":
                rtrn = main2()
            if(rtrn == -1):
                continue
        
            
