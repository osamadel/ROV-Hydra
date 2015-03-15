__author__ = 'OSAMA'
# This file tests serial communication with Arduino UNO
# when sending a message of data very quickly; Also,
# examine the initialization flag.

# Imports
import serial
import time
import threading
import socket

def main():
    # All the main code resides here.
    def read():
        try:
            while True:
                if s.inWaiting() > 0:
                    print s.readline().strip('\n\r')
        except KeyboardInterrupt:
            s.close()


    comport = "COM5"
    baudrate = 9600
    # timeout = 0.1
    # writetimeout = 0.1

    s = serial.Serial(comport,baudrate)
    readthread = threading.Thread(target=read)
    sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
    sock.bind(("localhost",5000))


    while not s.isOpen():
        s.open()
    else:
        print "Serial Port is open"
        time.sleep(2)
        message = "*255,0,-255,0,1$"
        readthread.start()
        try:
            while True:
                r = sock.recvfrom(1024)
                s.write(message)
                # print "sent:", message

        except KeyboardInterrupt:
            s.close()

if __name__ == "__main__":
    main()