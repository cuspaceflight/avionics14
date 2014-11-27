EESchema Schematic File Version 2
LIBS:IRF7910
LIBS:FDN304P
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
LIBS:resistor
LIBS:uSD_holder
LIBS:avionics-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 6
Title ""
Date "26 nov 2014"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L +3.3V #PWR?
U 1 1 54764AE2
P 5400 2800
F 0 "#PWR?" H 5400 2760 30  0001 C CNN
F 1 "+3.3V" H 5400 2910 30  0000 C CNN
F 2 "" H 5400 2800 60  0000 C CNN
F 3 "" H 5400 2800 60  0000 C CNN
	1    5400 2800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 54764C20
P 5400 4200
F 0 "#PWR?" H 5400 4200 30  0001 C CNN
F 1 "GND" H 5400 4130 30  0001 C CNN
F 2 "" H 5400 4200 60  0000 C CNN
F 3 "" H 5400 4200 60  0000 C CNN
	1    5400 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 4100 5400 4200
Wire Wire Line
	5400 2800 5400 3200
Wire Wire Line
	4950 3850 4900 3850
Text HLabel 4900 3850 0    60   Output ~ 0
Card Detect
Text HLabel 5950 3500 2    60   Input ~ 0
~CS
Text HLabel 5950 3600 2    60   Input ~ 0
SCLK
Text HLabel 7050 3700 2    60   Input ~ 0
Data_in
Text HLabel 7050 3800 2    60   Output ~ 0
Data_out
$Comp
L ZENER D?
U 1 1 54764C32
P 4350 3300
F 0 "D?" H 4350 3400 50  0000 C CNN
F 1 "1SMB15AT3G" H 4350 3200 40  0000 C CNN
F 2 "~" H 4350 3300 60  0000 C CNN
F 3 "~" H 4350 3300 60  0000 C CNN
	1    4350 3300
	0    -1   -1   0   
$EndComp
$Comp
L C C?
U 1 1 54764C46
P 4650 3300
F 0 "C?" H 4650 3400 40  0000 L CNN
F 1 "1uF" H 4656 3215 40  0000 L CNN
F 2 "~" H 4688 3150 30  0000 C CNN
F 3 "~" H 4650 3300 60  0000 C CNN
	1    4650 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	5900 3500 5950 3500
Wire Wire Line
	5900 3600 5950 3600
$Comp
L RESISTOR R?
U 1 1 54764CD8
P 6550 3325
F 0 "R?" H 6550 3425 60  0000 C CNN
F 1 "75k" H 6550 3225 60  0000 C CNN
F 2 "~" H 6550 3325 60  0000 C CNN
F 3 "~" H 6550 3325 60  0000 C CNN
	1    6550 3325
	0    1    1    0   
$EndComp
$Comp
L RESISTOR R?
U 1 1 54764CE7
P 6900 3325
F 0 "R?" H 6925 3425 60  0000 C CNN
F 1 "75k" H 6900 3225 60  0000 C CNN
F 2 "~" H 6900 3325 60  0000 C CNN
F 3 "~" H 6900 3325 60  0000 C CNN
	1    6900 3325
	0    1    1    0   
$EndComp
Wire Wire Line
	5900 3700 7050 3700
Wire Wire Line
	5900 3800 7050 3800
Wire Wire Line
	6550 3550 6550 3700
Connection ~ 6550 3700
Wire Wire Line
	6900 3550 6900 3800
Connection ~ 6900 3800
Wire Wire Line
	6550 3100 6550 3050
Wire Wire Line
	4350 3050 6900 3050
Connection ~ 5400 3050
Wire Wire Line
	6900 3050 6900 3100
Connection ~ 6550 3050
Wire Wire Line
	4350 3050 4350 3100
Wire Wire Line
	4650 3100 4650 3050
Connection ~ 4650 3050
Wire Wire Line
	4650 3500 4650 3550
Wire Wire Line
	4650 3550 4350 3550
Wire Wire Line
	4350 3550 4350 3500
$Comp
L GND #PWR?
U 1 1 54764ED4
P 4500 3650
F 0 "#PWR?" H 4500 3650 30  0001 C CNN
F 1 "GND" H 4500 3580 30  0001 C CNN
F 2 "" H 4500 3650 60  0000 C CNN
F 3 "" H 4500 3650 60  0000 C CNN
	1    4500 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 3650 4500 3550
Connection ~ 4500 3550
$Comp
L MICROSD_HOLDER ~?
U 1 1 54766118
P 5400 3650
F 0 "~?" H 5500 3250 60  0001 C CNN
F 1 "MICROSD_HOLDER" H 5850 4000 60  0000 C CNN
F 2 "~" H 5800 3650 60  0000 C CNN
F 3 "~" H 5800 3650 60  0000 C CNN
	1    5400 3650
	1    0    0    -1  
$EndComp
$EndSCHEMATC
