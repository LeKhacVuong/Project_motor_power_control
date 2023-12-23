#!/bin/bash
# SPI FLASH RZT1 R4 flashing.
JFLASH_PATH="JFlash"
PROJECT_PATH="bms.jflash"
#BINARY_PATH="test/test_I2C/test_I2C.hex"
#BINARY_PATH="test/test_UART/test_UART.hex"
#BINARY_PATH="test/test_MainSwitch/test_MainSwitch.hex"
#BINARY_PATH="test/test_CANBus/test_CANBus.hex"
#BINARY_PATH="test/test_AFE/test_AFE.hex"
BINARY_PATH="s2_cbox.hex"
LOG_FILE="jflash_log.log"
"${JFLASH_PATH}" -openprj"${PROJECT_PATH}" -open"${BINARY_PATH}" -auto -jflashlog${LOG_FILE} -hide -startapp -exit &
cat "jflash_log.log"
