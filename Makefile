# List all source files the application uses.
APPLICATION_SRCS = main.c $(wildcard src/*.c) hexparser.c
# Use shell to find name of root folder. Possible but horrible to do in make.
PROJECT_NAME = $(shell basename "$(realpath .)")

DEVICE = NRF51
BOARD = BOARD_PCA10001
USE_S110 = 0

LIBRARY_PATHS += inc/
SOURCE_PATHS += src/

LIBRARY_PATHS += ../libhexwriter/inc/
SOURCE_PATHS += ../libhexwriter/src/

SDK_PATH = ../nrf51/nrf51822/
LIBRARY_PATHS += $(SDK_PATH)Include/ble/softdevice/
CFLAGS = -O0 -g

FLASH_START_ADDRESS = 0x0
GDB_PORT_NUMBER = 2331

include $(SDK_PATH)Source/templates/pure-gcc/Makefile
