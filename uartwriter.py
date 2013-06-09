#!/usr/bin/python
# -*- coding: utf-8 -*-
import serial
import argparse
import time
import sys

SUCCESS = 0x01

class UARTBootloaderController(object):
    def __init__(self, port, baudrate):
        self.port = serial.Serial(port, baudrate=baudrate, timeout=10)
        self.port.flushInput()
        self.port.flushOutput()
        self.port.write("\x00\n")
        result = self.port.read()
        if len(result) == 0 or ord(result) != SUCCESS:
            raise Exception("Initialization failed. Are you sure the bootloader is running?")

    def erase_app(self):
        self.port.write("\x01\n")
        time.sleep(2)
        result = self.port.read(1)
        if len(result) == 0 or ord(result) != SUCCESS:
            raise Exception("Erase failed, return code: {0}.".format(result))
        
    def write_file(self, file):
        for line in file:
            self.port.write("\x02{0}\n".format(line.strip()))
            time.sleep(1)
            result = self.port.read(1)
            if len(result) == 0 or ord(result) != SUCCESS:
                raise Exception("Write of line {0} failed, error returned \
                        {1}.".format(line.strip(), result))
            print '.',
            sys.stdout.flush()

    def run(self):
        self.port.write("\x03\n");
        

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Write a hex file to an \
            nRF51822 hooked up to a serial port.')
    parser.add_argument('--file', type=file)
    parser.add_argument('--run', type=bool, default=True)
    parser.add_argument('--port', type=str, default="/dev/ttyACM0", help="The serial port to which the nRF51 is connected")
    parser.add_argument('--baud', type=int, default=38400, help="The baudrate with which the bootloader runs.")
    args = parser.parse_args()

    u = UARTBootloaderController(args.port, args.baud)

    if args.file:
        print "Erasing app..."
        u.erase_app()
        print "Flashing app..."
        u.write_file(args.file)

    if args.run:
        print "Starting app..."
        u.run()
