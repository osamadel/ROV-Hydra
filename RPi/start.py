#! /usr/bin/env python
import subprocess
from nodes import Console
import threading
import time
import socket

# Copilot.
copilot = Console("192.168.1.11", 11000)

# Connection verification thread method.
def piconn():
	try:
		while True:
			copilot.send("status,Raspberry connected")
			time.sleep(5)
	except:
		raise
		return

# Declare the thread.
#rpiConn = threading.Thread(target=piconn)
# Start the thread.
#rpiConn.start()
lcam = subprocess.Popen('mjpg_streamer -b -i "/usr/local/lib/input_uvc.so -f 25 -d /dev/video0 -r 1280*720" -o "/usr/local/lib/output_http.so -w /usr/local/www -p 8080"', shell=True)
rcam = subprocess.Popen('mjpg_streamer -b -i "/usr/local/lib/input_uvc.so -f 25 -d /dev/video1 -r 1280*720" -o "/usr/local/lib/output_http.so -w /usr/local/www -p 8081"', shell=True)
#time.sleep(5)
rard = subprocess.Popen(['sudo', 'python', '/home/pi/main.py', '0', '&'], shell=False)
#time.sleep(5)
lard = subprocess.Popen(['sudo', 'python', '/home/pi/main.py', '1', '&'], shell=False)
#time.sleep(5)
serv = subprocess.Popen(['sudo', 'python', '/home/pi/servo.py', '&'], shell=False)

# Request dictionary that map the symbol received to the corresponding shell signal.
req = {
       "mS" : "sudo python /home/pi/main.py &",
       "mT" : "sudo pkill -f main.py",
       "mR" : "sudo kill " + str(rard.pid) + " & sudo python /home/pi/control_files/main.py &",
       "m2S" : "sudo python /home/pi/main2.py",
       "m2T" : "sudo pkill -f main2.py",
       "m2R" : "sudo kill " + str(lard.pid) + " & sudo python /home/pi/main2.py & ",
       "sS" : "sudo python /home/pi/servo2.py",
       "sT" : "sudo pkill -f servo2.py",
       "sR" : "sudo kill " + str(serv.pid) + " & sudo python /home/pi/servo2.py & ",
       "rcS" : 'sudo mjpg_streamer -b -i "/usr/local/lib/input_uvc.so -f 25 -d /dev/video0 -r 1280*720" -o "/usr/local/lib/output_http.so -w /usr/local/www -p 8080"',
       "rcT" : 'sudo kill ' + str(rcam.pid),
       "rcR" : 'sudo kill ' + str(rcam.pid) + ' & sudo mjpg_streamer -b -i "/usr/local/lib/input_uvc.so -f 25 -d /dev/video0 -r 1280*720" -o "/usr/local/lib/output_http.so -w /usr/local/www -p 8080" &',
       "lcS" : 'sudo mjpg_streamer -b -i "/usr/local/lib/input_uvc.so -f 25 -d /dev/video1 -r 1280*720" -o "/usr/local/lib/output_http.so -w /usr/local/www -p 8081"',
       "lcT" : 'sudo kill ' + str(lcam.pid),
       "lcR" : 'sudo kill ' + str(lcam.pid) + ' & sudo mjpg_streamer -b -i "/usr/local/lib/input_uvc.so -f 25 -d /dev/video1 -r 1280*720" -o "/usr/local/lib/output_http.so -w /usr/local/www -p 8081" &',
       "KAP" : "sudo killall python",
       "KAC" : "sudo killall mjpg_streamer",
       "SA" : "sudo /home/pi/./start.py",
       "SD" : "sudo halt",
       "KA" : "sudo killall python & sudo killall mjpg_streamer"
       }


host = '192.168.1.6'
port = 8001
srv = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
conn =socket.socket()
srv.bind((host,port))
try:
    while True:
        print "Listenning .."
        try:
            srv.listen(1)
        except Exception as e:
            print "Error listening (Line) :", e.message
        try:
            conn , addr = srv.accept() #connection - blocking call
        except Exception as e:
            print "Error accepting TCP connection (Line ) :", e.message
        try:
            msg = conn.recv(1024)      # Normally 1024, 20 fast response
        except Exception as e:
            print "Error receiving TCP package (Line ) :", e.message
        msg = msg.strip("\n")
        msglen = len(msg)
        if(msglen<=3):
            ret = subprocess.Popen(req[msg].split(),shell=False)
            print req[msg].split()
            if msg == "mS": rard = ret.pid
            elif msg == "m2S": lard = ret.pid
            elif msg == "sS": serv = ret.pid
#        print req[msg]
        else:
            ret = subprocess.Popen(msg,shell=True)
#        print msg
        conn.close()
except KeyboardInterrupt as e:
    print "Keyboard Interrupt at Start.py"
except Exception as e:
    print "Problem in start.py :", e.message





