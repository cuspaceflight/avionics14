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
LIBS:rfm69w
LIBS:adp3335
LIBS:adxl345
LIBS:hmc5883l
LIBS:l3g4200d
LIBS:ms5611-01ba03
LIBS:stm32f405vgt
LIBS:swd
LIBS:sma
LIBS:avionics-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 7
Title ""
Date "23 jan 2015"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Sheet
S 5050 3300 800  950 
U 5476453E
F0 "microSD" 50
F1 "uSD.sch" 50
F2 "Card Detect" O R 5850 4100 60 
F3 "CLK" I R 5850 3450 60 
F4 "CMD" B R 5850 3600 60 
F5 "DAT0" B L 5050 3450 60 
F6 "DAT1" B L 5050 3600 60 
F7 "DAT2" B L 5050 3750 60 
F8 "DAT3" B L 5050 3900 60 
$EndSheet
$Sheet
S 4800 1900 1300 950 
U 5464CBD3
F0 "pyro chn" 50
F1 "pyro chn.sch" 50
F2 "PY2_TRG" I L 4800 2300 60 
F3 "PY4_TRG" I L 4800 2700 60 
F4 "PY1_TRG" I L 4800 2100 60 
F5 "PY3_TRG" I L 4800 2500 60 
F6 "~PY1_CHK" O R 6100 2100 60 
F7 "~PY2_CHK" O R 6100 2300 60 
F8 "~PY3_CHK" O R 6100 2500 60 
F9 "~PY4_CHK" O R 6100 2700 60 
$EndSheet
$Sheet
S 4850 4700 1150 850 
U 5478FE09
F0 "Indicators" 50
F1 "Indicators.sch" 50
F2 "PY_OK" I L 4850 4800 60 
F3 "IMU_OK" I L 4850 5000 60 
F4 "RADIO_OK" I L 4850 5200 60 
F5 "~PY_OK" I R 6000 4800 60 
F6 "~IMU_OK" I R 6000 5000 60 
F7 "~RADIO_OK" I R 6000 5200 60 
F8 "BUZZER" I L 4850 5400 60 
$EndSheet
Text HLabel 4700 2100 0    60   Input ~ 0
PY1_TRG
Text HLabel 4700 2300 0    60   Input ~ 0
PY2_TRG
Text HLabel 4700 2500 0    60   Input ~ 0
PY3_TRG
Text HLabel 4700 2700 0    60   Input ~ 0
PY4_TRG
Text HLabel 6200 2100 2    60   Output ~ 0
~PY1_CHK
Text HLabel 6200 2300 2    60   Output ~ 0
~PY2_CHK
Text HLabel 6200 2500 2    60   Output ~ 0
~PY3_CHK
Text HLabel 6200 2700 2    60   Output ~ 0
~PY4_CHK
Wire Wire Line
	6100 2100 6200 2100
Wire Wire Line
	6100 2300 6200 2300
Wire Wire Line
	6100 2500 6200 2500
Wire Wire Line
	6100 2700 6200 2700
Wire Wire Line
	4700 2100 4800 2100
Wire Wire Line
	4700 2300 4800 2300
Wire Wire Line
	4700 2500 4800 2500
Wire Wire Line
	4700 2700 4800 2700
Text HLabel 4950 3450 0    60   BiDi ~ 0
µSD_DAT0
Text HLabel 4950 3600 0    60   BiDi ~ 0
µSD_DAT1
Text HLabel 4950 3750 0    60   BiDi ~ 0
µSD_DAT2
Text HLabel 4950 3900 0    60   BiDi ~ 0
µSD_DAT3
Text HLabel 5950 3450 2    60   Input ~ 0
µSD_CLK
Text HLabel 5950 3600 2    60   BiDi ~ 0
µSD_CMD
Wire Wire Line
	5950 3600 5850 3600
Wire Wire Line
	5950 3450 5850 3450
Text HLabel 5950 4100 2    60   Output ~ 0
µSD_CD
Wire Wire Line
	5850 4100 5950 4100
Wire Wire Line
	4950 3450 5050 3450
Wire Wire Line
	4950 3600 5050 3600
Wire Wire Line
	4950 3750 5050 3750
Wire Wire Line
	4950 3900 5050 3900
Text HLabel 4750 4800 0    60   Input ~ 0
PY_OK
Text HLabel 4750 5000 0    60   Input ~ 0
IMU_OK
Text HLabel 4750 5200 0    60   Input ~ 0
RADIO_OK
Text HLabel 4750 5400 0    60   Input ~ 0
BUZZER
Text HLabel 6150 5200 2    60   Input ~ 0
~RADIO_OK
Text HLabel 6150 5000 2    60   Input ~ 0
~IMU_OK
Text HLabel 6150 4800 2    60   Input ~ 0
~PY_OK
Wire Wire Line
	6150 4800 6000 4800
Wire Wire Line
	6000 5000 6150 5000
Wire Wire Line
	6000 5200 6150 5200
Wire Wire Line
	4750 4800 4850 4800
Wire Wire Line
	4750 5000 4850 5000
Wire Wire Line
	4750 5200 4850 5200
Wire Wire Line
	4750 5400 4850 5400
$EndSCHEMATC
