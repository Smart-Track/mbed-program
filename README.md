# mbed-program
This repo contains the code used on the mbed for the Smart Track system. The mbed sends an interrupt to the connected device running the GUI for checkpoints passed and controls lights/sounds on the track. The hardware used includes sonars, audio amps with speakers, and LEDs.

It waits for a start character from the GUI before a countdown sequence starts. During the countdown, the program checks if the racer passes the chechpoint preemptively and responds by disqualifying them with lights and sound. During the race, sonars will detect whether a checkpoint is passed and updates the program acordingly. After the race is done, the mbed waits for a reset character to start another race.
