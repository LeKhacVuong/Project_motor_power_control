target extended-remote localhost:3333
mon reset halt
mon flash erase_address 0x08000000 0x00100000
mon reset halt
monitor halt
monitor flash protect 0 0 7 off
monitor flash info 0
file bms.elf
load
mon reset halt
thbreak main
continue
