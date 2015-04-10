from nodes import Raspberry, Arduino
#import subprocess
import spidev
import time
def main():
#    e = subprocess.call("sudo pigpiod", shell=True)
#    import pigpio
    import RPi.GPIO as GPIO
    nano = Arduino("/dev/ttyAMA0")
    #spi = spidev.SpiDev()
    #spi.open(0,0)
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(3, GPIO.OUT)
    GPIO.setup(15, GPIO.OUT)
    rpi = Raspberry(5002,"192.168.1.6")
#    pi = pigpio.pi()
    angle = 90
 #   pi.set_servo_pulsewidth(17,angle)
    print "Servo Program initiated .."
    try:
        print "Waiting for data .."
        while True:
            data = rpi.receive()
#            print "Just received %s from Raspberry"%str(data)
            if data[0] == '-1' and angle < 180:
#                print "down"
                angle += 1
                nano.send([0,0,angle])
#                pi.set_servo_pulsewidth(17,angle)
#                time.sleep(0.05)
            elif data[0] == '1' and angle > 0:
#                print "up"
                angle -= 1
                nano.send([0,0,angle])
#                pi.set_servo_pulsewidth(17,angle)
#                time.sleep(0.05)
            else:
                nano.send([0,0,angle])
            if data[2] == '0':
                GPIO.output(3, 0)
            else:
                GPIO.output(3,1)
            if data[1] == '1':
                GPIO.output(15,1)
            else:
                GPIO.output(15, 0)
    except KeyboardInterrupt:
#        pi.stop()
        GPIO.cleanup()
        nano.closeConnection()
    except Exception as e:
#        pi.stop()
        GPIO.cleanup()
        nano.closeConnection()
#        s.stop_servo(17)
        raise
    finally:
        print "------------Abort------------"


if __name__ == "__main__":
    try:
        main()
    except:
        raise
