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
P 6750 3750
F 0 "U?" H 5450 5500 60  0000 C CNN
F 1 "A2235-H" H 5450 5500 60  0000 C CNN
F 2 "~" H 5450 5500 60  0000 C CNN
F 3 "~" H 5450 5500 60  0000 C CNN
	1    6750 3750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 545BD57A
P 4150 2000
F 0 "#PWR?" H 4150 2000 30  0001 C CNN
F 1 "GND" H 4150 1930 30  0001 C CNN
F 2 "" H 4150 2000 60  0000 C CNN
F 3 "" H 4150 2000 60  0000 C CNN
	1    4150 2000
	0    1    1    0   
$EndComp
$Comp
L GND #PWR?
U 1 1 545BD9A4
P 4050 2300
F 0 "#PWR?" H 4050 2300 30  0001 C CNN
F 1 "GND" H 4050 2230 30  0001 C CNN
F 2 "" H 4050 2300 60  0000 C CNN
F 3 "" H 4050 2300 60  0000 C CNN
	1    4050 2300
	0    1    1    0   
$EndComp
$Comp
L GND #PWR?
U 1 1 545BDA73
P 4950 3600
F 0 "#PWR?" H 4950 3600 30  0001 C CNN
F 1 "GND" H 4950 3530 30  0001 C CNN
F 2 "" H 4950 3600 60  0000 C CNN
F 3 "" H 4950 3600 60  0000 C CNN
	1    4950 3600
	1    0    0    -1  
$EndComp
Text HLabel 3700 1500 0    60   Input ~ 0
Open collector/open drain 
Text HLabel 7450 1350 2    60   Input ~ 0
RX
Text HLabel 7450 1450 2    60   Input ~ 0
TX
Text HLabel 3450 2100 0    60   Input ~ 0
SPI Clock pin
Text HLabel 3250 2200 0    60   Input ~ 0
SPI chip select
$Comp
L +3.3V #PWR?
U 1 1 545BDEF8
P 3700 1700
F 0 "#PWR?" H 3700 1660 30  0001 C CNN
F 1 "+3.3V" H 3700 1810 30  0000 C CNN
F 2 "" H 3700 1700 60  0000 C CNN
F 3 "" H 3700 1700 60  0000 C CNN
	1    3700 1700
	1    0    0    -1  
$EndComp
Text HLabel 7500 2550 2    60   Input ~ 0
Antenna inpu
$Comp
L R R2
U 1 1 545BDA82
P 6900 1850
F 0 "R2" V 6980 1850 40  0000 C CNN
F 1 "2.2k" V 6907 1851 40  0000 C CNN
F 2 "~" V 6830 1850 30  0000 C CNN
F 3 "~" H 6900 1850 30  0000 C CNN
	1    6900 1850
	0    -1   -1   0   
$EndComp
$Comp
L R R1
U 1 1 545BDFEF
P 6900 1750
F 0 "R1" V 6980 1750 40  0000 C CNN
F 1 "2.2k" V 6907 1751 40  0000 C CNN
F 2 "~" V 6830 1750 30  0000 C CNN
F 3 "~" H 6900 1750 30  0000 C CNN
	1    6900 1750
	0    -1   -1   0   
$EndComp
Text HLabel 7700 1500 2    60   Input ~ 0
micro Push Pull output
Text HLabel 7850 1750 2    60   Input ~ 0
Connect to microcontroller I2C_DIO
Text HLabel 7850 1850 2    60   Input ~ 0
Connect to microcontroller I2C_CLK 
$Comp
L alpha_trx433s U?
U 1 1 5468BFE4
P 3900 6050
F 0 "U?" H 3900 6050 60  0000 C CNN
F 1 "alpha_trx433s" H 3900 6050 60  0000 C CNN
F 2 "" H 3900 6050 60  0000 C CNN
F 3 "" H 3900 6050 60  0000 C CNN
	1    3900 6050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 5468C022
P 4700 5650
F 0 "#PWR?" H 4700 5650 30  0001 C CNN
F 1 "GND" H 4700 5580 30  0001 C CNN
F 2 "" H 4700 5650 60  0000 C CNN
F 3 "" H 4700 5650 60  0000 C CNN
	1    4700 5650
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR?
U 1 1 5468C061
P 4850 5750
F 0 "#PWR?" H 4850 5850 30  0001 C CNN
F 1 "VCC" H 4850 5850 30  0000 C CNN
F 2 "" H 4850 5750 60  0000 C CNN
F 3 "" H 4850 5750 60  0000 C CNN
	1    4850 5750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 5468C0CE
P 3050 5900
F 0 "#PWR?" H 3050 5900 30  0001 C CNN
F 1 "GND" H 3050 5830 30  0001 C CNN
F 2 "" H 3050 5900 60  0000 C CNN
F 3 "" H 3050 5900 60  0000 C CNN
	1    3050 5900
	1    0    0    -1  
$EndComp
Text Notes 3400 5000 0    60   ~ 0
Transmitter Module\n
Text HLabel 4750 5350 2    60   Input ~ 0
SERIAL_CLK
NoConn ~ 3000 5650
NoConn ~ 3000 5550
Text HLabel 4750 5450 2    60   Input ~ 0
SERIAL_OUT
NoConn ~ 3000 5350
NoConn ~ 3000 5250
Text HLabel 4750 5250 2    60   Input ~ 0
SLAVE_SELECT
Wire Wire Line
	4400 5350 4750 5350
Wire Wire Line
	2850 5450 3250 5450
Wire Wire Line
	3050 5850 3050 5900
Wire Wire Line
	3250 5850 3050 5850
Wire Wire Line
	4400 5750 4850 5750
Wire Wire Line
	4700 5600 4700 5650
Wire Wire Line
	4550 5600 4700 5600
Wire Wire Line
	4550 5650 4550 5600
Wire Wire Line
	4400 5650 4550 5650
Wire Wire Line
	7150 1850 7850 1850
Wire Wire Line
	7150 1750 7850 1750
Wire Wire Line
	6500 1750 6650 1750
Wire Wire Line
	6500 2550 7500 2550
Wire Wire Line
	3250 2200 4400 2200
Wire Wire Line
	3450 2100 4400 2100
Wire Wire Line
	2300 1800 4400 1800
Wire Wire Line
	2300 750  2300 1800
Wire Wire Line
	7700 750  2300 750 
Wire Wire Line
	7700 1650 7700 750 
Wire Wire Line
	6500 1650 7700 1650
Wire Wire Line
	6500 1350 7450 1350
Wire Wire Line
	6500 1450 7450 1450
Wire Wire Line
	6650 1950 6650 2000
Wire Wire Line
	6500 1950 6650 1950
Wire Wire Line
	6500 1850 6650 1850
Connection ~ 4950 3400
Connection ~ 5050 3400
Wire Wire Line
	5050 3200 5050 3400
Connection ~ 5150 3400
Wire Wire Line
	5150 3200 5150 3400
Connection ~ 5250 3400
Wire Wire Line
	5250 3200 5250 3400
Connection ~ 5350 3400
Wire Wire Line
	5350 3200 5350 3400
Connection ~ 5450 3400
Wire Wire Line
	5450 3200 5450 3400
Connection ~ 5550 3400
Wire Wire Line
	5550 3200 5550 3400
Wire Wire Line
	5650 3400 5650 3200
Wire Wire Line
	4950 3400 5650 3400
Wire Wire Line
	4950 3200 4950 3600
Wire Wire Line
	4400 2300 4050 2300
Wire Wire Line
	4400 2000 4150 2000
Wire Wire Line
	4400 1900 4150 1900
Wire Wire Line
	4400 1700 3700 1700
Wire Wire Line
	3700 1500 4400 1500
Wire Wire Line
	3250 5650 3000 5650
Wire Wire Line
	3250 5550 3000 5550
Wire Wire Line
	4400 5450 4750 5450
Wire Wire Line
	3250 5350 3000 5350
Wire Wire Line
	3250 5250 3000 5250
Wire Wire Line
	4400 5250 4750 5250
Wire Wire Line
	2850 5450 2850 6250
Wire Wire Line
	2850 6250 4400 6250
Wire Wire Line
	4400 6250 4400 5850
Wire Wire Line
	4400 5850 4700 5850
Connection ~ 4400 5850
Text Label 5350 5900 2    60   ~ 0
ANTENNA_OUT
$EndSCHEMATC
