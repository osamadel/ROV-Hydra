from nodes import Arduino, Raspberry
import threading
import time
def main():
    # print ("main")
    rpi = Raspberry(5000)
    def reading():
        while True:
            if ard.s.inWaiting():
                r = ard.s.readline()
                r = r.strip('\r\n')
                print r
                time.sleep(100)
    re = threading.Thread(target= reading)
    ardmsg = []
    data = []
    re.start()
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
            ardmsg.append(data[3])
            ardmsg.append(data[9])
        if ard.s.isOpen():
            ard.send(ardmsg)
            ard.flush()
        else :
            return -1


        ardmsg = []        




def main2():
    print ("main2")
    rpi = Raspberry(5001)
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
            ardmsg.append(data[9])
        if ard.s.isOpen():
            ard.send(ardmsg)
            ard.flush()
        else :
            return -1
        ardmsg = []        

if __name__ == "__main__":
    while True:
        ard = Arduino("COM")
        chk = "-1"
        if not ard.s.isOpen():
            ard.s.open()
        chk = ard.s.write("2")
        chk = ard.s.readline()
        chk = chk.strip('\n\r')
        rtrn = 0
        if chk == "0":
            rtrn = main()
        elif chk == "1":
            rtrn = main2()
        if(rtrn == -1):
            continue
        
            
