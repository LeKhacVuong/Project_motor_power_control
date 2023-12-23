#!/bin/bash
gnome-terminal -- /bin/bash -c "cv bms; make clean; make ;make debug" & arm-none-eabi-gdb
