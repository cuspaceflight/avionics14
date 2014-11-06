EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:a2235-h
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 4
Title ""
Date "6 nov 2014"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L A2235-H U?
U 1 1 545BD321
P 8200 5050
F 0 "U?" H 6900 6800 60  0000 C CNN
F 1 "A2235-H" H 6900 6800 60  0000 C CNN
F 2 "~" H 6900 6800 60  0000 C CNN
F 3 "~" H 6900 6800 60  0000 C CNN
	1    8200 5050
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 2800 5850 2800
Wire Wire Line
	5850 3000 5150 3000
$Comp
L R R?
U 1 1 545BD4B1
P 5350 3200
F 0 "R?" V 5430 3200 40  0000 C CNN
F 1 "R" V 5357 3201 40  0000 C CNN
F 2 "~" V 5280 3200 30  0000 C CNN
F 3 "~" H 5350 3200 30  0000 C CNN
	1    5350 3200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5850 3200 5600 3200
$Comp
L GND #PWR?
U 1 1 545BD57A
P 5600 3300
F 0 "#PWR?" H 5600 3300 30  0001 C CNN
F 1 "GND" H 5600 3230 30  0001 C CNN
F 2 "" H 5600 3300 60  0000 C CNN
F 3 "" H 5600 3300 60  0000 C CNN
	1    5600 3300
	0    1    1    0   
$EndComp
Wire Wire Line
	5850 3300 5600 3300
$EndSCHEMATC
