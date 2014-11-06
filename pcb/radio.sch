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
$Comp
L R R3
U 1 1 545BD4B1
P 5350 3200
F 0 "R3" V 5430 3200 40  0000 C CNN
F 1 "2.2k" V 5357 3201 40  0000 C CNN
F 2 "~" V 5280 3200 30  0000 C CNN
F 3 "~" H 5350 3200 30  0000 C CNN
	1    5350 3200
	0    -1   -1   0   
$EndComp
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
$Comp
L GND #PWR?
U 1 1 545BD9A4
P 5500 3600
F 0 "#PWR?" H 5500 3600 30  0001 C CNN
F 1 "GND" H 5500 3530 30  0001 C CNN
F 2 "" H 5500 3600 60  0000 C CNN
F 3 "" H 5500 3600 60  0000 C CNN
	1    5500 3600
	0    1    1    0   
$EndComp
Wire Wire Line
	5150 2800 5850 2800
Wire Wire Line
	5850 3000 5150 3000
Wire Wire Line
	5850 3200 5600 3200
Wire Wire Line
	5850 3300 5600 3300
Wire Wire Line
	5100 3200 5100 3400
Wire Wire Line
	5100 3400 5850 3400
Wire Wire Line
	5850 3600 5500 3600
Wire Wire Line
	6400 4500 6400 4900
Wire Wire Line
	6400 4700 7100 4700
Wire Wire Line
	7100 4700 7100 4500
Wire Wire Line
	7000 4500 7000 4700
Connection ~ 7000 4700
Wire Wire Line
	6900 4500 6900 4700
Connection ~ 6900 4700
Wire Wire Line
	6800 4500 6800 4700
Connection ~ 6800 4700
Wire Wire Line
	6700 4500 6700 4700
Connection ~ 6700 4700
Wire Wire Line
	6600 4500 6600 4700
Connection ~ 6600 4700
Wire Wire Line
	6500 4500 6500 4700
Connection ~ 6500 4700
Connection ~ 6400 4700
$Comp
L GND #PWR?
U 1 1 545BDA73
P 6400 4900
F 0 "#PWR?" H 6400 4900 30  0001 C CNN
F 1 "GND" H 6400 4830 30  0001 C CNN
F 2 "" H 6400 4900 60  0000 C CNN
F 3 "" H 6400 4900 60  0000 C CNN
	1    6400 4900
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 545BDA82
P 8350 3150
F 0 "R2" V 8430 3150 40  0000 C CNN
F 1 "2.2k" V 8357 3151 40  0000 C CNN
F 2 "~" V 8280 3150 30  0000 C CNN
F 3 "~" H 8350 3150 30  0000 C CNN
	1    8350 3150
	0    -1   -1   0   
$EndComp
$Comp
L R R1
U 1 1 545BDA91
P 8350 3300
F 0 "R1" V 8430 3300 40  0000 C CNN
F 1 "2.2k" V 8357 3301 40  0000 C CNN
F 2 "~" V 8280 3300 30  0000 C CNN
F 3 "~" H 8350 3300 30  0000 C CNN
	1    8350 3300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7950 3150 8100 3150
Wire Wire Line
	7950 3250 8100 3250
Wire Wire Line
	8100 3250 8100 3300
Wire Wire Line
	7950 2750 8900 2750
Wire Wire Line
	7950 2650 8900 2650
$EndSCHEMATC
