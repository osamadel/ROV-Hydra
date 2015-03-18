__author__ = 'OSAMA'
from nodes import Arduino
def main():

    while True:
        if not ard.s.isOpen():
            return -1

if __name__ == "__main__":
    ard = Arduino("/dev/ttyACM0")
    while True:
        # Init code (flag code)
        x = 0 # Right
        if x == 0:
            y = main()
            if y == -1: continue
