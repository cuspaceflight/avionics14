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
S 4550 1300 800  950 
U 5476453E
F0 "microSD" 50
F1 "uSD.sch" 50
F2 "Card Detect" O R 5350 2100 60 
F3 "CLK" I R 5350 1450 60 
F4 "CMD" B R 5350 1600 60 
F5 "DAT0" B L 4550 1450 60 
F6 "DAT1" B L 4550 1600 60 
F7 "DAT2" B L 4550 1750 60 
F8 "DAT3" B L 4550 1900 60 
$EndSheet
$Sheet
S 2050 1300 1300 950 
U 5464CBD3
F0 "pyro chn" 50
F1 "pyro chn.sch" 50
F2 "PY2_TRG" I L 2050 1700 60 
F3 "PY4_TRG" I L 2050 2100 60 
F4 "PY1_TRG" I L 2050 1500 60 
F5 "PY3_TRG" I L 2050 1900 60 
F6 "~PY1_CHK" O R 3350 1500 60 
F7 "~PY2_CHK" O R 3350 1700 60 
F8 "~PY3_CHK" O R 3350 1900 60 
F9 "~PY4_CHK" O R 3350 2100 60 
$EndSheet
$Sheet
S 6150 1500 1150 850 
U 5478FE09
F0 "Indicators" 50
F1 "Indicators.sch" 50
F2 "PY_OK" I L 6150 1600 60 
F3 "IMU_OK" I L 6150 1800 60 
F4 "RADIO_OK" I L 6150 2000 60 
F5 "~PY_OK" I R 7300 1600 60 
F6 "~IMU_OK" I R 7300 1800 60 
F7 "~RADIO_OK" I R 7300 2000 60 
F8 "BUZZER" I L 6150 2200 60 
$EndSheet
$EndSCHEMATC
