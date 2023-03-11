#!/bin/bash
# copy prog over to another computer


USER=rritu
SERVER="192.168.1.58"

scp -r "../Kagami" "$USER@$SERVER:~/Downloads/Kagami"
