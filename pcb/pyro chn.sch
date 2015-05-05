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
LIBS:max-7q
LIBS:uSD_holder
LIBS:tvsd
LIBS:irf7910
LIBS:buzzer
LIBS:sma
LIBS:a2235-h
LIBS:swd
LIBS:stm32f405vgt
LIBS:ms5611-01ba03
LIBS:l3g4200d
LIBS:hmc5883l
LIBS:adxl345
LIBS:adp3335
LIBS:FG6943010R
LIBS:rfm69w
LIBS:alpha_trx433s
LIBS:avionics-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 5 7
Title ""
Date "23 jan 2015"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 6850 2450 0    60   Input ~ 0
PY2_TRG
Text HLabel 6850 5250 0    60   Input ~ 0
PY4_TRG
$Comp
L +BATT #PWR031
U 1 1 5464E6DA
P 4100 1450
F 0 "#PWR031" H 4100 1400 20  0001 C CNN
F 1 "+BATT" H 4100 1550 30  0000 C CNN
F 2 "" H 4100 1450 60  0000 C CNN
F 3 "" H 4100 1450 60  0000 C CNN
	1    4100 1450
	1    0    0    -1  
$EndComp
$Comp
L FDN304P Q502
U 1 1 5464E6EE
P 3950 1950
F 0 "Q502" H 4230 1940 60  0000 C CNN
F 1 "FDN304P" H 3670 2040 60  0000 C CNN
F 2 "" H 3950 1950 60  0001 C CNN
F 3 "~" H 3950 1950 60  0000 C CNN
F 4 "1467974" H 3950 1950 60  0001 C CNN "Farnell"
F 5 "Value" H 3950 1950 60  0001 C CNN "Fieldname"
	1    3950 1950
	1    0    0    -1  
$EndComp
$Comp
L IRF7910 Q501
U 1 1 5464E6F4
P 8000 2350
F 0 "Q501" H 8305 2345 60  0000 C CNN
F 1 "IRF7910" H 7720 2440 60  0000 C CNN
F 2 "avionics14:SO-8" H 8000 2350 60  0001 C CNN
F 3 "~" H 8000 2350 60  0000 C CNN
F 4 "1013366" H 8000 2350 60  0001 C CNN "Farnell"
	1    8000 2350
	1    0    0    -1  
$EndComp
$Comp
L RESISTOR R503
U 1 1 5464E6FA
P 2750 2700
F 0 "R503" H 2870 2790 60  0000 C CNN
F 1 "100k" H 2750 2550 60  0000 C CNN
F 2 "avionics14:0603" H 2750 2700 60  0001 C CNN
F 3 "~" H 2750 2700 60  0000 C CNN
F 4 "2309107" H 2750 2700 60  0001 C CNN "Farnell"
	1    2750 2700
	0    -1   -1   0   
$EndComp
$Comp
L RESISTOR R505
U 1 1 5464E700
P 3150 1800
F 0 "R505" H 3270 1890 60  0000 C CNN
F 1 "100k" H 3150 1650 60  0000 C CNN
F 2 "avionics14:0603" H 3150 1800 60  0001 C CNN
F 3 "~" H 3150 1800 60  0000 C CNN
F 4 "2309107" H 3150 1800 60  0001 C CNN "Farnell"
	1    3150 1800
	0    -1   -1   0   
$EndComp
$Comp
L RESISTOR R501
U 1 1 5464E70F
P 2150 2450
F 0 "R501" H 2270 2540 60  0000 C CNN
F 1 "1k" H 2150 2300 60  0000 C CNN
F 2 "avionics14:0603" H 2150 2450 60  0001 C CNN
F 3 "~" H 2150 2450 60  0000 C CNN
F 4 "2309108" H 2150 2450 60  0001 C CNN "Farnell"
	1    2150 2450
	1    0    0    -1  
$EndComp
Text HLabel 1850 2450 0    60   Input ~ 0
PY1_TRG
Text HLabel 1850 5250 0    60   Input ~ 0
PY3_TRG
$Comp
L GND #PWR032
U 1 1 5464EFFD
P 4100 3050
F 0 "#PWR032" H 4100 3050 30  0001 C CNN
F 1 "GND" H 4100 2980 30  0001 C CNN
F 2 "" H 4100 3050 60  0000 C CNN
F 3 "" H 4100 3050 60  0000 C CNN
	1    4100 3050
	1    0    0    -1  
$EndComp
$Comp
L +BATT #PWR033
U 1 1 5475F20A
P 4100 4250
F 0 "#PWR033" H 4100 4200 20  0001 C CNN
F 1 "+BATT" H 4100 4350 30  0000 C CNN
F 2 "" H 4100 4250 60  0000 C CNN
F 3 "" H 4100 4250 60  0000 C CNN
	1    4100 4250
	1    0    0    -1  
$EndComp
$Comp
L FDN304P Q503
U 1 1 5475F216
P 3950 4750
F 0 "Q503" H 4230 4740 60  0000 C CNN
F 1 "FDN304P" H 3670 4840 60  0000 C CNN
F 2 "" H 3950 4750 60  0001 C CNN
F 3 "~" H 3950 4750 60  0000 C CNN
F 4 "1467974" H 3950 4750 60  0001 C CNN "Farnell"
F 5 "Value" H 3950 4750 60  0001 C CNN "Fieldname"
	1    3950 4750
	1    0    0    -1  
$EndComp
$Comp
L IRF7910 Q501
U 2 1 5475F21C
P 3000 2350
F 0 "Q501" H 3305 2345 60  0000 C CNN
F 1 "IRF7910" H 2720 2440 60  0000 C CNN
F 2 "avionics14:SO-8" H 3000 2350 60  0001 C CNN
F 3 "~" H 3000 2350 60  0000 C CNN
F 4 "1013366" H 3000 2350 60  0001 C CNN "Farnell"
	2    3000 2350
	1    0    0    -1  
$EndComp
$Comp
L RESISTOR R504
U 1 1 5475F222
P 2750 5500
F 0 "R504" H 2870 5590 60  0000 C CNN
F 1 "100k" H 2750 5350 60  0000 C CNN
F 2 "avionics14:0603" H 2750 5500 60  0001 C CNN
F 3 "~" H 2750 5500 60  0000 C CNN
F 4 "2309107" H 2750 5500 60  0001 C CNN "Farnell"
	1    2750 5500
	0    -1   -1   0   
$EndComp
$Comp
L RESISTOR R506
U 1 1 5475F228
P 3150 4600
F 0 "R506" H 3270 4690 60  0000 C CNN
F 1 "100k" H 3150 4450 60  0000 C CNN
F 2 "avionics14:0603" H 3150 4600 60  0001 C CNN
F 3 "~" H 3150 4600 60  0000 C CNN
F 4 "2309107" H 3150 4600 60  0001 C CNN "Farnell"
	1    3150 4600
	0    -1   -1   0   
$EndComp
$Comp
L RESISTOR R502
U 1 1 5475F239
P 2150 5250
F 0 "R502" H 2270 5340 60  0000 C CNN
F 1 "1k" H 2150 5100 60  0000 C CNN
F 2 "avionics14:0603" H 2150 5250 60  0001 C CNN
F 3 "~" H 2150 5250 60  0000 C CNN
F 4 "2309108" H 2150 5250 60  0001 C CNN "Farnell"
	1    2150 5250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR034
U 1 1 5475F241
P 4100 5850
F 0 "#PWR034" H 4100 5850 30  0001 C CNN
F 1 "GND" H 4100 5780 30  0001 C CNN
F 2 "" H 4100 5850 60  0000 C CNN
F 3 "" H 4100 5850 60  0000 C CNN
	1    4100 5850
	1    0    0    -1  
$EndComp
$Comp
L +BATT #PWR035
U 1 1 5475F283
P 9100 4250
F 0 "#PWR035" H 9100 4200 20  0001 C CNN
F 1 "+BATT" H 9100 4350 30  0000 C CNN
F 2 "" H 9100 4250 60  0000 C CNN
F 3 "" H 9100 4250 60  0000 C CNN
	1    9100 4250
	1    0    0    -1  
$EndComp
$Comp
L FDN304P Q506
U 1 1 5475F28F
P 8950 4750
F 0 "Q506" H 9230 4740 60  0000 C CNN
F 1 "FDN304P" H 8670 4840 60  0000 C CNN
F 2 "" H 8950 4750 60  0001 C CNN
F 3 "~" H 8950 4750 60  0000 C CNN
F 4 "1467974" H 8950 4750 60  0001 C CNN "Farnell"
F 5 "Value" H 8950 4750 60  0001 C CNN "Fieldname"
	1    8950 4750
	1    0    0    -1  
$EndComp
$Comp
L IRF7910 Q504
U 2 1 5475F295
P 3000 5150
F 0 "Q504" H 3305 5145 60  0000 C CNN
F 1 "IRF7910" H 2720 5240 60  0000 C CNN
F 2 "avionics14:SO-8" H 3000 5150 60  0001 C CNN
F 3 "~" H 3000 5150 60  0000 C CNN
F 4 "1013366" H 3000 5150 60  0001 C CNN "Farnell"
	2    3000 5150
	1    0    0    -1  
$EndComp
$Comp
L RESISTOR R514
U 1 1 5475F29B
P 7750 5500
F 0 "R514" H 7870 5590 60  0000 C CNN
F 1 "100k" H 7750 5350 60  0000 C CNN
F 2 "avionics14:0603" H 7750 5500 60  0001 C CNN
F 3 "~" H 7750 5500 60  0000 C CNN
F 4 "2309107" H 7750 5500 60  0001 C CNN "Farnell"
	1    7750 5500
	0    -1   -1   0   
$EndComp
$Comp
L RESISTOR R516
U 1 1 5475F2A1
P 8150 4600
F 0 "R516" H 8270 4690 60  0000 C CNN
F 1 "100k" H 8150 4450 60  0000 C CNN
F 2 "avionics14:0603" H 8150 4600 60  0001 C CNN
F 3 "~" H 8150 4600 60  0000 C CNN
F 4 "2309107" H 8150 4600 60  0001 C CNN "Farnell"
	1    8150 4600
	0    -1   -1   0   
$EndComp
$Comp
L RESISTOR R512
U 1 1 5475F2B2
P 7150 5250
F 0 "R512" H 7270 5340 60  0000 C CNN
F 1 "1k" H 7150 5100 60  0000 C CNN
F 2 "avionics14:0603" H 7150 5250 60  0001 C CNN
F 3 "~" H 7150 5250 60  0000 C CNN
F 4 "2309108" H 7150 5250 60  0001 C CNN "Farnell"
	1    7150 5250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR036
U 1 1 5475F2BA
P 9100 5850
F 0 "#PWR036" H 9100 5850 30  0001 C CNN
F 1 "GND" H 9100 5780 30  0001 C CNN
F 2 "" H 9100 5850 60  0000 C CNN
F 3 "" H 9100 5850 60  0000 C CNN
	1    9100 5850
	1    0    0    -1  
$EndComp
$Comp
L +BATT #PWR037
U 1 1 5475F2D2
P 9100 1450
F 0 "#PWR037" H 9100 1400 20  0001 C CNN
F 1 "+BATT" H 9100 1550 30  0000 C CNN
F 2 "" H 9100 1450 60  0000 C CNN
F 3 "" H 9100 1450 60  0000 C CNN
	1    9100 1450
	1    0    0    -1  
$EndComp
$Comp
L FDN304P Q505
U 1 1 5475F2DE
P 8950 1950
F 0 "Q505" H 9230 1940 60  0000 C CNN
F 1 "FDN304P" H 8670 2040 60  0000 C CNN
F 2 "" H 8950 1950 60  0001 C CNN
F 3 "~" H 8950 1950 60  0000 C CNN
F 4 "1467974" H 8950 1950 60  0001 C CNN "Farnell"
F 5 "Value" H 8950 1950 60  0001 C CNN "Fieldname"
	1    8950 1950
	1    0    0    -1  
$EndComp
$Comp
L IRF7910 Q504
U 1 1 5475F2E4
P 8000 5150
F 0 "Q504" H 8305 5145 60  0000 C CNN
F 1 "IRF7910" H 7720 5240 60  0000 C CNN
F 2 "avionics14:SO-8" H 8000 5150 60  0001 C CNN
F 3 "~" H 8000 5150 60  0000 C CNN
F 4 "1013366" H 8000 5150 60  0001 C CNN "Farnell"
	1    8000 5150
	1    0    0    -1  
$EndComp
$Comp
L RESISTOR R513
U 1 1 5475F2EA
P 7750 2700
F 0 "R513" H 7870 2790 60  0000 C CNN
F 1 "100k" H 7750 2550 60  0000 C CNN
F 2 "avionics14:0603" H 7750 2700 60  0001 C CNN
F 3 "~" H 7750 2700 60  0000 C CNN
F 4 "2309107" H 7750 2700 60  0001 C CNN "Farnell"
	1    7750 2700
	0    -1   -1   0   
$EndComp
$Comp
L RESISTOR R515
U 1 1 5475F2F0
P 8150 1800
F 0 "R515" H 8270 1890 60  0000 C CNN
F 1 "100k" H 8150 1650 60  0000 C CNN
F 2 "avionics14:0603" H 8150 1800 60  0001 C CNN
F 3 "~" H 8150 1800 60  0000 C CNN
	1    8150 1800
	0    -1   -1   0   
$EndComp
$Comp
L RESISTOR R511
U 1 1 5475F301
P 7150 2450
F 0 "R511" H 7270 2540 60  0000 C CNN
F 1 "1k" H 7150 2300 60  0000 C CNN
F 2 "avionics14:0603" H 7150 2450 60  0001 C CNN
F 3 "~" H 7150 2450 60  0000 C CNN
F 4 "2309108" H 7150 2450 60  0001 C CNN "Farnell"
	1    7150 2450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR038
U 1 1 5475F309
P 9100 3050
F 0 "#PWR038" H 9100 3050 30  0001 C CNN
F 1 "GND" H 9100 2980 30  0001 C CNN
F 2 "" H 9100 3050 60  0000 C CNN
F 3 "" H 9100 3050 60  0000 C CNN
	1    9100 3050
	1    0    0    -1  
$EndComp
$Comp
L TVSD D503
U 1 1 5478E408
P 4100 5450
F 0 "D503" H 4220 5440 60  0000 C CNN
F 1 "CGA0402MLC-12G" V 4000 5450 39  0000 C CNN
F 2 "avionics14:R0402" H 4100 5450 60  0001 C CNN
F 3 "~" H 4100 5450 60  0000 C CNN
F 4 "2368169" H 4100 5450 60  0001 C CNN "Farnell"
F 5 "Value" H 4100 5450 60  0001 C CNN "Fieldname"
	1    4100 5450
	1    0    0    -1  
$EndComp
$Comp
L TVSD D504
U 1 1 5478E43A
P 9100 5450
F 0 "D504" H 9220 5440 60  0000 C CNN
F 1 "CGA0402MLC-12G" V 9000 5450 39  0000 C CNN
F 2 "avionics14:R0402" H 9100 5450 60  0001 C CNN
F 3 "~" H 9100 5450 60  0000 C CNN
F 4 "2368169" H 9100 5450 60  0001 C CNN "Farnell"
F 5 "Value" H 9100 5450 60  0001 C CNN "Fieldname"
	1    9100 5450
	1    0    0    -1  
$EndComp
$Comp
L TVSD D502
U 1 1 5478E44A
P 9100 2650
F 0 "D502" H 9220 2640 60  0000 C CNN
F 1 "CGA0402MLC-12G" V 9000 2650 39  0000 C CNN
F 2 "avionics14:R0402" H 9100 2650 60  0001 C CNN
F 3 "~" H 9100 2650 60  0000 C CNN
F 4 "2368169" H 9100 2650 60  0001 C CNN "Farnell"
F 5 "Value" H 9100 2650 60  0001 C CNN "Fieldname"
	1    9100 2650
	1    0    0    -1  
$EndComp
$Comp
L TVSD D501
U 1 1 5478E450
P 4100 2650
F 0 "D501" H 4220 2640 60  0000 C CNN
F 1 "CGA0402MLC-12G" V 4000 2650 39  0000 C CNN
F 2 "avionics14:R0402" H 4100 2650 60  0001 C CNN
F 3 "~" H 4100 2650 60  0000 C CNN
F 4 "2368169" H 4100 2650 60  0001 C CNN "Farnell"
F 5 "Value" H 4100 2650 60  0001 C CNN "Fieldname"
	1    4100 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 2150 4100 2450
Wire Wire Line
	2750 2950 4500 2950
Wire Wire Line
	4100 2850 4100 3050
Connection ~ 4100 2950
Wire Wire Line
	4100 1450 4100 1750
Connection ~ 4100 1550
Connection ~ 3150 2050
Connection ~ 3150 2950
Connection ~ 2750 2450
Wire Wire Line
	4100 2250 4500 2250
Connection ~ 4100 5750
Wire Wire Line
	2750 5750 4500 5750
Wire Wire Line
	3150 4350 4500 4350
Connection ~ 4100 4350
Connection ~ 3150 4850
Connection ~ 3150 5750
Connection ~ 2750 5250
Connection ~ 9100 5750
Wire Wire Line
	7750 5750 9500 5750
Wire Wire Line
	8150 4350 9500 4350
Connection ~ 9100 4350
Connection ~ 8150 4850
Connection ~ 8150 5750
Connection ~ 7750 5250
Connection ~ 9100 2950
Wire Wire Line
	7750 2950 9500 2950
Connection ~ 8150 2050
Connection ~ 8150 2950
Connection ~ 7750 2450
Wire Wire Line
	4500 2050 4500 2850
Wire Wire Line
	3150 1550 4500 1550
Wire Wire Line
	8150 1550 9500 1550
Connection ~ 9100 1550
$Comp
L RESISTOR R507
U 1 1 5478E9AE
P 4500 1800
F 0 "R507" H 4620 1890 60  0000 C CNN
F 1 "51k" H 4500 1650 60  0000 C CNN
F 2 "avionics14:0603" H 4500 1800 60  0001 C CNN
F 3 "~" H 4500 1800 60  0000 C CNN
F 4 "2333565" H 4500 1800 60  0001 C CNN "Farnell"
	1    4500 1800
	0    -1   -1   0   
$EndComp
Connection ~ 4500 2250
Connection ~ 4100 2250
Text HLabel 5050 2250 2    60   Output ~ 0
~PY1_CHK
$Comp
L RESISTOR R509
U 1 1 5478EC5D
P 4750 2250
F 0 "R509" H 4870 2340 60  0000 C CNN
F 1 "51k" H 4750 2100 60  0000 C CNN
F 2 "avionics14:0603" H 4750 2250 60  0001 C CNN
F 3 "~" H 4750 2250 60  0000 C CNN
F 4 "2333565" H 4750 2250 60  0001 C CNN "Farnell"
	1    4750 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 2250 5050 2250
$Comp
L RESISTOR R517
U 1 1 5478ED11
P 9500 1800
F 0 "R517" H 9620 1890 60  0000 C CNN
F 1 "51k" H 9500 1650 60  0000 C CNN
F 2 "avionics14:0603" H 9500 1800 60  0001 C CNN
F 3 "~" H 9500 1800 60  0000 C CNN
F 4 "2333565" H 9500 1800 60  0001 C CNN "Farnell"
	1    9500 1800
	0    -1   -1   0   
$EndComp
Wire Wire Line
	9100 2850 9100 3050
Wire Wire Line
	9100 2250 9500 2250
Wire Wire Line
	9500 2050 9500 2850
Connection ~ 9100 2250
Connection ~ 9500 2250
$Comp
L RESISTOR R519
U 1 1 5478EF64
P 9750 2250
F 0 "R519" H 9870 2340 60  0000 C CNN
F 1 "51k" H 9750 2100 60  0000 C CNN
F 2 "avionics14:0603" H 9750 2250 60  0001 C CNN
F 3 "~" H 9750 2250 60  0000 C CNN
F 4 "2333565" H 9750 2250 60  0001 C CNN "Farnell"
	1    9750 2250
	1    0    0    -1  
$EndComp
Text HLabel 10050 2250 2    60   Output ~ 0
~PY2_CHK
Wire Wire Line
	10000 2250 10050 2250
$Comp
L RESISTOR R508
U 1 1 5478EFBF
P 4500 4600
F 0 "R508" H 4620 4690 60  0000 C CNN
F 1 "51k" H 4500 4450 60  0000 C CNN
F 2 "avionics14:0603" H 4500 4600 60  0001 C CNN
F 3 "~" H 4500 4600 60  0000 C CNN
F 4 "2333565" H 4500 4600 60  0001 C CNN "Farnell"
	1    4500 4600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4100 5650 4100 5850
Wire Wire Line
	4500 4850 4500 5650
Wire Wire Line
	4100 5050 4500 5050
Connection ~ 4500 5050
Connection ~ 4100 5050
$Comp
L RESISTOR R510
U 1 1 5478F17B
P 4750 5050
F 0 "R510" H 4870 5140 60  0000 C CNN
F 1 "51k" H 4750 4900 60  0000 C CNN
F 2 "avionics14:0603" H 4750 5050 60  0001 C CNN
F 3 "~" H 4750 5050 60  0000 C CNN
F 4 "2333565" H 4750 5050 60  0001 C CNN "Farnell"
	1    4750 5050
	1    0    0    -1  
$EndComp
Text HLabel 5050 5050 2    60   Output ~ 0
~PY3_CHK
Wire Wire Line
	5000 5050 5050 5050
$Comp
L RESISTOR R518
U 1 1 5478F1D1
P 9500 4600
F 0 "R518" H 9620 4690 60  0000 C CNN
F 1 "51k" H 9500 4450 60  0000 C CNN
F 2 "avionics14:0603" H 9500 4600 60  0001 C CNN
F 3 "~" H 9500 4600 60  0000 C CNN
F 4 "2333565" H 9500 4600 60  0001 C CNN "Farnell"
	1    9500 4600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	9100 5650 9100 5850
Wire Wire Line
	9500 4850 9500 5650
Wire Wire Line
	9100 5050 9500 5050
Connection ~ 9500 5050
Connection ~ 9100 5050
$Comp
L RESISTOR R520
U 1 1 5478F39C
P 9750 5050
F 0 "R520" H 9870 5140 60  0000 C CNN
F 1 "51k" H 9750 4900 60  0000 C CNN
F 2 "avionics14:0603" H 9750 5050 60  0001 C CNN
F 3 "~" H 9750 5050 60  0000 C CNN
F 4 "2333565" H 9750 5050 60  0001 C CNN "Farnell"
	1    9750 5050
	1    0    0    -1  
$EndComp
Text HLabel 10050 5050 2    60   Output ~ 0
~PY4_CHK
Wire Wire Line
	10000 5050 10050 5050
Wire Wire Line
	3150 2050 3800 2050
Wire Wire Line
	1850 2450 1900 2450
Wire Wire Line
	2400 2450 2850 2450
Wire Wire Line
	3150 2550 3150 2950
Wire Wire Line
	3150 2050 3150 2150
Wire Wire Line
	6850 2450 6900 2450
Wire Wire Line
	1850 5250 1900 5250
Wire Wire Line
	2400 5250 2850 5250
Wire Wire Line
	3150 5350 3150 5750
Wire Wire Line
	3150 4850 3150 4950
Wire Wire Line
	3150 4850 3800 4850
Wire Wire Line
	4100 4950 4100 5250
Wire Wire Line
	4100 4250 4100 4550
Wire Wire Line
	7400 5250 7850 5250
Wire Wire Line
	8150 4850 8150 4950
Wire Wire Line
	8150 5350 8150 5750
Wire Wire Line
	9100 4950 9100 5250
Wire Wire Line
	9100 4250 9100 4550
Wire Wire Line
	8150 4850 8800 4850
Wire Wire Line
	7400 2450 7850 2450
Wire Wire Line
	8150 2550 8150 2950
Wire Wire Line
	8150 2050 8150 2150
Wire Wire Line
	9100 2150 9100 2450
Wire Wire Line
	9100 1450 9100 1750
Wire Wire Line
	8150 2050 8800 2050
$Comp
L CONN_01X02 P503
U 1 1 54D92414
P 4700 5700
F 0 "P503" H 4700 5850 50  0000 C CNN
F 1 "CONN_01X02" V 4800 5700 50  0000 C CNN
F 2 "avionics14:S02B-PASK-2" H 4700 5700 60  0001 C CNN
F 3 "" H 4700 5700 60  0000 C CNN
F 4 "RS 512-8462" H 4700 5700 60  0001 C CNN "Farnell"
	1    4700 5700
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P504
U 1 1 54D924F7
P 9700 5700
F 0 "P504" H 9700 5850 50  0000 C CNN
F 1 "CONN_01X02" V 9800 5700 50  0000 C CNN
F 2 "avionics14:S02B-PASK-2" H 9700 5700 60  0001 C CNN
F 3 "" H 9700 5700 60  0000 C CNN
F 4 "RS 512-8462" H 9700 5700 60  0001 C CNN "Farnell"
	1    9700 5700
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P502
U 1 1 54D92735
P 9700 2900
F 0 "P502" H 9700 3050 50  0000 C CNN
F 1 "CONN_01X02" V 9800 2900 50  0000 C CNN
F 2 "avionics14:S02B-PASK-2" H 9700 2900 60  0001 C CNN
F 3 "" H 9700 2900 60  0000 C CNN
F 4 "RS 512-8462" H 9700 2900 60  0001 C CNN "Farnell"
	1    9700 2900
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P501
U 1 1 54D92A16
P 4700 2900
F 0 "P501" H 4700 3050 50  0000 C CNN
F 1 "CONN_01X02" V 4800 2900 50  0000 C CNN
F 2 "avionics14:S02B-PASK-2" H 4700 2900 60  0001 C CNN
F 3 "" H 4700 2900 60  0000 C CNN
F 4 "RS 512-8462" H 4700 2900 60  0001 C CNN "Farnell"
	1    4700 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	6850 5250 6900 5250
Wire Wire Line
	3200 4950 3200 4850
Connection ~ 3200 4850
Wire Wire Line
	3200 2150 3200 2050
Connection ~ 3200 2050
Wire Wire Line
	8200 2150 8200 2050
Connection ~ 8200 2050
Wire Wire Line
	8200 4950 8200 4850
Connection ~ 8200 4850
$EndSCHEMATC
