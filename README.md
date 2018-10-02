# CCSUART_9-2
This repository documents the debugging code used to send and receive data via UART using Code Composure Studio (CCS).

The initialization code included in main is for an MSP430G2553 and it sets pin configuration, baudrate, clock speed, and enables intterupts. Version 9-2 is capable of sending float values by a push button as well as receiving data which prompts a feedback message.

Future code structure will consist of a main C file with an initilization C file, interrupt C file, and functions C file included in
main. The main program will also be adapted to an MSP430F5529 for added capabilities. 

