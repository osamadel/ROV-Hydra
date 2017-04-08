# ROV Hydra Code

ROV Hydra had both software running on both a Surface Console and on a Raspberry Pi 2 embedded on board - and a firmware uploaded to 3 Arduino  boards on board.

The files are in a mess, sorry. I'll try to put them into order very soon.

Raspberry Pi 2 Software includes:
- main.py, main2.py
- nodes.py
- servo.pu
- start.py

##main.py
Python script which has receives TCP packages and send and receive serial messages without any losses.

###nodes.py
Python module that contains Arduino, Raspberry and Cosnole classes.

###serialcommunication.py
Just a try to test serial communication.

###workspace
Contains all Arduino files.
