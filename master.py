from subprocess import Popen, PIPE
from nodes import Raspberry, Console
import sys
import time

def main():
    p = Popen(['python', './control.py'], stdin=PIPE, stdout=PIPE, stderr=PIPE)
    p.stdin.write("/dev/ttyACM0")
    for i in range(3):
        print p.stdout.readline()

if __name__ == "__main__": main()