# Author        :   OSAMA ADEL SHOKRY
# DATE          :   30/1/2015
# DESCRIPTION   :
# Module of classes used to represent all nodes in the network consisting the ROV
#                   control system which are : PilotConsole, ROVSupervision, CoPilotConsole, RaspberryPi,
#                   ArduinoRight, ArduinoLeft.
import serial
import socket
# import RPi.GPIO as GPIO
import time

# Super Class for all nodes in the network.
class Workstation:
    def __init__(self, address=""):
        self.address = address


# Subclass of Workstation.
# Initiates an Arduino device.
# Consructor receives the device name
# setSensorNames must be set first to identify the message components.
# read returns the message sent from Arduino
# send sends the message to Arduino with the approperiate delimeter.
# closeConnection closes the serial port. Must be called at the end of the main program.
class Arduino(Workstation):
    def __init__(self, address=""):
        Workstation.__init__(self, address)
        self.s = serial.Serial(address, baudrate=115200, writeTimeout=0.1)
        self.sensorsKeys = []

    # Decompose message received from arduino
    def decompose(self, message):
        msg = message.lstrip("*/").rstrip("\n").split(",")
        return dict(zip(self.sensorsKeys, msg))

    # Set the sensors data sent from this instance (in order)
    def setSensorNames(self, names):
        if self.sensorsKeys == []:
            for i in names:
                self.sensorsKeys.append(i)

    def flush(self):
        self.s.flushInput()

    def read(self):
        '''msg = ""
        #print "read"
        if self.s.inWaiting() > 0:
#            print "Data in input buffer exists"
            try:
                c = self.s.read(1)
                if c == '*':
            l = self.s.read(1)
            msg = self.s.readline()
            lineIndex = msg.index('\r\n')
            if lineIndex > -1 and len(msg) == l + 2:
                            msg = msg[:lineIndex]
            else:
                msg = -1
            except:
                msg = -1'''
        if self.s.inWaiting() > 0:
            msg = self.s.readline()
        else:
            msg = -1
        #                print "exception in reading from arduino"
        #print "Message Read:", msg
        #self.s.flushInput()
        # return self.decompose(msg)  # Type: Dictionary
        return msg


    def send(self, values):
        m = ",".join(map(str, values))
        message = "*" + chr(len(m)) + m + "$"
        #        print "Print this if we are in send. Message =", message
        try:
            #self.s.write(chr(len(message)-2))	# Send length of message
            self.s.write(message)  # *100,200,300$
        #        	time.sleep(0.01)
        except:
            #		time.sleep(0.01)
            pass


def closeConnection(self):
    self.s.close()


# Subclass of Workstation.
# Represents a computer process connected to the network.
# Used only to send data to this process(program).
# Cannot receive from it. SEE Raspberry Class to receive data.
# Initiates a client process to send data (as UDP packages).
class Console(Workstation):
    def __init__(self, address="", port=""):
        Workstation.__init__(self, address)  # IP address of the console.
        self.port = port  # Port number of the console.


    def send(self, values):
        client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        if type(values) == list:
            message = "*"
            message += ",".join(values)
            message += "$"
            #print "Message sent to console: " + message
            client.sendto(message, (self.address, self.port))
            client.close()
        elif type(values) == str:
            values = "*" + values + "$"
            #print "Message sent to console: " + values
            client.sendto(values, (self.address, self.port))
            client.close()


# Subclass of Workstation.
# Represents the raspberry pi itself.
# Initiates a server process used to retrieve data from multiple consoles (as UDP packages).
class Raspberry(Workstation):
    def __init__(self, port, address=socket.gethostbyname(socket.gethostname())):
        Workstation.__init__(self, address)
        self.port = port
        self.server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.server.bind((address, port))


    def receive(self):
        data, clientIP = self.server.recvfrom(1024)
        data = data.strip("/*").split(",")
        #        print "Message received by RPi:", data
        return data


# Main program for test ONLY.
if __name__ == '__main__':
    # GPIO.setmode(GPIO.BOARD)
    # GPIO.setup(7, GPIO.OUT)
    # p = GPIO.PWM(7,50)
    # p.start(7.5)
    console = Console("192.168.1.200", 5001)
    rpi = Raspberry(5000)
    print "Waiting for connection .."
    try:
        while True:
            data = rpi.receive()
            console.send(data)
            # p.ChangeDutyCycle(7.5)

    except KeyboardInterrupt:
        # p.stop()
        # GPIO.cleanup()
        pass
