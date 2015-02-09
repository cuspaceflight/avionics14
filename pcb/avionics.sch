EESchema Schematic File Version 2
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
LIBS:tvsd
LIBS:FG6943010R
LIBS:buzzer
LIBS:irf7910
LIBS:a2235-h
LIBS:alpha_trx433s
LIBS:quarter_wave_ant
LIBS:rfm69w
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 7
Title "Avionics Board M14"
Date "23 jan 2015"
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
