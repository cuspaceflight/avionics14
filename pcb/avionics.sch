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
LIBS:alpha_trx433s
LIBS:a2235-h
EELAYER 24 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 4
Title "Avionics Board M14"
Date "06 Nov 2014"
Rev "1"
Comp "CU Spaceflight"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Sheet
S 1800 1700 1250 2150
U 545BA057
F0 "Radio" 60
F1 "radio.sch" 60
$EndSheet
$Sheet
S 5000 1700 1350 2150
U 545BA068
F0 "Datalogging & Pyros" 60
F1 "datalogging.sch" 60
$EndSheet
$Sheet
S 8350 1700 1350 2150
U 545BA07E
F0 "Inertial Measurement" 60
F1 "imu.sch" 60
$EndSheet
Text Notes 600  900  0    250  ~ 0
Avionics Project Michalmas 2014
Text Notes 650  1150 0    60   ~ 0
Each team has a subsheet below which can contain further subsheets and should expose hierarchical labels for input/output connections.
$Comp
L GND #PWR?
U 1 1 545BA0DA
P 1000 4400
F 0 "#PWR?" H 1000 4400 30  0001 C CNN
F 1 "GND" H 1000 4330 30  0001 C CNN
F 2 "" H 1000 4400 60  0000 C CNN
F 3 "" H 1000 4400 60  0000 C CNN
	1    1000 4400
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR?
U 1 1 545BA0EE
P 850 4400
F 0 "#PWR?" H 850 4360 30  0001 C CNN
F 1 "+3.3V" H 850 4510 30  0000 C CNN
F 2 "" H 850 4400 60  0000 C CNN
F 3 "" H 850 4400 60  0000 C CNN
	1    850  4400
	1    0    0    -1  
$EndComp
$Comp
L +BATT #PWR?
U 1 1 545BA102
P 700 4400
F 0 "#PWR?" H 700 4350 20  0001 C CNN
F 1 "+BATT" H 700 4500 30  0000 C CNN
F 2 "" H 700 4400 60  0000 C CNN
F 3 "" H 700 4400 60  0000 C CNN
	1    700  4400
	1    0    0    -1  
$EndComp
Wire Notes Line
	600  4200 1150 4200
Wire Notes Line
	1150 4200 1150 4700
Wire Notes Line
	1150 4700 600  4700
Wire Notes Line
	600  4700 600  4200
Text Notes 650  4650 0    60   ~ 0
POWER\nSYMBOLS
Wire Notes Line
	600  4100 600  1300
Text Notes 650  1500 0    100  ~ 0
Team Sheets
Wire Notes Line
	600  1300 11000 1300
Wire Notes Line
	11000 1300 11000 4100
Wire Notes Line
	11000 4100 600  4100
$EndSCHEMATC
