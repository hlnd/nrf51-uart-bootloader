nrf51-uart-bootloader
=====================

A very simple UART bootloader for nRF51822. 

This application is to be written in code region 0 on an nRF51822. When running, it can then receive a Intel Hex file, 
line by line over the UART port, and write the contents to code region 1. By default, a size of 0x14000 is used for CR0,
but this can be changed if needed.

The application was developed with GCC and on the Evaluation Kit, PCA10001. The quality should be considered to be a 
proof of concept, nothing more. 

Architecture
============
The bootloader runs in its own code region, and implements assembly wrappers for all interrupts. If the bootloader is not
currently running, any interrupts will be forwarded to the application, so that they can be handled. This gives some 
extra latency. 

Known issues
============
- The writing is extremely slow. It hasn't been checked in detail why this is, but transmitting a hex file is not an 
efficient solution. It was chosen for the simplicity of the PC-side implementation.
- The application has not been tested very thoroughly, and might very well have issues. 
- As stated above, this has been developed with GCC, and has not been tested or verified at all with Keil. 
