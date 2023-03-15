#!/bin/bash
# copy prog over to another computer


USER=raghav
SERVER="192.168.1.58"

rsync -avrP ~/Desktop/Kagami "envy:~/Desktop/"
