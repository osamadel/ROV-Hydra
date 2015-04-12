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
class Arduino(serial.Serial):


    def __init__(self, address=""):
        serial.Serial.__init__(self, port=address, baudrate=115200, writeTimeout=0.1)


    def get(self):
        r = self.readline()
        r = r.rstrip('\n\r')
        return r


    # Used to send a list of values to serial port.
    def putList(self, values):
        m = ""
        if type(values) == list:
            if type(values[0]) == int:
                values = map(str, values)
            m = ",".join(values)
        m = "*" + m + "$"
        try:
            return self.write(m)
        except Exception as e:
            # I am not sure what the error message should be.
            print e.message

    def initConn(self):
        if not self.isOpen(): self.open()
        else:
            try:
                self.write('2')
            except serial.SerialException as e:
                pass    # Continue
            print "WROTE TO ARDUINO"
            #time.sleep(0.01)
            if self.inWaiting() > 0:
                print "READ FROM ARDUINO"
                return self.get()


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
