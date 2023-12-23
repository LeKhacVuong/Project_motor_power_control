#!/bin/bash
PORT="/dev/ttyACM0"

grabserial -d $PORT -b 9600 -e 5000 -t
