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
LIBS:Custom
LIBS:ucboard-cache
EELAYER 25 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 1 1
Title "ucboard (PS Echtzeitsysteme): µC-Platine"
Date "2016-07-17"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L FT232RL IC1
U 1 1 578BBFC0
P 13300 2025
F 0 "IC1" H 13300 3075 60  0000 C CNN
F 1 "FT232RL" H 13300 975 60  0000 C CNN
F 2 "Housings_SSOP:SSOP-28_5.3x10.2mm_Pitch0.65mm" H 13300 2025 60  0001 C CNN
F 3 "http://www.farnell.com/datasheets/2007793.pdf" H 13300 2025 60  0001 C CNN
F 4 "Farnell" H 13300 2025 60  0001 C CNN "Supply"
F 5 "1146032" H 13300 2025 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/ftdi/ft232rl-reel/usb-uart-smd-ssop28-232/dp/1146032" H 13300 2025 60  0001 C CNN "URL"
F 7 "\"4,25\"" H 13300 2025 60  0001 C CNN "Price"
F 8 "SSOP-28" H 13300 2025 60  0001 C CNN "Package"
	1    13300 2025
	1    0    0    -1  
$EndComp
Text Label 15450 3850 0    60   ~ 0
UART4_RX
Text Label 15450 3950 0    60   ~ 0
UART4_TX
Text Label 14450 1325 0    60   ~ 0
RESET#
Text Label 14450 1525 0    60   ~ 0
BOOT0
$Comp
L LED D5
U 1 1 578BC436
P 14650 2025
F 0 "D5" H 14650 2125 50  0000 C CNN
F 1 "LED" H 14650 1925 50  0000 C CNN
F 2 "Custom:LED_0603_HandSoldering" H 14650 2025 50  0001 C CNN
F 3 "" H 14650 2025 50  0000 C CNN
	1    14650 2025
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR120
U 1 1 578BC578
P 14000 3175
F 0 "#PWR120" H 14000 2925 50  0001 C CNN
F 1 "GND" H 14000 3025 50  0000 C CNN
F 2 "" H 14000 3175 50  0000 C CNN
F 3 "" H 14000 3175 50  0000 C CNN
	1    14000 3175
	1    0    0    -1  
$EndComp
Text Label 10950 1175 0    59   ~ 0
USBDPOS
Text Label 10950 1075 0    59   ~ 0
USBDNEG
$Comp
L C_Small C31
U 1 1 578BCE33
P 10675 2525
F 0 "C31" H 10685 2595 50  0000 L CNN
F 1 "100n" H 10685 2445 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 10675 2525 50  0001 C CNN
F 3 "" H 10675 2525 50  0000 C CNN
F 4 "6V3" H 10675 2525 60  0001 C CNN "Spec"
	1    10675 2525
	1    0    0    -1  
$EndComp
$Comp
L C_Small C36
U 1 1 578BCEF8
P 11075 2525
F 0 "C36" H 11085 2595 50  0000 L CNN
F 1 "100n" H 11085 2445 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 11075 2525 50  0001 C CNN
F 3 "" H 11075 2525 50  0000 C CNN
F 4 "6V3" H 11075 2525 60  0001 C CNN "Spec"
	1    11075 2525
	1    0    0    -1  
$EndComp
Text Label 12750 2475 2    59   ~ 0
USBDPOS
Text Label 12750 2575 2    59   ~ 0
USBDNEG
$Comp
L C_Small C38
U 1 1 578BD03A
P 11950 2525
F 0 "C38" H 11960 2595 50  0000 L CNN
F 1 "100n" H 11960 2445 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 11950 2525 50  0001 C CNN
F 3 "" H 11950 2525 50  0000 C CNN
F 4 "6V3" H 11950 2525 60  0001 C CNN "Spec"
	1    11950 2525
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR94
U 1 1 578BDB30
P 12050 3225
F 0 "#PWR94" H 12050 2975 50  0001 C CNN
F 1 "GND" H 12050 3075 50  0000 C CNN
F 2 "" H 12050 3225 50  0000 C CNN
F 3 "" H 12050 3225 50  0000 C CNN
	1    12050 3225
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR87
U 1 1 578CC232
P 10700 1600
F 0 "#PWR87" H 10700 1350 50  0001 C CNN
F 1 "GND" H 10700 1450 50  0000 C CNN
F 2 "" H 10700 1600 50  0000 C CNN
F 3 "" H 10700 1600 50  0000 C CNN
	1    10700 1600
	1    0    0    -1  
$EndComp
Text Notes 10000 750  0    60   ~ 0
USB-UART-Wandler
$Comp
L MAX3232 U8
U 1 1 578E1D29
P 12125 4325
F 0 "U8" H 12125 4775 60  0000 C CNN
F 1 "MAX3232" H 12125 3875 60  0000 C CNN
F 2 "Housings_SOIC:SOIC-16_3.9x9.9mm_Pitch1.27mm" H 12125 4325 60  0001 C CNN
F 3 "http://www.farnell.com/datasheets/2020372.pdf" H 12125 4325 60  0001 C CNN
F 4 "MAX3232CDR" H 12125 4325 60  0001 C CNN "Spec"
F 5 "Farnell" H 12125 4325 60  0001 C CNN "Supply"
F 6 "1564957 " H 12125 4325 60  0001 C CNN "Part No."
F 7 "http://de.farnell.com/texas-instruments/max3232cdr/driver-receiver-rs232-3-5-5v/dp/1564957" H 12125 4325 60  0001 C CNN "URL"
F 8 "\"1,33\"" H 12125 4325 60  0001 C CNN "Price"
F 9 "SOIC16" H 12125 4325 60  0001 C CNN "Package"
	1    12125 4325
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR84
U 1 1 578E32CA
P 10575 4125
F 0 "#PWR84" H 10575 3875 50  0001 C CNN
F 1 "GND" H 10575 3975 50  0000 C CNN
F 2 "" H 10575 4125 50  0000 C CNN
F 3 "" H 10575 4125 50  0000 C CNN
	1    10575 4125
	1    0    0    -1  
$EndComp
$Comp
L C_Small C28
U 1 1 578E339F
P 10125 4125
F 0 "C28" H 10135 4195 50  0000 L CNN
F 1 "100n" H 10135 4045 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 10125 4125 50  0001 C CNN
F 3 "" H 10125 4125 50  0000 C CNN
	1    10125 4125
	1    0    0    -1  
$EndComp
$Comp
L C_Small C32
U 1 1 578E3559
P 10725 4075
F 0 "C32" V 10675 3900 50  0000 L CNN
F 1 "100n" V 10775 3850 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 10725 4075 50  0001 C CNN
F 3 "" H 10725 4075 50  0000 C CNN
	1    10725 4075
	0    -1   -1   0   
$EndComp
$Comp
L C_Small C33
U 1 1 578E3AF4
P 10725 4400
F 0 "C33" H 10735 4470 50  0000 L CNN
F 1 "100n" H 10735 4320 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 10725 4400 50  0001 C CNN
F 3 "" H 10725 4400 50  0000 C CNN
	1    10725 4400
	1    0    0    -1  
$EndComp
$Comp
L C_Small C35
U 1 1 578E3D94
P 11000 4600
F 0 "C35" H 11010 4670 50  0000 L CNN
F 1 "100n" H 11010 4520 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 11000 4600 50  0001 C CNN
F 3 "" H 11000 4600 50  0000 C CNN
	1    11000 4600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR89
U 1 1 578E3EE2
P 11000 4750
F 0 "#PWR89" H 11000 4500 50  0001 C CNN
F 1 "GND" H 11000 4600 50  0000 C CNN
F 2 "" H 11000 4750 50  0000 C CNN
F 3 "" H 11000 4750 50  0000 C CNN
	1    11000 4750
	1    0    0    -1  
$EndComp
$Comp
L C_Small C39
U 1 1 578E532F
P 13050 3975
F 0 "C39" H 13060 4045 50  0000 L CNN
F 1 "100n" H 13060 3895 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 13050 3975 50  0001 C CNN
F 3 "" H 13050 3975 50  0000 C CNN
	1    13050 3975
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR103
U 1 1 578E57E2
P 13250 4075
F 0 "#PWR103" H 13250 3825 50  0001 C CNN
F 1 "GND" H 13250 3925 50  0000 C CNN
F 2 "" H 13250 4075 50  0000 C CNN
F 3 "" H 13250 4075 50  0000 C CNN
	1    13250 4075
	0    -1   -1   0   
$EndComp
Text Label 14450 975  0    60   ~ 0
USART2_RX
Text Label 14450 1225 0    60   ~ 0
USART2_TX
Text Notes 10000 3650 0    60   ~ 0
RS232-UART-Wandler
NoConn ~ 13850 1425
NoConn ~ 13850 1625
NoConn ~ 13850 1725
NoConn ~ 13850 1825
NoConn ~ 13850 2125
NoConn ~ 13850 2225
NoConn ~ 13850 2325
NoConn ~ 13850 2425
NoConn ~ 12750 1525
NoConn ~ 12750 1625
NoConn ~ 11575 4675
NoConn ~ 12675 4675
$Comp
L Q_NMOS_GSD Q3
U 1 1 5790D58D
P 5450 4100
F 0 "Q3" H 5750 4150 50  0000 R CNN
F 1 "IRLML6344" V 5700 4075 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 5650 4200 50  0001 C CNN
F 3 "http://www.farnell.com/datasheets/1911844.pdf" H 5450 4100 50  0001 C CNN
F 4 "Farnell" H 5450 4100 60  0001 C CNN "Supply"
F 5 "1857299" H 5450 4100 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/international-rectifier/irlml6344trpbf/mosfet-n-kanal-30v-5a-sot23/dp/1857299" H 5450 4100 60  0001 C CNN "URL"
F 7 "\"0,168\"" H 5450 4100 60  0001 C CNN "Price"
F 8 "SOT-23" H 5450 4100 60  0001 C CNN "Package"
	1    5450 4100
	-1   0    0    -1  
$EndComp
$Comp
L R R6
U 1 1 5790D62A
P 4950 4350
F 0 "R6" V 5030 4350 50  0000 C CNN
F 1 "56k" V 4950 4350 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 4880 4350 50  0001 C CNN
F 3 "" H 4950 4350 50  0000 C CNN
F 4 "\"0,1%\"" H 4950 4350 60  0001 C CNN "Spec"
	1    4950 4350
	1    0    0    -1  
$EndComp
Text Notes 4350 2900 0    60   ~ 0
Batteriespannungsmessung
$Comp
L TSR_1- U4
U 1 1 579157CE
P 7025 1175
F 0 "U4" H 7025 1425 60  0000 C CNN
F 1 "TSR_1-2450" H 7025 1225 60  0000 C CNN
F 2 "Custom:TSR1" H 7025 1225 60  0001 C CNN
F 3 "http://www.tracopower.com/products/tsr1.pdf" H 7025 1225 60  0001 C CNN
F 4 "Farnell" H 7025 1175 60  0001 C CNN "Supply"
F 5 "1696320" H 7025 1175 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/tracopower/tsr-1-2450/wandler-dc-dc-5v-1a-sip/dp/1696320?selectedCategoryId=&exaMfpn=true&categoryId=&searchRef=SearchLookAhead&searchView=table&iscrfnonsku=false" H 7025 1175 60  0001 C CNN "URL"
F 7 "\"4,4\"" H 7025 1175 60  0001 C CNN "Price"
	1    7025 1175
	1    0    0    -1  
$EndComp
$Comp
L C_Small C16
U 1 1 579157D5
P 5650 1275
F 0 "C16" H 5660 1345 50  0000 L CNN
F 1 "4u7" H 5660 1195 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206_HandSoldering" H 5650 1275 50  0001 C CNN
F 3 "" H 5650 1275 50  0000 C CNN
F 4 "Farnell" H 5650 1275 60  0001 C CNN "Supply"
F 5 "1828835" H 5650 1275 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/murata/grm31cr71e475ka88l/kondensator-mlcc-x7r-4-7uf-25v/dp/1828835" H 5650 1275 60  0001 C CNN "URL"
F 7 "\"0,154\"" H 5650 1275 60  0001 C CNN "Price"
F 8 "1206" H 5650 1275 60  0001 C CNN "Package"
F 9 "25V" H 5650 1275 60  0001 C CNN "Spec"
	1    5650 1275
	1    0    0    -1  
$EndComp
$Comp
L C_Small C27
U 1 1 57915802
P 9550 2275
F 0 "C27" H 9560 2345 50  0000 L CNN
F 1 "100n" H 9560 2195 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 9550 2275 50  0001 C CNN
F 3 "" H 9550 2275 50  0000 C CNN
	1    9550 2275
	1    0    0    -1  
$EndComp
$Comp
L C_Small C20
U 1 1 57915809
P 7825 2275
F 0 "C20" H 7835 2345 50  0000 L CNN
F 1 "1u" H 7835 2195 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 7825 2275 50  0001 C CNN
F 3 "" H 7825 2275 50  0000 C CNN
F 4 "Farnell" H 7825 2275 60  0001 C CNN "Supply"
F 5 "2346901" H 7825 2275 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/tdk/c1608x7r1e105k080ab/kondensator-mlcc-x7r-1uf-25v-0603/dp/2346901" H 7825 2275 60  0001 C CNN "URL"
F 7 "\"0,0858\"" H 7825 2275 60  0001 C CNN "Price"
F 8 "0603" H 7825 2275 60  0001 C CNN "Package"
F 9 "6V3" H 7825 2275 60  0001 C CNN "Spec"
	1    7825 2275
	1    0    0    -1  
$EndComp
$Comp
L C_Small C17
U 1 1 57915810
P 6025 1275
F 0 "C17" H 6035 1345 50  0000 L CNN
F 1 "4u7" H 6035 1195 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206_HandSoldering" H 6025 1275 50  0001 C CNN
F 3 "" H 6025 1275 50  0000 C CNN
F 4 "Farnell" H 6025 1275 60  0001 C CNN "Supply"
F 5 "1828835" H 6025 1275 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/murata/grm31cr71e475ka88l/kondensator-mlcc-x7r-4-7uf-25v/dp/1828835" H 6025 1275 60  0001 C CNN "URL"
F 7 "\"0,154\"" H 6025 1275 60  0001 C CNN "Price"
F 8 "1206" H 6025 1275 60  0001 C CNN "Package"
F 9 "25V" H 6025 1275 60  0001 C CNN "Spec"
	1    6025 1275
	1    0    0    -1  
$EndComp
$Comp
L L_Small L1
U 1 1 57915817
P 5825 1025
F 0 "L1" H 5855 1065 50  0000 L CNN
F 1 "8u2" H 5855 985 50  0000 L CNN
F 2 "Custom:SRN6045" H 5825 1025 50  0001 C CNN
F 3 "http://www.farnell.com/datasheets/1875285.pdf" H 5825 1025 50  0001 C CNN
F 4 "Farnell" H 5825 1025 60  0001 C CNN "Supply"
F 5 "2061651" H 5825 1025 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/bourns/srn6045-8r2y/leistungsinduktivit-t-8-2uh-halb/dp/2061651?ost=2061651&selectedCategoryId=&categoryNameResp=Alle%2BKategorien&searchView=table&iscrfnonsku=false" H 5825 1025 60  0001 C CNN "URL"
F 7 "\"0,512\"" H 5825 1025 60  0001 C CNN "Price"
	1    5825 1025
	0    -1   -1   0   
$EndComp
$Comp
L C_Small C23
U 1 1 5791585E
P 8425 1275
F 0 "C23" H 8435 1345 50  0000 L CNN
F 1 "1u" H 8435 1195 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 8425 1275 50  0001 C CNN
F 3 "" H 8425 1275 50  0000 C CNN
F 4 "Farnell" H 8425 1275 60  0001 C CNN "Supply"
F 5 "2346901" H 8425 1275 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/tdk/c1608x7r1e105k080ab/kondensator-mlcc-x7r-1uf-25v-0603/dp/2346901" H 8425 1275 60  0001 C CNN "URL"
F 7 "\"0,0858\"" H 8425 1275 60  0001 C CNN "Price"
F 8 "0603" H 8425 1275 60  0001 C CNN "Package"
F 9 "6V3" H 8425 1275 60  0001 C CNN "Spec"
	1    8425 1275
	1    0    0    -1  
$EndComp
$Comp
L C_Small C26
U 1 1 57915865
P 8800 1275
F 0 "C26" H 8810 1345 50  0000 L CNN
F 1 "100n" H 8810 1195 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 8800 1275 50  0001 C CNN
F 3 "" H 8800 1275 50  0000 C CNN
F 4 "6V3" H 8800 1275 60  0001 C CNN "Spec"
	1    8800 1275
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG4
U 1 1 57915884
P 6050 2500
F 0 "#FLG4" H 6050 2595 50  0001 C CNN
F 1 "PWR_FLAG" H 6050 2680 50  0000 C CNN
F 2 "" H 6050 2500 50  0000 C CNN
F 3 "" H 6050 2500 50  0000 C CNN
	1    6050 2500
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG3
U 1 1 5791588A
P 5425 975
F 0 "#FLG3" H 5425 1070 50  0001 C CNN
F 1 "PWR_FLAG" H 5425 1155 50  0000 C CNN
F 2 "" H 5425 975 50  0000 C CNN
F 3 "" H 5425 975 50  0000 C CNN
	1    5425 975 
	1    0    0    -1  
$EndComp
Text Notes 4350 750  0    60   ~ 0
Spannungsversorgung
$Comp
L CONN_01X03 P10
U 1 1 5791EAC0
P 8325 3200
F 0 "P10" H 8325 3400 50  0000 C CNN
F 1 "BATTERY 2" V 8425 3200 50  0000 C CNN
F 2 "Custom:PS25_3G" H 8325 3200 50  0001 C CNN
F 3 "" H 8325 3200 50  0000 C CNN
F 4 "Reichelt" H 8325 3200 60  0001 C CNN "Supply"
F 5 "PS25 3G" H 8325 3200 60  0001 C CNN "Part No."
F 6 "http://www.reichelt.de/index.html?ACTION=3;ARTICLE=40274;SEARCH=PS%2025/3G%20BR" H 8325 3200 60  0001 C CNN "URL"
F 7 "\"0,48\"" H 8325 3200 60  0001 C CNN "Price"
F 8 "1x3 Header" H 8325 3200 60  0001 C CNN "Package"
	1    8325 3200
	-1   0    0    -1  
$EndComp
Text Label 8525 3300 0    60   ~ 0
CTL_BATT2_OUT
Text Label 4725 1425 2    60   ~ 0
PPC_+V
Text Label 5775 2550 2    60   ~ 0
PPC_-V_PWR
$Comp
L +3V3 #PWR78
U 1 1 579217D5
P 9700 1975
F 0 "#PWR78" H 9700 1825 50  0001 C CNN
F 1 "+3V3" H 9700 2115 50  0000 C CNN
F 2 "" H 9700 1975 50  0000 C CNN
F 3 "" H 9700 1975 50  0000 C CNN
	1    9700 1975
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR57
U 1 1 5792184F
P 8025 1975
F 0 "#PWR57" H 8025 1825 50  0001 C CNN
F 1 "+5V" H 8025 2115 50  0000 C CNN
F 2 "" H 8025 1975 50  0000 C CNN
F 3 "" H 8025 1975 50  0000 C CNN
	1    8025 1975
	1    0    0    -1  
$EndComp
Text Label 6775 3025 0    60   ~ 0
BATT2_+V_MEAS
Text Label 12675 4375 0    60   ~ 0
USART3_RX
Text Label 12675 4475 0    60   ~ 0
USART3_TX
$Comp
L GND #PWR145
U 1 1 5792EE00
P 15450 4050
F 0 "#PWR145" H 15450 3800 50  0001 C CNN
F 1 "GND" H 15450 3900 50  0000 C CNN
F 2 "" H 15450 4050 50  0000 C CNN
F 3 "" H 15450 4050 50  0000 C CNN
	1    15450 4050
	0    -1   -1   0   
$EndComp
$Comp
L C_Small C34
U 1 1 57934E4F
P 10875 8200
F 0 "C34" H 10885 8270 50  0000 L CNN
F 1 "100n" H 10885 8120 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 10875 8200 50  0001 C CNN
F 3 "" H 10875 8200 50  0000 C CNN
F 4 "X7R 2V" H 10875 8200 60  0001 C CNN "Spec"
	1    10875 8200
	1    0    0    -1  
$EndComp
$Comp
L C_Small C30
U 1 1 5793571F
P 10575 7675
F 0 "C30" H 10585 7745 50  0000 L CNN
F 1 "100n" H 10585 7595 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 10575 7675 50  0001 C CNN
F 3 "" H 10575 7675 50  0000 C CNN
F 4 "X7R 4V" H 10575 7675 60  0001 C CNN "Spec"
	1    10575 7675
	1    0    0    -1  
$EndComp
$Comp
L C_Small C29
U 1 1 57935BB2
P 10250 7675
F 0 "C29" H 10260 7745 50  0000 L CNN
F 1 "10n" H 10260 7595 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 10250 7675 50  0001 C CNN
F 3 "" H 10250 7675 50  0000 C CNN
F 4 "X7R 4V" H 10250 7675 60  0001 C CNN "Spec"
	1    10250 7675
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR88
U 1 1 57935E8D
P 10875 8350
F 0 "#PWR88" H 10875 8100 50  0001 C CNN
F 1 "GND" H 10875 8200 50  0000 C CNN
F 2 "" H 10875 8350 50  0000 C CNN
F 3 "" H 10875 8350 50  0000 C CNN
	1    10875 8350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR83
U 1 1 57935F48
P 10250 8350
F 0 "#PWR83" H 10250 8100 50  0001 C CNN
F 1 "GND" H 10250 8200 50  0000 C CNN
F 2 "" H 10250 8350 50  0000 C CNN
F 3 "" H 10250 8350 50  0000 C CNN
	1    10250 8350
	1    0    0    -1  
$EndComp
NoConn ~ 12425 7325
NoConn ~ 12425 7725
$Comp
L GND #PWR97
U 1 1 5793740D
P 12450 7525
F 0 "#PWR97" H 12450 7275 50  0001 C CNN
F 1 "GND" H 12450 7375 50  0000 C CNN
F 2 "" H 12450 7525 50  0000 C CNN
F 3 "" H 12450 7525 50  0000 C CNN
	1    12450 7525
	0    -1   -1   0   
$EndComp
Text Notes 10000 7050 0    60   ~ 0
MPU-9250 SPI Accelerometer-Gyroskop-Magnetometer
$Comp
L +3V3 #PWR82
U 1 1 5793C33B
P 10250 7300
F 0 "#PWR82" H 10250 7150 50  0001 C CNN
F 1 "+3V3" H 10250 7440 50  0000 C CNN
F 2 "" H 10250 7300 50  0000 C CNN
F 3 "" H 10250 7300 50  0000 C CNN
	1    10250 7300
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR85
U 1 1 5793C47C
P 10575 7300
F 0 "#PWR85" H 10575 7150 50  0001 C CNN
F 1 "+3V3" H 10575 7440 50  0000 C CNN
F 2 "" H 10575 7300 50  0000 C CNN
F 3 "" H 10575 7300 50  0000 C CNN
	1    10575 7300
	1    0    0    -1  
$EndComp
$Comp
L MPU-9250 U7
U 1 1 5793D14D
P 11725 7675
F 0 "U7" H 11725 8175 60  0000 C CNN
F 1 "MPU-9250" H 11725 7175 60  0000 C CNN
F 2 "Custom:MPU-9250" H 11725 7675 60  0001 C CNN
F 3 "https://store.invensense.com/datasheets/invensense/MPU9250REV1.0.pdf" H 11725 7675 60  0001 C CNN
F 4 "Digikey" H 11725 7675 60  0001 C CNN "Supply"
F 5 "1428-1019-1-ND " H 11725 7675 60  0001 C CNN "Part No."
F 6 "http://www.digikey.de/product-detail/de/invensense/MPU-9250/1428-1019-1-ND/4626450" H 11725 7675 60  0001 C CNN "URL"
F 7 "\"9,58\"" H 11725 7675 60  0001 C CNN "Price"
F 8 "QFN-24 3x3" H 11725 7675 60  0001 C CNN "Package"
	1    11725 7675
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X07 P18
U 1 1 57940067
P 13500 7325
F 0 "P18" H 13500 7725 50  0000 C CNN
F 1 "MPU-9250_EXT" V 13600 7325 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x07" H 13500 7325 50  0001 C CNN
F 3 "" H 13500 7325 50  0000 C CNN
F 4 "1x7 Header" H 13500 7325 60  0001 C CNN "Package"
	1    13500 7325
	-1   0    0    -1  
$EndComp
$Comp
L +5V #PWR109
U 1 1 579401C8
P 13725 7025
F 0 "#PWR109" H 13725 6875 50  0001 C CNN
F 1 "+5V" H 13725 7165 50  0000 C CNN
F 2 "" H 13725 7025 50  0000 C CNN
F 3 "" H 13725 7025 50  0000 C CNN
	1    13725 7025
	-1   0    0    -1  
$EndComp
$Comp
L GND #PWR119
U 1 1 57940356
P 13900 7025
F 0 "#PWR119" H 13900 6775 50  0001 C CNN
F 1 "GND" H 13900 6875 50  0000 C CNN
F 2 "" H 13900 7025 50  0000 C CNN
F 3 "" H 13900 7025 50  0000 C CNN
	1    13900 7025
	1    0    0    1   
$EndComp
Text Notes 13300 6775 0    60   ~ 0
MPU-9250 SPI Extern
$Comp
L CONN_01X05 P19
U 1 1 5794370E
P 13500 8275
F 0 "P19" H 13500 8575 50  0000 C CNN
F 1 "SFR08_I2C" V 13600 8275 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x05" H 13500 8275 50  0001 C CNN
F 3 "" H 13500 8275 50  0000 C CNN
F 4 "1x5 Header" H 13500 8275 60  0001 C CNN "Package"
	1    13500 8275
	-1   0    0    -1  
$EndComp
Text Label 13700 8175 0    60   Italic 0
I2C1_SDA
Text Label 13700 8275 0    60   Italic 0
I2C1_SCL
NoConn ~ 13700 8375
$Comp
L GND #PWR110
U 1 1 57944557
P 13725 8475
F 0 "#PWR110" H 13725 8225 50  0001 C CNN
F 1 "GND" H 13725 8325 50  0000 C CNN
F 2 "" H 13725 8475 50  0000 C CNN
F 3 "" H 13725 8475 50  0000 C CNN
	1    13725 8475
	0    -1   -1   0   
$EndComp
$Comp
L CONN_01X03 P2
U 1 1 579445EE
P 4700 8725
F 0 "P2" H 4700 8925 50  0000 C CNN
F 1 "PWM_SERVO" V 4800 8725 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03" H 4700 8725 50  0001 C CNN
F 3 "" H 4700 8725 50  0000 C CNN
F 4 "1x3 Header" H 4700 8725 60  0001 C CNN "Package"
	1    4700 8725
	-1   0    0    -1  
$EndComp
$Comp
L CONN_01X02 P3
U 1 1 57944BC4
P 4700 9200
F 0 "P3" H 4700 9350 50  0000 C CNN
F 1 "PWM_DRIVE" V 4800 9200 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02" H 4700 9200 50  0001 C CNN
F 3 "" H 4700 9200 50  0000 C CNN
F 4 "1x2 Header" H 4700 9200 60  0001 C CNN "Package"
	1    4700 9200
	-1   0    0    -1  
$EndComp
$Comp
L SERVO_+5V #PWR21
U 1 1 5794CEC9
P 5550 8725
F 0 "#PWR21" H 5550 8575 50  0001 C CNN
F 1 "SERVO_+5V" H 5550 8865 50  0000 C CNN
F 2 "" H 5550 8725 50  0000 C CNN
F 3 "" H 5550 8725 50  0000 C CNN
	1    5550 8725
	0    1    -1   0   
$EndComp
$Comp
L SERVO_+5V #PWR77
U 1 1 5794E83E
P 9700 975
F 0 "#PWR77" H 9700 825 50  0001 C CNN
F 1 "SERVO_+5V" H 9700 1115 50  0000 C CNN
F 2 "" H 9700 975 50  0000 C CNN
F 3 "" H 9700 975 50  0000 C CNN
	1    9700 975 
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR15
U 1 1 579502CC
P 5100 8825
F 0 "#PWR15" H 5100 8575 50  0001 C CNN
F 1 "GND" H 5100 8675 50  0000 C CNN
F 2 "" H 5100 8825 50  0000 C CNN
F 3 "" H 5100 8825 50  0000 C CNN
	1    5100 8825
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR16
U 1 1 57950E2C
P 5100 9250
F 0 "#PWR16" H 5100 9000 50  0001 C CNN
F 1 "GND" H 5100 9100 50  0000 C CNN
F 2 "" H 5100 9250 50  0000 C CNN
F 3 "" H 5100 9250 50  0000 C CNN
	1    5100 9250
	0    -1   -1   0   
$EndComp
Text Notes 7650 5750 0    60   ~ 0
Zusätzliche Timer Ausgänge
Text Notes 4500 8375 0    60   ~ 0
PWM Ausgänge\nLenkservos und\nAntriebsmotor
Text Notes 13300 7850 0    60   ~ 0
Ultraschallsensoren I2C
Text Notes 7600 7300 0    60   ~ 0
Hallsensor Analog
$Comp
L GND #PWR116
U 1 1 5795635F
P 13900 5825
F 0 "#PWR116" H 13900 5575 50  0001 C CNN
F 1 "GND" H 13900 5675 50  0000 C CNN
F 2 "" H 13900 5825 50  0000 C CNN
F 3 "" H 13900 5825 50  0000 C CNN
	1    13900 5825
	-1   0    0    -1  
$EndComp
$Comp
L GND #PWR118
U 1 1 5795AFB0
P 13900 6400
F 0 "#PWR118" H 13900 6150 50  0001 C CNN
F 1 "GND" H 13900 6250 50  0000 C CNN
F 2 "" H 13900 6400 50  0000 C CNN
F 3 "" H 13900 6400 50  0000 C CNN
	1    13900 6400
	-1   0    0    -1  
$EndComp
Text Notes 12975 5325 0    60   ~ 0
Zusätzliche I2C
$Comp
L GND #PWR147
U 1 1 5796E268
P 15475 5250
F 0 "#PWR147" H 15475 5000 50  0001 C CNN
F 1 "GND" H 15475 5100 50  0000 C CNN
F 2 "" H 15475 5250 50  0000 C CNN
F 3 "" H 15475 5250 50  0000 C CNN
	1    15475 5250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR149
U 1 1 5796E55D
P 15475 6150
F 0 "#PWR149" H 15475 5900 50  0001 C CNN
F 1 "GND" H 15475 6000 50  0000 C CNN
F 2 "" H 15475 6150 50  0000 C CNN
F 3 "" H 15475 6150 50  0000 C CNN
	1    15475 6150
	1    0    0    -1  
$EndComp
Text Notes 14500 4400 0    60   ~ 0
Zusätzliche SPI
Text Label 6675 8975 0    60   ~ 0
CAR_ID_1
Text Label 6675 9075 0    60   ~ 0
CAR_ID_2
Text Label 6675 9175 0    60   ~ 0
CAR_ID_3
Text Label 6675 9275 0    60   ~ 0
CAR_ID_4
Text Notes 5875 8750 0    60   ~ 0
Car-ID Einstellung
Text Notes 11825 5325 0    60   ~ 0
Zusätzliche GPIO
$Comp
L GND #PWR50
U 1 1 57981A63
P 7825 2575
F 0 "#PWR50" H 7825 2325 50  0001 C CNN
F 1 "GND" H 7825 2425 50  0000 C CNN
F 2 "" H 7825 2575 50  0000 C CNN
F 3 "" H 7825 2575 50  0000 C CNN
	1    7825 2575
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR34
U 1 1 5798227F
P 6500 2600
F 0 "#PWR34" H 6500 2350 50  0001 C CNN
F 1 "GND" H 6500 2450 50  0000 C CNN
F 2 "" H 6500 2600 50  0000 C CNN
F 3 "" H 6500 2600 50  0000 C CNN
	1    6500 2600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR64
U 1 1 5798233B
P 8700 2575
F 0 "#PWR64" H 8700 2325 50  0001 C CNN
F 1 "GND" H 8700 2425 50  0000 C CNN
F 2 "" H 8700 2575 50  0000 C CNN
F 3 "" H 8700 2575 50  0000 C CNN
	1    8700 2575
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR75
U 1 1 579823F7
P 9550 2575
F 0 "#PWR75" H 9550 2325 50  0001 C CNN
F 1 "GND" H 9550 2425 50  0000 C CNN
F 2 "" H 9550 2575 50  0000 C CNN
F 3 "" H 9550 2575 50  0000 C CNN
	1    9550 2575
	1    0    0    -1  
$EndComp
$Comp
L CONN_02X10 P15
U 1 1 57951002
P 11000 6025
F 0 "P15" H 11000 6575 50  0000 C CNN
F 1 "JTAG" V 11000 6025 50  0000 C CNN
F 2 "Connect:IDC_Header_Straight_20pins" H 11000 4825 50  0001 C CNN
F 3 "" H 11000 4825 50  0000 C CNN
F 4 "Reichelt" H 11000 6025 60  0001 C CNN "Supply"
F 5 "WSL 20G" H 11000 6025 60  0001 C CNN "Part No."
F 6 "http://www.reichelt.de/WSL-20G/3/index.html?&ACTION=3&LA=446&ARTICLE=22825&artnr=WSL+20G&SEARCH=wannenstecker" H 11000 6025 60  0001 C CNN "URL"
F 7 "\"0,12\"" H 11000 6025 60  0001 C CNN "Price"
F 8 "2x10" H 11000 6025 60  0001 C CNN "Package"
	1    11000 6025
	1    0    0    -1  
$EndComp
Text Label 10750 5675 2    60   ~ 0
JTRST
Text Label 10750 5775 2    60   ~ 0
JTDI
Text Label 10750 5875 2    60   ~ 0
JTMS
Text Label 10750 5975 2    60   ~ 0
JTCK
Text Label 10750 6175 2    60   ~ 0
JTDO
$Comp
L GND #PWR91
U 1 1 57951FAE
P 11350 6550
F 0 "#PWR91" H 11350 6300 50  0001 C CNN
F 1 "GND" H 11350 6400 50  0000 C CNN
F 2 "" H 11350 6550 50  0000 C CNN
F 3 "" H 11350 6550 50  0000 C CNN
	1    11350 6550
	1    0    0    -1  
$EndComp
Text Notes 10000 5325 0    60   ~ 0
JTAG
Text Notes 4400 5750 0    60   ~ 0
Spannungsversorgung Kinect 12V
$Comp
L 25LC_EEPROM U9
U 1 1 579739FC
P 15075 9325
F 0 "U9" H 14775 9575 50  0000 L CNN
F 1 "25LC160A-I/SN" H 15125 9575 50  0000 L CNN
F 2 "Housings_SOIC:SOIC-8_3.9x4.9mm_Pitch1.27mm" H 14725 9275 50  0001 C CNN
F 3 "http://www.farnell.com/datasheets/129934.pdf" H 14725 9275 50  0001 C CNN
F 4 "Farnell" H 15075 9325 60  0001 C CNN "Supply"
F 5 "1084340" H 15075 9325 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/microchip/25lc160a-i-sn/eeprom-seriell-16kbit-10mhz-soic/dp/1084340" H 15075 9325 60  0001 C CNN "URL"
F 7 "\"0,565\"" H 15075 9325 60  0001 C CNN "Price"
F 8 "SOIC 8" H 15075 9325 60  0001 C CNN "Package"
	1    15075 9325
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR135
U 1 1 57976893
P 15075 8975
F 0 "#PWR135" H 15075 8825 50  0001 C CNN
F 1 "+3V3" H 15075 9115 50  0000 C CNN
F 2 "" H 15075 8975 50  0000 C CNN
F 3 "" H 15075 8975 50  0000 C CNN
	1    15075 8975
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR136
U 1 1 57976921
P 15075 9675
F 0 "#PWR136" H 15075 9425 50  0001 C CNN
F 1 "GND" H 15075 9525 50  0000 C CNN
F 2 "" H 15075 9675 50  0000 C CNN
F 3 "" H 15075 9675 50  0000 C CNN
	1    15075 9675
	1    0    0    -1  
$EndComp
$Comp
L CONN_02X05 P22
U 1 1 5797861C
P 13825 4625
F 0 "P22" H 13825 4925 50  0000 C CNN
F 1 "Serial Port" H 13825 4325 50  0000 C CNN
F 2 "Connect:IDC_Header_Straight_10pins" H 13825 3425 50  0001 C CNN
F 3 "" H 13825 3425 50  0000 C CNN
F 4 "Reichelt" H 13825 4625 60  0001 C CNN "Supply"
F 5 "WSL 10G" H 13825 4625 60  0001 C CNN "Part No."
F 6 "http://www.reichelt.de/WSL-10G/3/index.html?&ACTION=3&LA=446&ARTICLE=22816&artnr=WSL+10G&SEARCH=wannenstecker" H 13825 4625 60  0001 C CNN "URL"
F 7 "\"0,09\"" H 13825 4625 60  0001 C CNN "Price"
F 8 "2x5 Header" H 13825 4625 60  0001 C CNN "Package"
	1    13825 4625
	1    0    0    -1  
$EndComp
Text Notes 15575 8700 0    60   Italic 12
\n
$Comp
L USB_B P13
U 1 1 57987B73
P 10350 1175
F 0 "P13" H 10550 975 50  0000 C CNN
F 1 "USB_B" H 10300 1375 50  0000 C CNN
F 2 "Custom:USB_B" H 10300 1075 50  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C120/USBBW.pdf" H 10300 1075 50  0001 C CNN
F 4 "Reichelt" H 10350 1175 60  0001 C CNN "Supply"
F 5 "USB BW" H 10350 1175 60  0001 C CNN "Part No."
F 6 "http://www.reichelt.de/USB-Einbauverbinder/USB-BW/3/index.html?&ACTION=3&LA=2&ARTICLE=22186&GROUPID=7530&artnr=USB+BW" H 10350 1175 60  0001 C CNN "URL"
F 7 "\"0,26\"" H 10350 1175 60  0001 C CNN "Price"
	1    10350 1175
	0    -1   1    0   
$EndComp
$Comp
L GND #PWR92
U 1 1 5798BDB2
P 11525 1775
F 0 "#PWR92" H 11525 1525 50  0001 C CNN
F 1 "GND" H 11525 1625 50  0000 C CNN
F 2 "" H 11525 1775 50  0000 C CNN
F 3 "" H 11525 1775 50  0000 C CNN
	1    11525 1775
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR93
U 1 1 5799AF69
P 11950 1075
F 0 "#PWR93" H 11950 925 50  0001 C CNN
F 1 "+5V" H 11950 1215 50  0000 C CNN
F 2 "" H 11950 1075 50  0000 C CNN
F 3 "" H 11950 1075 50  0000 C CNN
	1    11950 1075
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR129
U 1 1 5799E4EA
P 14900 2025
F 0 "#PWR129" H 14900 1875 50  0001 C CNN
F 1 "+3V3" H 14900 2165 50  0000 C CNN
F 2 "" H 14900 2025 50  0000 C CNN
F 3 "" H 14900 2025 50  0000 C CNN
	1    14900 2025
	1    0    0    -1  
$EndComp
Text Label 1250 1225 2    60   ~ 0
RESET#
Text Label 1250 1325 2    60   ~ 0
BOOT0
Text Label 3750 1525 0    60   ~ 0
USART2_RX
Text Label 3750 1425 0    60   ~ 0
USART2_TX
Text Label 3750 9025 0    60   ~ 0
USART3_TX
Text Label 3750 9125 0    60   ~ 0
USART3_RX
Text Notes 14600 4250 0    60   ~ 0
5V Tolerant
Text Label 3750 3725 0    60   ~ 0
I2C1_SDA
Text Label 3750 3625 0    60   ~ 0
I2C1_SCL
Text Label 3750 2525 0    60   ~ 0
JTMS
Text Label 3750 2625 0    60   ~ 0
JTCK
Text Label 3750 2725 0    60   ~ 0
JTDI
Text Label 3750 3325 0    60   ~ 0
JTDO
Text Label 3750 3425 0    60   ~ 0
JTRST
$Comp
L +3V3 #PWR86
U 1 1 5799968D
P 10650 5525
F 0 "#PWR86" H 10650 5375 50  0001 C CNN
F 1 "+3V3" H 10650 5665 50  0000 C CNN
F 2 "" H 10650 5525 50  0000 C CNN
F 3 "" H 10650 5525 50  0000 C CNN
	1    10650 5525
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR90
U 1 1 57999756
P 11350 5525
F 0 "#PWR90" H 11350 5375 50  0001 C CNN
F 1 "+3V3" H 11350 5665 50  0000 C CNN
F 2 "" H 11350 5525 50  0000 C CNN
F 3 "" H 11350 5525 50  0000 C CNN
	1    11350 5525
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR81
U 1 1 5799ADA3
P 10150 6550
F 0 "#PWR81" H 10150 6300 50  0001 C CNN
F 1 "GND" H 10150 6400 50  0000 C CNN
F 2 "" H 10150 6550 50  0000 C CNN
F 3 "" H 10150 6550 50  0000 C CNN
	1    10150 6550
	1    0    0    -1  
$EndComp
Text Label 10750 6275 2    60   ~ 0
RESET#
$Comp
L SW_PUSH SW1
U 1 1 579A632C
P 1325 900
F 0 "SW1" H 1475 1010 50  0000 C CNN
F 1 "RESET" H 1325 820 50  0000 C CNN
F 2 "Custom:BUTTON" H 1325 900 50  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C200/JTP_1138_02.pdf" H 1325 900 50  0001 C CNN
F 4 "Reichelt" H 1325 900 60  0001 C CNN "Supply"
F 5 "TASTER 9314" H 1325 900 60  0001 C CNN "Part No."
F 6 "http://www.reichelt.de/Kurzhubtaster/TASTER-9314/3/index.html?&ACTION=3&LA=2&ARTICLE=44510&GROUPID=7587&artnr=TASTER+9314" H 1325 900 60  0001 C CNN "URL"
F 7 "\"0,22\"" H 1325 900 60  0001 C CNN "Price"
	1    1325 900 
	0    -1   -1   0   
$EndComp
$Comp
L C_Small C10
U 1 1 579A716D
P 1500 875
F 0 "C10" H 1510 945 50  0000 L CNN
F 1 "100n" V 1550 650 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 1500 875 50  0001 C CNN
F 3 "" H 1500 875 50  0000 C CNN
	1    1500 875 
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR9
U 1 1 579A7C40
P 1600 575
F 0 "#PWR9" H 1600 325 50  0001 C CNN
F 1 "GND" H 1600 425 50  0000 C CNN
F 2 "" H 1600 575 50  0000 C CNN
F 3 "" H 1600 575 50  0000 C CNN
	1    1600 575 
	0    -1   -1   0   
$EndComp
$Comp
L R_Small R38
U 1 1 579AB45E
P 10300 6075
F 0 "R38" H 10330 6095 50  0000 L CNN
F 1 "10k" H 10330 6035 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 10300 6075 50  0001 C CNN
F 3 "" H 10300 6075 50  0000 C CNN
	1    10300 6075
	0    -1   -1   0   
$EndComp
$Comp
L R_Small R39
U 1 1 579AB763
P 10300 6375
F 0 "R39" H 10330 6395 50  0000 L CNN
F 1 "10k" H 10330 6335 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 10300 6375 50  0001 C CNN
F 3 "" H 10300 6375 50  0000 C CNN
	1    10300 6375
	0    -1   -1   0   
$EndComp
$Comp
L R_Small R40
U 1 1 579AB97B
P 10300 6475
F 0 "R40" H 10330 6495 50  0000 L CNN
F 1 "10k" H 10330 6435 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 10300 6475 50  0001 C CNN
F 3 "" H 10300 6475 50  0000 C CNN
	1    10300 6475
	0    -1   1    0   
$EndComp
$Comp
L R_Small R49
U 1 1 579AE6A3
P 14100 2025
F 0 "R49" H 14130 2045 50  0000 L CNN
F 1 "680R" H 14130 1985 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 14100 2025 50  0001 C CNN
F 3 "" H 14100 2025 50  0000 C CNN
	1    14100 2025
	0    -1   -1   0   
$EndComp
$Comp
L R_Small R48
U 1 1 579AEAFA
P 14100 1525
F 0 "R48" H 14130 1545 50  0000 L CNN
F 1 "1k" H 14130 1485 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 14100 1525 50  0001 C CNN
F 3 "" H 14100 1525 50  0000 C CNN
	1    14100 1525
	0    -1   1    0   
$EndComp
$Comp
L R_Small R47
U 1 1 579AECC7
P 14100 1325
F 0 "R47" H 14130 1345 50  0000 L CNN
F 1 "1k" H 14130 1285 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 14100 1325 50  0001 C CNN
F 3 "" H 14100 1325 50  0000 C CNN
	1    14100 1325
	0    -1   1    0   
$EndComp
$Comp
L R_Small R42
U 1 1 579AF7F7
P 11525 1575
F 0 "R42" H 11555 1595 50  0000 L CNN
F 1 "10k" H 11555 1535 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 11525 1575 50  0001 C CNN
F 3 "" H 11525 1575 50  0000 C CNN
	1    11525 1575
	-1   0    0    -1  
$EndComp
$Comp
L R_Small R41
U 1 1 579AF90F
P 11525 1200
F 0 "R41" H 11555 1220 50  0000 L CNN
F 1 "4k7" H 11555 1160 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 11525 1200 50  0001 C CNN
F 3 "" H 11525 1200 50  0000 C CNN
	1    11525 1200
	-1   0    0    -1  
$EndComp
$Comp
L C_Small C8
U 1 1 579BA35B
P 1275 2800
F 0 "C8" H 1285 2870 50  0000 L CNN
F 1 "10n" H 1285 2720 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 1275 2800 50  0001 C CNN
F 3 "" H 1275 2800 50  0000 C CNN
	1    1275 2800
	1    0    0    -1  
$EndComp
$Comp
L C_Small C3
U 1 1 579BC0D8
P 725 3750
F 0 "C3" H 735 3820 50  0000 L CNN
F 1 "100n" H 735 3670 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 725 3750 50  0001 C CNN
F 3 "" H 725 3750 50  0000 C CNN
	1    725  3750
	1    0    0    -1  
$EndComp
$Comp
L C_Small C5
U 1 1 579BC1EE
P 950 3750
F 0 "C5" H 960 3820 50  0000 L CNN
F 1 "100n" H 960 3670 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 950 3750 50  0001 C CNN
F 3 "" H 950 3750 50  0000 C CNN
	1    950  3750
	1    0    0    -1  
$EndComp
$Comp
L C_Small C7
U 1 1 579BCBF7
P 1175 3750
F 0 "C7" H 1185 3820 50  0000 L CNN
F 1 "100n" H 1185 3670 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 1175 3750 50  0001 C CNN
F 3 "" H 1175 3750 50  0000 C CNN
	1    1175 3750
	1    0    0    -1  
$EndComp
$Comp
L C_Small C9
U 1 1 579BCCF9
P 1400 3750
F 0 "C9" H 1410 3820 50  0000 L CNN
F 1 "100n" H 1410 3670 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 1400 3750 50  0001 C CNN
F 3 "" H 1400 3750 50  0000 C CNN
	1    1400 3750
	1    0    0    -1  
$EndComp
$Comp
L C_Small C4
U 1 1 579BEA4D
P 775 2775
F 0 "C4" H 785 2845 50  0000 L CNN
F 1 "10n" H 785 2695 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 775 2775 50  0001 C CNN
F 3 "" H 775 2775 50  0000 C CNN
	1    775  2775
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR1
U 1 1 579BF1A1
P 550 4075
F 0 "#PWR1" H 550 3825 50  0001 C CNN
F 1 "GND" H 550 3925 50  0000 C CNN
F 2 "" H 550 4075 50  0000 C CNN
F 3 "" H 550 4075 50  0000 C CNN
	1    550  4075
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR8
U 1 1 579BF743
P 1500 2125
F 0 "#PWR8" H 1500 1975 50  0001 C CNN
F 1 "+3V3" H 1500 2265 50  0000 C CNN
F 2 "" H 1500 2125 50  0000 C CNN
F 3 "" H 1500 2125 50  0000 C CNN
	1    1500 2125
	1    0    0    -1  
$EndComp
Text Label 8000 5975 0    60   ~ 0
TIM1_CH1
Text Label 5200 8625 0    60   ~ 0
TIM2_CH1
Text Label 8800 5900 0    60   ~ 0
TIM8_CH1
Text Label 8800 6600 0    60   ~ 0
TIM1_CH3
$Comp
L GND #PWR53
U 1 1 57952BF4
P 8000 6175
F 0 "#PWR53" H 8000 5925 50  0001 C CNN
F 1 "GND" H 8000 6025 50  0000 C CNN
F 2 "" H 8000 6175 50  0000 C CNN
F 3 "" H 8000 6175 50  0000 C CNN
	1    8000 6175
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X03 P7
U 1 1 579A9E69
P 7800 6075
F 0 "P7" H 7800 6275 50  0000 C CNN
F 1 "Timer1" V 7900 6075 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03" H 7800 6075 50  0001 C CNN
F 3 "" H 7800 6075 50  0000 C CNN
F 4 "1x3 Header" H 7800 6075 60  0001 C CNN "Package"
	1    7800 6075
	-1   0    0    -1  
$EndComp
$Comp
L GND #PWR55
U 1 1 57952B89
P 8000 6750
F 0 "#PWR55" H 8000 6500 50  0001 C CNN
F 1 "GND" H 8000 6600 50  0000 C CNN
F 2 "" H 8000 6750 50  0000 C CNN
F 3 "" H 8000 6750 50  0000 C CNN
	1    8000 6750
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X03 P8
U 1 1 579AA309
P 7800 6650
F 0 "P8" H 7800 6850 50  0000 C CNN
F 1 "Timer3" V 7900 6650 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03" H 7800 6650 50  0001 C CNN
F 3 "" H 7800 6650 50  0000 C CNN
F 4 "1x3 Header" H 7800 6650 60  0001 C CNN "Package"
	1    7800 6650
	-1   0    0    -1  
$EndComp
$Comp
L GND #PWR70
U 1 1 579AAEB9
P 8800 7000
F 0 "#PWR70" H 8800 6750 50  0001 C CNN
F 1 "GND" H 8800 6850 50  0000 C CNN
F 2 "" H 8800 7000 50  0000 C CNN
F 3 "" H 8800 7000 50  0000 C CNN
	1    8800 7000
	1    0    0    -1  
$EndComp
Text Label 5200 9150 0    60   ~ 0
TIM2_CH2
Text Label 8800 6700 0    60   ~ 0
TIM1_CH4
Text Label 8800 6000 0    60   ~ 0
TIM8_CH2
Text Label 3750 1725 0    60   ~ 0
SPI1_SCK
Text Label 3750 1825 0    60   ~ 0
SPI1_MISO
Text Label 3750 1925 0    60   ~ 0
SPI1_MOSI
Text Label 3750 2025 0    60   ~ 0
I2C3_SCL
Text Label 3750 3525 0    60   ~ 0
I2C3_SDA
Text Label 3750 4425 0    60   ~ 0
SPI2_MISO
Text Label 3750 4525 0    60   ~ 0
SPI2_MOSI
Text Label 3750 4325 0    60   ~ 0
SPI2_SCK
Text Label 3750 2125 0    60   ~ 0
I2C2_SCL
Text Label 3750 2225 0    60   ~ 0
I2C2_SDA
Text Label 3750 7725 0    60   ~ 0
SPI4_SCK
Text Label 3750 7025 0    60   ~ 0
SPI4_MISO
Text Label 3750 7125 0    60   ~ 0
SPI4_MOSI
Text Label 3750 5825 0    60   ~ 0
UART4_TX
Text Label 3750 5925 0    60   ~ 0
UART4_RX
Text Label 3750 7425 0    60   ~ 0
TIM1_CH1
Text Label 3750 1225 0    60   ~ 0
TIM2_CH1
Text Label 3750 1325 0    60   ~ 0
TIM2_CH2
Text Label 3750 5425 0    60   ~ 0
TIM8_CH1
Text Label 3750 5525 0    60   ~ 0
TIM8_CH2
Text Label 3750 5025 0    60   ~ 0
TIM1_CH3
Text Label 3750 5125 0    60   ~ 0
TIM1_CH4
Text Label 3750 6725 0    60   ~ 0
TIM3_CH1
Text Label 8000 6550 0    60   ~ 0
TIM3_CH1
Text Label 3750 1625 0    60   ~ 0
SPI1_CS1
Text Label 3750 4025 0    60   ~ 0
SPI2_CS2
Text Label 3750 4125 0    60   ~ 0
SPI2_CS3
Text Label 3750 3925 0    60   ~ 0
SPI2_CS1
Text Label 3750 4225 0    60   ~ 0
SPI2_CS4
Text Label 3750 8225 0    60   ~ 0
PD0
Text Label 3750 8325 0    60   ~ 0
PD1
Text Label 3750 8425 0    60   ~ 0
PD2
Text Label 3750 8525 0    60   ~ 0
PD3
Text Label 3750 8625 0    60   ~ 0
PD4
Text Label 3750 8725 0    60   ~ 0
PD5
Text Label 3750 8825 0    60   ~ 0
PD6
Text Label 3750 8925 0    60   ~ 0
PD7
Text Label 12675 7925 0    60   ~ 0
SPI1_SCK
Text Label 12675 7425 0    60   ~ 0
SPI1_MISO
Text Label 12675 8025 0    60   ~ 0
SPI1_MOSI
Text Label 12675 7825 0    60   ~ 0
SPI1_CS1
Text Label 13700 7225 0    60   ~ 0
SPI1_SCK
Text Label 13700 7625 0    60   ~ 0
SPI1_MISO
Text Label 13700 7325 0    60   ~ 0
SPI1_MOSI
Text Label 13700 7525 0    60   ~ 0
SPI1_CS3
Text Label 3750 3825 0    60   ~ 0
SPI1_CS3
Text Label 15475 9225 0    60   ~ 0
SPI1_SCK
Text Label 15475 9425 0    60   ~ 0
SPI1_MISO
Text Label 15475 9325 0    60   ~ 0
SPI1_MOSI
Text Label 14675 9425 2    60   ~ 0
SPI1_CS2
Text Label 3750 7925 0    60   ~ 0
ADC4_IN1
Text Label 3750 7225 0    60   ~ 0
CTL_MEAS_V1
Text Label 3750 7325 0    60   ~ 0
CTL_MEAS_V2
Text Label 5750 4100 0    60   ~ 0
CTL_MEAS_V1
Text Label 7450 4100 0    60   ~ 0
CTL_MEAS_V2
Text Label 9475 4825 0    60   ~ 0
ADC4_IN1
$Comp
L STM32F303VE U1
U 1 1 5797F52C
P 2650 5825
F 0 "U1" H 2650 10575 60  0000 C CNN
F 1 "STM32F303VE" H 2650 1075 60  0000 C CNN
F 2 "Custom:LQFP100_14x14mm" H 2650 5825 60  0001 C CNN
F 3 "http://www.st.com/content/ccc/resource/technical/document/datasheet/2c/6f/d7/64/1f/a3/4f/c9/DM00118585.pdf/files/DM00118585.pdf/jcr:content/translations/en.DM00118585.pdf" H 2650 5825 60  0001 C CNN
F 4 "Digikey" H 2650 5825 60  0001 C CNN "Supply"
F 5 "497-15164-ND" H 2650 5825 60  0001 C CNN "Part No."
F 6 "http://www.digikey.de/product-search/de/integrated-circuits-ics/embedded-microcontrollers/2556109?k=stm32f303ve" H 2650 5825 60  0001 C CNN "URL"
F 7 "\"7,39\"" H 2650 5825 60  0001 C CNN "Price"
	1    2650 5825
	1    0    0    -1  
$EndComp
Text Label 15475 4750 0    60   ~ 0
SPI2_MISO
Text Label 15475 4850 0    60   ~ 0
SPI2_MOSI
Text Label 15475 4650 0    60   ~ 0
SPI2_SCK
Text Label 15475 5050 0    60   ~ 0
SPI2_CS2
Text Label 14975 4950 2    60   ~ 0
SPI2_CS3
Text Label 15475 4950 0    60   ~ 0
SPI2_CS1
Text Label 14975 5050 2    60   ~ 0
SPI2_CS4
Text Label 15475 5650 0    60   ~ 0
SPI4_MISO
Text Label 15475 5750 0    60   ~ 0
SPI4_MOSI
Text Label 15475 5550 0    60   ~ 0
SPI4_SCK
Text Label 15475 5950 0    60   ~ 0
SPI4_CS2
Text Label 14975 5850 2    60   ~ 0
SPI4_CS3
Text Label 15475 5850 0    60   ~ 0
SPI4_CS1
Text Label 14975 5950 2    60   ~ 0
SPI4_CS4
Text Label 3750 6525 0    60   ~ 0
SPI4_CS1
Text Label 3750 6625 0    60   ~ 0
SPI4_CS2
Text Label 3750 6825 0    60   ~ 0
SPI4_CS3
Text Label 3750 6925 0    60   ~ 0
SPI4_CS4
Text Label 3750 10125 0    60   ~ 0
CAR_ID_1
Text Label 3750 10225 0    60   ~ 0
CAR_ID_2
Text Label 3750 10325 0    60   ~ 0
CAR_ID_3
Text Label 3750 10425 0    60   ~ 0
CAR_ID_4
Text Label 13900 5525 0    60   ~ 0
I2C2_SCL
Text Label 13900 5625 0    60   ~ 0
I2C2_SDA
Text Label 13900 6100 0    60   ~ 0
I2C3_SCL
Text Label 13900 6200 0    60   ~ 0
I2C3_SDA
$Comp
L +3V3 #PWR104
U 1 1 57A79028
P 13400 5725
F 0 "#PWR104" H 13400 5575 50  0001 C CNN
F 1 "+3V3" H 13400 5865 50  0000 C CNN
F 2 "" H 13400 5725 50  0000 C CNN
F 3 "" H 13400 5725 50  0000 C CNN
	1    13400 5725
	0    -1   -1   0   
$EndComp
$Comp
L +3V3 #PWR106
U 1 1 57A79436
P 13400 6300
F 0 "#PWR106" H 13400 6150 50  0001 C CNN
F 1 "+3V3" H 13400 6440 50  0000 C CNN
F 2 "" H 13400 6300 50  0000 C CNN
F 3 "" H 13400 6300 50  0000 C CNN
	1    13400 6300
	0    -1   -1   0   
$EndComp
Text Label 3750 8025 0    60   ~ 0
ADC4_IN2
Text Label 9525 7825 0    60   ~ 0
ADC4_IN2
$Comp
L +5V #PWR56
U 1 1 579ECE3F
P 8000 7525
F 0 "#PWR56" H 8000 7375 50  0001 C CNN
F 1 "+5V" H 8000 7665 50  0000 C CNN
F 2 "" H 8000 7525 50  0000 C CNN
F 3 "" H 8000 7525 50  0000 C CNN
	1    8000 7525
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR114
U 1 1 579F3903
P 13750 8075
F 0 "#PWR114" H 13750 7925 50  0001 C CNN
F 1 "+5V" H 13750 8215 50  0000 C CNN
F 2 "" H 13750 8075 50  0000 C CNN
F 3 "" H 13750 8075 50  0000 C CNN
	1    13750 8075
	-1   0    0    -1  
$EndComp
Text Label 3750 5725 0    60   ~ 0
CTL_BATT2_OUT
Text Label 3750 7525 0    60   ~ 0
EEPROM_WP
Text Label 3750 7825 0    60   ~ 0
EEPROM_HOLD
Text Label 14675 9225 2    60   ~ 0
EEPROM_WP
Text Label 14675 9325 2    60   ~ 0
EEPROM_HOLD
Text Notes 14000 8750 0    60   ~ 0
EEPROM
Text Notes 7225 8650 0    60   ~ 0
Externe Op-Amps
Text Notes 12675 8750 0    60   ~ 0
Taster
$Comp
L +3V3 #PWR137
U 1 1 57A3AC0D
P 15200 8300
F 0 "#PWR137" H 15200 8150 50  0001 C CNN
F 1 "+3V3" H 15200 8440 50  0000 C CNN
F 2 "" H 15200 8300 50  0000 C CNN
F 3 "" H 15200 8300 50  0000 C CNN
	1    15200 8300
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR139
U 1 1 57A3ACC9
P 15225 6800
F 0 "#PWR139" H 15225 6650 50  0001 C CNN
F 1 "+5V" H 15225 6940 50  0000 C CNN
F 2 "" H 15225 6800 50  0000 C CNN
F 3 "" H 15225 6800 50  0000 C CNN
	1    15225 6800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR138
U 1 1 57A3AD85
P 15200 8400
F 0 "#PWR138" H 15200 8150 50  0001 C CNN
F 1 "GND" H 15200 8250 50  0000 C CNN
F 2 "" H 15200 8400 50  0000 C CNN
F 3 "" H 15200 8400 50  0000 C CNN
	1    15200 8400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR140
U 1 1 57A3AE93
P 15225 6900
F 0 "#PWR140" H 15225 6650 50  0001 C CNN
F 1 "GND" H 15225 6750 50  0000 C CNN
F 2 "" H 15225 6900 50  0000 C CNN
F 3 "" H 15225 6900 50  0000 C CNN
	1    15225 6900
	1    0    0    -1  
$EndComp
Text Notes 14500 6575 0    60   ~ 0
Zusätzliche\nSpannungsversorgung
Text Label 14975 5650 2    60   ~ 0
SPI4_MISO
Text Label 14975 5750 2    60   ~ 0
SPI4_MOSI
Text Label 14975 5550 2    60   ~ 0
SPI4_SCK
$Comp
L GND #PWR134
U 1 1 57A3C417
P 14975 6150
F 0 "#PWR134" H 14975 5900 50  0001 C CNN
F 1 "GND" H 14975 6000 50  0000 C CNN
F 2 "" H 14975 6150 50  0000 C CNN
F 3 "" H 14975 6150 50  0000 C CNN
	1    14975 6150
	1    0    0    -1  
$EndComp
Text Label 14975 4750 2    60   ~ 0
SPI2_MISO
Text Label 14975 4850 2    60   ~ 0
SPI2_MOSI
Text Label 14975 4650 2    60   ~ 0
SPI2_SCK
$Comp
L GND #PWR132
U 1 1 57A3CC35
P 14975 5250
F 0 "#PWR132" H 14975 5000 50  0001 C CNN
F 1 "GND" H 14975 5100 50  0000 C CNN
F 2 "" H 14975 5250 50  0000 C CNN
F 3 "" H 14975 5250 50  0000 C CNN
	1    14975 5250
	-1   0    0    -1  
$EndComp
$Comp
L CONN_02X04 P20
U 1 1 57A3D04B
P 13650 5675
F 0 "P20" H 13650 5925 50  0000 C CNN
F 1 "I2C2" H 13650 5425 50  0000 C CNN
F 2 "Custom:IDC_Header_Straight_8pins" H 13650 4475 50  0001 C CNN
F 3 "" H 13650 4475 50  0000 C CNN
F 4 "2x4 Wannenstecker" H 13650 5675 60  0001 C CNN "Package"
	1    13650 5675
	1    0    0    -1  
$EndComp
Text Label 13400 5525 2    60   ~ 0
I2C2_SCL
Text Label 13400 5625 2    60   ~ 0
I2C2_SDA
$Comp
L GND #PWR105
U 1 1 57A3DA20
P 13400 5825
F 0 "#PWR105" H 13400 5575 50  0001 C CNN
F 1 "GND" H 13400 5675 50  0000 C CNN
F 2 "" H 13400 5825 50  0000 C CNN
F 3 "" H 13400 5825 50  0000 C CNN
	1    13400 5825
	-1   0    0    -1  
$EndComp
$Comp
L CONN_02X04 P21
U 1 1 57A3DF80
P 13650 6250
F 0 "P21" H 13650 6500 50  0000 C CNN
F 1 "I2C3" H 13650 6000 50  0000 C CNN
F 2 "Custom:IDC_Header_Straight_8pins" H 13650 5050 50  0001 C CNN
F 3 "" H 13650 5050 50  0000 C CNN
F 4 "2x4 Wannenstecker" H 13650 6250 60  0001 C CNN "Package"
	1    13650 6250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR107
U 1 1 57A3E263
P 13400 6400
F 0 "#PWR107" H 13400 6150 50  0001 C CNN
F 1 "GND" H 13400 6250 50  0000 C CNN
F 2 "" H 13400 6400 50  0000 C CNN
F 3 "" H 13400 6400 50  0000 C CNN
	1    13400 6400
	1    0    0    -1  
$EndComp
Text Label 13400 6100 2    60   ~ 0
I2C3_SCL
Text Label 13400 6200 2    60   ~ 0
I2C3_SDA
$Comp
L CONN_02X03 P26
U 1 1 57A4052A
P 15200 3950
F 0 "P26" H 15200 4150 50  0000 C CNN
F 1 "UART4" H 15200 3750 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x03" H 15200 2750 50  0001 C CNN
F 3 "" H 15200 2750 50  0000 C CNN
F 4 "2x3 Header" H 15200 3950 60  0001 C CNN "Package"
	1    15200 3950
	1    0    0    -1  
$EndComp
Text Label 14950 3850 2    60   ~ 0
UART4_RX
Text Label 14950 3950 2    60   ~ 0
UART4_TX
$Comp
L GND #PWR130
U 1 1 57A40C9F
P 14950 4050
F 0 "#PWR130" H 14950 3800 50  0001 C CNN
F 1 "GND" H 14950 3900 50  0000 C CNN
F 2 "" H 14950 4050 50  0000 C CNN
F 3 "" H 14950 4050 50  0000 C CNN
	1    14950 4050
	0    1    -1   0   
$EndComp
Text Notes 14500 3650 0    60   ~ 0
UART
$Comp
L +5V #PWR141
U 1 1 57A9B61A
P 15225 7300
F 0 "#PWR141" H 15225 7150 50  0001 C CNN
F 1 "+5V" H 15225 7440 50  0000 C CNN
F 2 "" H 15225 7300 50  0000 C CNN
F 3 "" H 15225 7300 50  0000 C CNN
	1    15225 7300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR142
U 1 1 57A9B620
P 15225 7400
F 0 "#PWR142" H 15225 7150 50  0001 C CNN
F 1 "GND" H 15225 7250 50  0000 C CNN
F 2 "" H 15225 7400 50  0000 C CNN
F 3 "" H 15225 7400 50  0000 C CNN
	1    15225 7400
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR125
U 1 1 57A9B677
P 14725 7300
F 0 "#PWR125" H 14725 7150 50  0001 C CNN
F 1 "+5V" H 14725 7440 50  0000 C CNN
F 2 "" H 14725 7300 50  0000 C CNN
F 3 "" H 14725 7300 50  0000 C CNN
	1    14725 7300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR126
U 1 1 57A9B67D
P 14725 7400
F 0 "#PWR126" H 14725 7150 50  0001 C CNN
F 1 "GND" H 14725 7250 50  0000 C CNN
F 2 "" H 14725 7400 50  0000 C CNN
F 3 "" H 14725 7400 50  0000 C CNN
	1    14725 7400
	1    0    0    -1  
$EndComp
$Comp
L CONN_02X02 P23
U 1 1 57A9B6D6
P 14975 6850
F 0 "P23" H 14975 7000 50  0000 C CNN
F 1 "5V" H 14975 6700 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x02" H 14975 5650 50  0001 C CNN
F 3 "" H 14975 5650 50  0000 C CNN
F 4 "2x2 Header" H 14975 6850 60  0001 C CNN "Package"
	1    14975 6850
	1    0    0    -1  
$EndComp
$Comp
L CONN_02X02 P24
U 1 1 57A9B859
P 14975 7350
F 0 "P24" H 14975 7500 50  0000 C CNN
F 1 "5V" H 14975 7200 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x02" H 14975 6150 50  0001 C CNN
F 3 "" H 14975 6150 50  0000 C CNN
F 4 "2x2 Header" H 14975 7350 60  0001 C CNN "Package"
	1    14975 7350
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR123
U 1 1 57A9BA10
P 14725 6800
F 0 "#PWR123" H 14725 6650 50  0001 C CNN
F 1 "+5V" H 14725 6940 50  0000 C CNN
F 2 "" H 14725 6800 50  0000 C CNN
F 3 "" H 14725 6800 50  0000 C CNN
	1    14725 6800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR124
U 1 1 57A9BA16
P 14725 6900
F 0 "#PWR124" H 14725 6650 50  0001 C CNN
F 1 "GND" H 14725 6750 50  0000 C CNN
F 2 "" H 14725 6900 50  0000 C CNN
F 3 "" H 14725 6900 50  0000 C CNN
	1    14725 6900
	1    0    0    -1  
$EndComp
$Comp
L CONN_02X02 P25
U 1 1 57A9BC72
P 14950 8350
F 0 "P25" H 14950 8500 50  0000 C CNN
F 1 "3.3V" H 14950 8200 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x02" H 14950 7150 50  0001 C CNN
F 3 "" H 14950 7150 50  0000 C CNN
F 4 "2x2 Header" H 14950 8350 60  0001 C CNN "Package"
	1    14950 8350
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR121
U 1 1 57A9BD46
P 14700 8300
F 0 "#PWR121" H 14700 8150 50  0001 C CNN
F 1 "+3V3" H 14700 8440 50  0000 C CNN
F 2 "" H 14700 8300 50  0000 C CNN
F 3 "" H 14700 8300 50  0000 C CNN
	1    14700 8300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR122
U 1 1 57A9BD4C
P 14700 8400
F 0 "#PWR122" H 14700 8150 50  0001 C CNN
F 1 "GND" H 14700 8250 50  0000 C CNN
F 2 "" H 14700 8400 50  0000 C CNN
F 3 "" H 14700 8400 50  0000 C CNN
	1    14700 8400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR108
U 1 1 57A9FAF8
P 13575 4625
F 0 "#PWR108" H 13575 4375 50  0001 C CNN
F 1 "GND" H 13575 4475 50  0000 C CNN
F 2 "" H 13575 4625 50  0000 C CNN
F 3 "" H 13575 4625 50  0000 C CNN
	1    13575 4625
	0    1    1    0   
$EndComp
$Comp
L CONN_01X05 P1
U 1 1 57AA2889
P 4525 2125
F 0 "P1" H 4525 2425 50  0000 C CNN
F 1 "Powerpath In" V 4625 2125 50  0000 C CNN
F 2 "Custom:PS25_5G" H 4525 2125 50  0001 C CNN
F 3 "" H 4525 2125 50  0000 C CNN
F 4 "Reichelt" H 4525 2125 60  0001 C CNN "Supply"
F 5 "PS25_5G" H 4525 2125 60  0001 C CNN "Part No."
F 6 "http://www.reichelt.de/index.html?ACTION=3;ARTICLE=40400;SEARCH=PS%2025/5G%20BR" H 4525 2125 60  0001 C CNN "URL"
F 7 "\"0,63\"" H 4525 2125 60  0001 C CNN "Price"
F 8 "1x5" H 4525 2125 60  0001 C CNN "Package"
	1    4525 2125
	-1   0    0    -1  
$EndComp
Text Label 4725 2025 0    60   ~ 0
PPC_+V
Text Label 4725 2125 0    60   ~ 0
PPC_+V_MEAS
Text Label 4725 1925 0    60   ~ 0
PPC_-V
Text Label 4725 2225 0    60   ~ 0
PPC_NetzOk
Text Label 4725 2325 0    60   ~ 0
PPC_Akkuwng
Text Label 5050 3025 0    60   ~ 0
PPC_+V_MEAS
Text Label 8525 3100 0    60   ~ 0
BATT2_+V_MEAS
Text Label 8525 3200 0    60   ~ 0
BATT2_-V_MEAS
$Comp
L PWR_FLAG #FLG7
U 1 1 57AD060E
P 6425 2500
F 0 "#FLG7" H 6425 2595 50  0001 C CNN
F 1 "PWR_FLAG" H 6425 2680 50  0000 C CNN
F 2 "" H 6425 2500 50  0000 C CNN
F 3 "" H 6425 2500 50  0000 C CNN
	1    6425 2500
	1    0    0    -1  
$EndComp
$Comp
L GNDPWR #PWR27
U 1 1 57AD0887
P 5925 2600
F 0 "#PWR27" H 5925 2400 50  0001 C CNN
F 1 "GNDPWR" H 5925 2470 50  0000 C CNN
F 2 "" H 5925 2550 50  0000 C CNN
F 3 "" H 5925 2550 50  0000 C CNN
	1    5925 2600
	1    0    0    -1  
$EndComp
$Comp
L R_Small R18
U 1 1 57AD0EAE
P 6250 2550
F 0 "R18" H 6280 2570 50  0000 L CNN
F 1 "0R" H 6280 2510 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 6250 2550 50  0001 C CNN
F 3 "" H 6250 2550 50  0000 C CNN
	1    6250 2550
	0    1    1    0   
$EndComp
$Comp
L GNDPWR #PWR18
U 1 1 57AD3B1F
P 5275 1575
F 0 "#PWR18" H 5275 1375 50  0001 C CNN
F 1 "GNDPWR" H 5275 1445 50  0000 C CNN
F 2 "" H 5275 1525 50  0000 C CNN
F 3 "" H 5275 1525 50  0000 C CNN
	1    5275 1575
	1    0    0    -1  
$EndComp
$Comp
L GNDPWR #PWR22
U 1 1 57AD3E91
P 5650 1575
F 0 "#PWR22" H 5650 1375 50  0001 C CNN
F 1 "GNDPWR" H 5650 1445 50  0000 C CNN
F 2 "" H 5650 1525 50  0000 C CNN
F 3 "" H 5650 1525 50  0000 C CNN
	1    5650 1575
	1    0    0    -1  
$EndComp
$Comp
L GNDPWR #PWR28
U 1 1 57AD3F9E
P 6025 1575
F 0 "#PWR28" H 6025 1375 50  0001 C CNN
F 1 "GNDPWR" H 6025 1445 50  0000 C CNN
F 2 "" H 6025 1525 50  0000 C CNN
F 3 "" H 6025 1525 50  0000 C CNN
	1    6025 1575
	1    0    0    -1  
$EndComp
$Comp
L GNDPWR #PWR59
U 1 1 57AD40AB
P 8075 1575
F 0 "#PWR59" H 8075 1375 50  0001 C CNN
F 1 "GNDPWR" H 8075 1445 50  0000 C CNN
F 2 "" H 8075 1525 50  0000 C CNN
F 3 "" H 8075 1525 50  0000 C CNN
	1    8075 1575
	1    0    0    -1  
$EndComp
$Comp
L GNDPWR #PWR62
U 1 1 57AD4264
P 8425 1575
F 0 "#PWR62" H 8425 1375 50  0001 C CNN
F 1 "GNDPWR" H 8425 1445 50  0000 C CNN
F 2 "" H 8425 1525 50  0000 C CNN
F 3 "" H 8425 1525 50  0000 C CNN
	1    8425 1575
	1    0    0    -1  
$EndComp
$Comp
L GNDPWR #PWR66
U 1 1 57AD4371
P 8800 1575
F 0 "#PWR66" H 8800 1375 50  0001 C CNN
F 1 "GNDPWR" H 8800 1445 50  0000 C CNN
F 2 "" H 8800 1525 50  0000 C CNN
F 3 "" H 8800 1525 50  0000 C CNN
	1    8800 1575
	1    0    0    -1  
$EndComp
$Comp
L GNDPWR #PWR40
U 1 1 57AD47B6
P 7025 1575
F 0 "#PWR40" H 7025 1375 50  0001 C CNN
F 1 "GNDPWR" H 7025 1445 50  0000 C CNN
F 2 "" H 7025 1525 50  0000 C CNN
F 3 "" H 7025 1525 50  0000 C CNN
	1    7025 1575
	1    0    0    -1  
$EndComp
Text Label 3750 3025 0    60   ~ 0
SPI1_CS2
Text Label 3750 2425 0    60   ~ 0
PPC_NetzOk
Text Label 3750 6025 0    60   ~ 0
PPC_Akkuwng
$Comp
L CONN_01X05 P11
U 1 1 57AEAD18
P 8600 6100
F 0 "P11" H 8600 6400 50  0000 C CNN
F 1 "Timer8" V 8700 6100 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x05" H 8600 6100 50  0001 C CNN
F 3 "" H 8600 6100 50  0000 C CNN
F 4 "1x5 Header" H 8600 6100 60  0001 C CNN "Package"
	1    8600 6100
	-1   0    0    -1  
$EndComp
$Comp
L GND #PWR68
U 1 1 57AEAE74
P 8800 6300
F 0 "#PWR68" H 8800 6050 50  0001 C CNN
F 1 "GND" H 8800 6150 50  0000 C CNN
F 2 "" H 8800 6300 50  0000 C CNN
F 3 "" H 8800 6300 50  0000 C CNN
	1    8800 6300
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X05 P12
U 1 1 57AEAFD6
P 8600 6800
F 0 "P12" H 8600 7100 50  0000 C CNN
F 1 "Timer20" V 8700 6800 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x05" H 8600 6800 50  0001 C CNN
F 3 "" H 8600 6800 50  0000 C CNN
F 4 "1x5 Header" H 8600 6800 60  0001 C CNN "Package"
	1    8600 6800
	-1   0    0    -1  
$EndComp
$Comp
L +5V #PWR67
U 1 1 57AEB59E
P 8800 6200
F 0 "#PWR67" H 8800 6050 50  0001 C CNN
F 1 "+5V" H 8800 6340 50  0000 C CNN
F 2 "" H 8800 6200 50  0000 C CNN
F 3 "" H 8800 6200 50  0000 C CNN
	1    8800 6200
	0    1    1    0   
$EndComp
$Comp
L +5V #PWR69
U 1 1 57AEB6B2
P 8800 6900
F 0 "#PWR69" H 8800 6750 50  0001 C CNN
F 1 "+5V" H 8800 7040 50  0000 C CNN
F 2 "" H 8800 6900 50  0000 C CNN
F 3 "" H 8800 6900 50  0000 C CNN
	1    8800 6900
	0    1    1    0   
$EndComp
$Comp
L +5V #PWR52
U 1 1 57AEBC7B
P 8000 6075
F 0 "#PWR52" H 8000 5925 50  0001 C CNN
F 1 "+5V" H 8000 6215 50  0000 C CNN
F 2 "" H 8000 6075 50  0000 C CNN
F 3 "" H 8000 6075 50  0000 C CNN
	1    8000 6075
	0    1    1    0   
$EndComp
$Comp
L +5V #PWR54
U 1 1 57AEBD88
P 8000 6650
F 0 "#PWR54" H 8000 6500 50  0001 C CNN
F 1 "+5V" H 8000 6790 50  0000 C CNN
F 2 "" H 8000 6650 50  0000 C CNN
F 3 "" H 8000 6650 50  0000 C CNN
	1    8000 6650
	0    1    1    0   
$EndComp
$Comp
L Crystal_Small Y1
U 1 1 57AECC39
P 4575 9975
F 0 "Y1" H 4575 10075 50  0000 C CNN
F 1 "8MHz" H 4575 9875 50  0000 C CNN
F 2 "Custom:HC-49S_Crystal" H 4575 9975 50  0001 C CNN
F 3 "http://www.farnell.com/datasheets/1497895.pdf" H 4575 9975 50  0001 C CNN
F 4 "Farnell" H 4575 9975 60  0001 C CNN "Supply"
F 5 "1842347" H 4575 9975 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/txc/9c-8-000meej-t/xtal-8-000mhz-18pf-smd-hc-49s/dp/1842347?ost=1842347&selectedCategoryId=&categoryNameResp=Alle%2BKategorien&searchView=table&iscrfnonsku=false" H 4575 9975 60  0001 C CNN "URL"
F 7 "\"0,581\"" H 4575 9975 60  0001 C CNN "Price"
	1    4575 9975
	0    -1   -1   0   
$EndComp
$Comp
L C_Small C11
U 1 1 57AECF10
P 4575 9700
F 0 "C11" H 4585 9770 50  0000 L CNN
F 1 "27p" H 4585 9620 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 4575 9700 50  0001 C CNN
F 3 "" H 4575 9700 50  0000 C CNN
F 4 "Farnell" H 4575 9700 60  0001 C CNN "Supply"
F 5 "7568584" H 4575 9700 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/avx/08052u270gat2a/hf-kondensator-c0g-np0-27pf-200v/dp/7568584" H 4575 9700 60  0001 C CNN "URL"
F 7 "\"0,227\"" H 4575 9700 60  0001 C CNN "Price"
F 8 "0805" H 4575 9700 60  0001 C CNN "Package"
	1    4575 9700
	-1   0    0    -1  
$EndComp
$Comp
L GND #PWR10
U 1 1 57AEF0C7
P 4575 9600
F 0 "#PWR10" H 4575 9350 50  0001 C CNN
F 1 "GND" H 4575 9450 50  0000 C CNN
F 2 "" H 4575 9600 50  0000 C CNN
F 3 "" H 4575 9600 50  0000 C CNN
	1    4575 9600
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR11
U 1 1 57AEF1E4
P 4575 10350
F 0 "#PWR11" H 4575 10100 50  0001 C CNN
F 1 "GND" H 4575 10200 50  0000 C CNN
F 2 "" H 4575 10350 50  0000 C CNN
F 3 "" H 4575 10350 50  0000 C CNN
	1    4575 10350
	0    -1   -1   0   
$EndComp
$Comp
L R_Small R2
U 1 1 57AEF775
P 4325 10100
F 0 "R2" H 4355 10120 50  0000 L CNN
F 1 "470R" H 4355 10060 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 4325 10100 50  0001 C CNN
F 3 "" H 4325 10100 50  0000 C CNN
	1    4325 10100
	0    1    1    0   
$EndComp
Text Label 8800 6100 0    60   ~ 0
PB1
Text Label 3750 3125 0    60   ~ 0
PB1
Text Label 3750 3225 0    60   ~ 0
PB2
Text Label 8800 6800 0    60   ~ 0
PB2
$Comp
L CONN_02X03 P4
U 1 1 57B119B9
P 5275 7800
F 0 "P4" H 5275 8000 50  0000 C CNN
F 1 "BATT2_IN" H 5275 7600 50  0000 C CNN
F 2 "Custom:WAGO_713-1423" H 5275 6600 50  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C151/WAGO713-14X.pdf" H 5275 6600 50  0001 C CNN
F 4 "Reichelt" H 5275 7800 60  0001 C CNN "Supply"
F 5 "WAGO 713-1423" H 5275 7800 60  0001 C CNN "Part No."
F 6 "http://www.reichelt.de/WAGO-713-1403/3/index.html?&ACTION=3&LA=446&ARTICLE=100883&artnr=WAGO+713-1403&SEARCH=713-1403" H 5275 7800 60  0001 C CNN "URL"
F 7 "\"1,20\"" H 5275 7800 60  0001 C CNN "Price"
F 8 "2x3" H 5275 7800 60  0001 C CNN "Package"
	1    5275 7800
	-1   0    0    -1  
$EndComp
Text Label 5525 7700 0    60   ~ 0
PPC_+V_PWR
Text Label 5525 7800 0    60   ~ 0
PPC_-V_PWR
$Comp
L Q_NMOS_GSD Q6
U 1 1 57B48381
P 7150 4100
F 0 "Q6" H 7450 4150 50  0000 R CNN
F 1 "IRLML6344" V 7400 4075 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 7350 4200 50  0001 C CNN
F 3 "http://www.farnell.com/datasheets/1911844.pdf" H 7150 4100 50  0001 C CNN
F 4 "Farnell" H 7150 4100 60  0001 C CNN "Supply"
F 5 "1857299" H 7150 4100 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/international-rectifier/irlml6344trpbf/mosfet-n-kanal-30v-5a-sot23/dp/1857299" H 7150 4100 60  0001 C CNN "URL"
F 7 "\"0,168\"" H 7150 4100 60  0001 C CNN "Price"
F 8 "SOT-23" H 7150 4100 60  0001 C CNN "Package"
	1    7150 4100
	-1   0    0    -1  
$EndComp
$Comp
L CONN_02X03 P6
U 1 1 57B4BAA9
P 6975 7650
F 0 "P6" H 6975 7850 50  0000 C CNN
F 1 "BATT2_REG_OUT" H 6975 7450 50  0000 C CNN
F 2 "Custom:WAGO_713-1423" H 6975 6450 50  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C151/WAGO713-14X.pdf" H 6975 6450 50  0001 C CNN
F 4 "Reichelt" H 6975 7650 60  0001 C CNN "Supply"
F 5 "WAGO 713-1423" H 6975 7650 60  0001 C CNN "Part No."
F 6 "https://www.reichelt.de/WAGO-713-1423/3/index.html?ACTION=3&LA=446&ARTICLE=100889&artnr=WAGO+713-1423&SEARCH=wago+713" H 6975 7650 60  0001 C CNN "URL"
F 7 "\"1,20\"" H 6975 7650 60  0001 C CNN "Price"
F 8 "2x3" H 6975 7650 60  0001 C CNN "Package"
	1    6975 7650
	-1   0    0    -1  
$EndComp
Text Notes 14325 2675 0    60   Italic 12
Verdeckt
Text Notes 15375 4200 0    60   Italic 12
Verdeckt
Text Notes 7650 8325 0    60   Italic 12
Verdeckt
$Comp
L CONN_02X07 P27
U 1 1 57ABEB87
P 15225 4950
F 0 "P27" H 15225 5350 50  0000 C CNN
F 1 "SPI2" V 15225 4950 50  0000 C CNN
F 2 "Connect:IDC_Header_Straight_14pins" H 15225 3750 50  0001 C CNN
F 3 "" H 15225 3750 50  0000 C CNN
F 4 "Reichelt" H 15225 4950 60  0001 C CNN "Supply"
F 5 "WSL 14G" H 15225 4950 60  0001 C CNN "Part No."
F 6 "http://www.reichelt.de/WSL-14G/3/index.html?&ACTION=3&LA=446&ARTICLE=22819&artnr=WSL+14G&SEARCH=wannenstecker" H 15225 4950 60  0001 C CNN "URL"
F 7 "\"0,12\"" H 15225 4950 60  0001 C CNN "Price"
F 8 "2x7" H 15225 4950 60  0001 C CNN "Package"
	1    15225 4950
	1    0    0    -1  
$EndComp
Text Label 13125 8950 2    60   ~ 0
BUTTON1
Text Label 13125 9275 2    60   ~ 0
BUTTON2
$Comp
L GND #PWR111
U 1 1 57AC1B88
P 13725 8950
F 0 "#PWR111" H 13725 8700 50  0001 C CNN
F 1 "GND" H 13725 8800 50  0000 C CNN
F 2 "" H 13725 8950 50  0000 C CNN
F 3 "" H 13725 8950 50  0000 C CNN
	1    13725 8950
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR112
U 1 1 57AC1CA4
P 13725 9275
F 0 "#PWR112" H 13725 9025 50  0001 C CNN
F 1 "GND" H 13725 9125 50  0000 C CNN
F 2 "" H 13725 9275 50  0000 C CNN
F 3 "" H 13725 9275 50  0000 C CNN
	1    13725 9275
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR101
U 1 1 57ACE69E
P 12675 4575
F 0 "#PWR101" H 12675 4325 50  0001 C CNN
F 1 "GND" H 12675 4425 50  0000 C CNN
F 2 "" H 12675 4575 50  0000 C CNN
F 3 "" H 12675 4575 50  0000 C CNN
	1    12675 4575
	0    -1   -1   0   
$EndComp
NoConn ~ 13575 4825
NoConn ~ 14075 4525
NoConn ~ 14075 4825
$Comp
L CONN_02X07 P28
U 1 1 57AD0392
P 15225 5850
F 0 "P28" H 15225 6250 50  0000 C CNN
F 1 "SPI4" V 15225 5850 50  0000 C CNN
F 2 "Connect:IDC_Header_Straight_14pins" H 15225 4650 50  0001 C CNN
F 3 "" H 15225 4650 50  0000 C CNN
F 4 "Reichelt" H 15225 5850 60  0001 C CNN "Supply"
F 5 "WSL 14G" H 15225 5850 60  0001 C CNN "Part No."
F 6 "http://www.reichelt.de/WSL-14G/3/index.html?&ACTION=3&LA=446&ARTICLE=22819&artnr=WSL+14G&SEARCH=wannenstecker" H 15225 5850 60  0001 C CNN "URL"
F 7 "\"0,12\"" H 15225 5850 60  0001 C CNN "Price"
F 8 "2x7" H 15225 5850 60  0001 C CNN "Package"
	1    15225 5850
	1    0    0    -1  
$EndComp
NoConn ~ 5525 7900
NoConn ~ 5025 7900
NoConn ~ 6725 7550
NoConn ~ 7225 7550
Text Label 5650 7375 2    60   ~ 0
PPC_-V_PWR
$Comp
L +12V #PWR44
U 1 1 57AD9EE6
P 7325 7875
F 0 "#PWR44" H 7325 7725 50  0001 C CNN
F 1 "+12V" H 7325 8015 50  0000 C CNN
F 2 "" H 7325 7875 50  0000 C CNN
F 3 "" H 7325 7875 50  0000 C CNN
	1    7325 7875
	-1   0    0    1   
$EndComp
Text Label 4775 6725 2    60   ~ 0
CTL_12V
Text Label 3750 7625 0    60   ~ 0
CTL_12V
Text Label 5650 5850 2    60   ~ 0
PPC_+V_PWR
$Comp
L +12V #PWR45
U 1 1 57AE8770
P 7350 6375
F 0 "#PWR45" H 7350 6225 50  0001 C CNN
F 1 "+12V" H 7350 6515 50  0000 C CNN
F 2 "" H 7350 6375 50  0000 C CNN
F 3 "" H 7350 6375 50  0000 C CNN
	1    7350 6375
	0    1    1    0   
$EndComp
$Comp
L PWR_FLAG #FLG5
U 1 1 57AE8892
P 6050 6000
F 0 "#FLG5" H 6050 6095 50  0001 C CNN
F 1 "PWR_FLAG" H 6050 6180 50  0000 C CNN
F 2 "" H 6050 6000 50  0000 C CNN
F 3 "" H 6050 6000 50  0000 C CNN
	1    6050 6000
	0    1    1    0   
$EndComp
$Comp
L LED D3
U 1 1 57AEB1F2
P 12175 9175
F 0 "D3" H 12175 9275 50  0000 C CNN
F 1 "LED" H 12175 9075 50  0000 C CNN
F 2 "Custom:LED_0603_HandSoldering" H 12175 9175 50  0001 C CNN
F 3 "" H 12175 9175 50  0000 C CNN
	1    12175 9175
	-1   0    0    -1  
$EndComp
$Comp
L R_Small R44
U 1 1 57AEB1F9
P 11850 9175
F 0 "R44" H 11880 9195 50  0000 L CNN
F 1 "680R" H 11880 9135 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 11850 9175 50  0001 C CNN
F 3 "" H 11850 9175 50  0000 C CNN
	1    11850 9175
	0    -1   -1   0   
$EndComp
$Comp
L LED D2
U 1 1 57AEB434
P 12175 8900
F 0 "D2" H 12175 9000 50  0000 C CNN
F 1 "LED" H 12175 8800 50  0000 C CNN
F 2 "Custom:LED_0603_HandSoldering" H 12175 8900 50  0001 C CNN
F 3 "" H 12175 8900 50  0000 C CNN
	1    12175 8900
	-1   0    0    -1  
$EndComp
$Comp
L R_Small R43
U 1 1 57AEB43B
P 11850 8900
F 0 "R43" H 11880 8920 50  0000 L CNN
F 1 "680R" H 11880 8860 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 11850 8900 50  0001 C CNN
F 3 "" H 11850 8900 50  0000 C CNN
	1    11850 8900
	0    -1   -1   0   
$EndComp
$Comp
L LED D4
U 1 1 57AEB523
P 12175 9450
F 0 "D4" H 12175 9550 50  0000 C CNN
F 1 "LED" H 12175 9350 50  0000 C CNN
F 2 "Custom:LED_0603_HandSoldering" H 12175 9450 50  0001 C CNN
F 3 "" H 12175 9450 50  0000 C CNN
	1    12175 9450
	-1   0    0    -1  
$EndComp
$Comp
L R_Small R45
U 1 1 57AEB52A
P 11850 9450
F 0 "R45" H 11880 9470 50  0000 L CNN
F 1 "680R" H 11880 9410 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 11850 9450 50  0001 C CNN
F 3 "" H 11850 9450 50  0000 C CNN
	1    11850 9450
	0    -1   -1   0   
$EndComp
Text Notes 11450 8675 0    60   ~ 0
Status LEDs
$Comp
L MCP6054 U3
U 2 1 57AF0807
P 8825 4825
F 0 "U3" H 8875 5025 50  0000 C CNN
F 1 "MCP6054" H 8975 4625 50  0000 C CNN
F 2 "Housings_SOIC:SOIC-14_3.9x8.7mm_Pitch1.27mm" H 8775 4925 50  0001 C CNN
F 3 "http://www.farnell.com/datasheets/1062686.pdf" H 8875 5025 50  0001 C CNN
F 4 "MCP6054-E/SL" H 8825 4825 60  0001 C CNN "Spec"
F 5 "Farnell" H 8825 4825 60  0001 C CNN "Supply"
F 6 "1715855 " H 8825 4825 60  0001 C CNN "Part No."
F 7 "http://de.farnell.com/microchip/mcp6054-e-sl/op-amp-quad-1-8v-300khz-14soic/dp/1715855" H 8825 4825 60  0001 C CNN "URL"
F 8 "\"1,74\"" H 8825 4825 60  0001 C CNN "Price"
F 9 "SOIC 14" H 8825 4825 60  0001 C CNN "Package"
	2    8825 4825
	1    0    0    -1  
$EndComp
Text Label 3750 9225 0    60   ~ 0
PD10
Text Label 3750 9325 0    60   ~ 0
PD11
Text Label 3750 9425 0    60   ~ 0
PD12
Text Label 3750 9525 0    60   ~ 0
PD13
Text Label 3750 9625 0    60   ~ 0
PD14
$Comp
L CONN_02X06 P16
U 1 1 57AFE8DA
P 12350 5725
F 0 "P16" H 12350 6075 50  0000 C CNN
F 1 "DIO" H 12350 5375 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x06" H 12350 4525 50  0001 C CNN
F 3 "" H 12350 4525 50  0000 C CNN
F 4 "2x6 Header" H 12350 5725 60  0001 C CNN "Package"
	1    12350 5725
	1    0    0    -1  
$EndComp
Text Label 12100 5475 2    60   ~ 0
PD0
Text Label 12100 5575 2    60   ~ 0
PD1
Text Label 12100 5675 2    60   ~ 0
PD2
Text Label 12100 5775 2    60   ~ 0
PD3
Text Label 12600 5475 0    60   ~ 0
PD4
Text Label 12600 5575 0    60   ~ 0
PD5
Text Label 12600 5675 0    60   ~ 0
PD6
Text Label 12600 5775 0    60   ~ 0
PD7
$Comp
L +5V #PWR99
U 1 1 57AFF054
P 12600 5875
F 0 "#PWR99" H 12600 5725 50  0001 C CNN
F 1 "+5V" H 12600 6015 50  0000 C CNN
F 2 "" H 12600 5875 50  0000 C CNN
F 3 "" H 12600 5875 50  0000 C CNN
	1    12600 5875
	0    1    1    0   
$EndComp
$Comp
L GND #PWR100
U 1 1 57AFF276
P 12600 5975
F 0 "#PWR100" H 12600 5725 50  0001 C CNN
F 1 "GND" H 12600 5825 50  0000 C CNN
F 2 "" H 12600 5975 50  0000 C CNN
F 3 "" H 12600 5975 50  0000 C CNN
	1    12600 5975
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR96
U 1 1 57AFF3BD
P 12100 5975
F 0 "#PWR96" H 12100 5725 50  0001 C CNN
F 1 "GND" H 12100 5825 50  0000 C CNN
F 2 "" H 12100 5975 50  0000 C CNN
F 3 "" H 12100 5975 50  0000 C CNN
	1    12100 5975
	1    0    0    -1  
$EndComp
$Comp
L CONN_02X05 P17
U 1 1 57AFF85F
P 5950 10325
F 0 "P17" H 5950 10625 50  0000 C CNN
F 1 "ADC" H 5950 10025 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x05" H 5950 9125 50  0001 C CNN
F 3 "" H 5950 9125 50  0000 C CNN
F 4 "2x5 Header" H 5950 10325 60  0001 C CNN "Package"
	1    5950 10325
	1    0    0    -1  
$EndComp
Text Label 6850 10000 0    60   ~ 0
PD10
Text Label 6850 10225 0    60   ~ 0
PD11
Text Label 6825 10600 0    60   ~ 0
PD12
Text Label 5050 10000 2    60   ~ 0
PD13
Text Label 3750 4825 0    60   ~ 0
ADC12_IN6
Text Label 3750 4925 0    60   ~ 0
ADC12_IN7
Text Label 8800 9825 0    60   ~ 0
ADC12_IN6
Text Label 10800 9825 0    60   ~ 0
ADC12_IN7
Text Label 3750 5225 0    60   ~ 0
LED1
Text Label 3750 5325 0    60   ~ 0
LED2
Text Label 3750 5625 0    60   ~ 0
LED3
Text Label 11700 8900 2    60   ~ 0
LED1
Text Label 11700 9175 2    60   ~ 0
LED2
Text Label 11700 9450 2    60   ~ 0
LED3
$Comp
L GND #PWR98
U 1 1 57AD321E
P 12475 9750
F 0 "#PWR98" H 12475 9500 50  0001 C CNN
F 1 "GND" H 12475 9600 50  0000 C CNN
F 2 "" H 12475 9750 50  0000 C CNN
F 3 "" H 12475 9750 50  0000 C CNN
	1    12475 9750
	1    0    0    -1  
$EndComp
$Comp
L CP_Small C14
U 1 1 57AE2144
P 5275 1275
F 0 "C14" H 5285 1345 50  0000 L CNN
F 1 "1m" H 5285 1195 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Radial_D10_L20_P5-7.5" H 5275 1275 50  0001 C CNN
F 3 "" H 5275 1275 50  0000 C CNN
F 4 "Farnell" H 5275 1275 60  0001 C CNN "Supply"
F 5 "2469429" H 5275 1275 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/rubycon/25zls1000mefc10x20/alu-elko-1000uf-25v-radial/dp/2469429" H 5275 1275 60  0001 C CNN "URL"
F 7 "\"0,661\"" H 5275 1275 60  0001 C CNN "Price"
F 8 "Elko 25V" H 5275 1275 60  0001 C CNN "Spec"
	1    5275 1275
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH SW2
U 1 1 57AE84E3
P 13425 8950
F 0 "SW2" H 13575 9060 50  0000 C CNN
F 1 "Button1" H 13425 8870 50  0000 C CNN
F 2 "Custom:BUTTON" H 13425 8950 50  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C200/JTP_1138_02.pdf" H 13425 8950 50  0001 C CNN
F 4 "Reichelt" H 13425 8950 60  0001 C CNN "Supply"
F 5 "TASTER 9314" H 13425 8950 60  0001 C CNN "Part No."
F 6 "http://www.reichelt.de/Kurzhubtaster/TASTER-9314/3/index.html?&ACTION=3&LA=2&ARTICLE=44510&GROUPID=7587&artnr=TASTER+9314" H 13425 8950 60  0001 C CNN "URL"
F 7 "\"0,22\"" H 13425 8950 60  0001 C CNN "Price"
	1    13425 8950
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH SW3
U 1 1 57AE9D1B
P 13425 9275
F 0 "SW3" H 13575 9385 50  0000 C CNN
F 1 "Button2" H 13425 9195 50  0000 C CNN
F 2 "Custom:BUTTON" H 13425 9275 50  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C200/JTP_1138_02.pdf" H 13425 9275 50  0001 C CNN
F 4 "Reichelt" H 13425 9275 60  0001 C CNN "Supply"
F 5 "TASTER 9314" H 13425 9275 60  0001 C CNN "Part No."
F 6 "http://www.reichelt.de/Kurzhubtaster/TASTER-9314/3/index.html?&ACTION=3&LA=2&ARTICLE=44510&GROUPID=7587&artnr=TASTER+9314" H 13425 9275 60  0001 C CNN "URL"
F 7 "\"0,22\"" H 13425 9275 60  0001 C CNN "Price"
	1    13425 9275
	1    0    0    -1  
$EndComp
$Comp
L MCP6054 U3
U 4 1 57AEF5EC
P 8925 7825
F 0 "U3" H 8975 8025 50  0000 C CNN
F 1 "MCP6054" H 9075 7625 50  0000 C CNN
F 2 "Housings_SOIC:SOIC-14_3.9x8.7mm_Pitch1.27mm" H 8875 7925 50  0001 C CNN
F 3 "http://www.farnell.com/datasheets/1062686.pdf" H 8975 8025 50  0001 C CNN
F 4 "MCP6054-E/SL" H 8925 7825 60  0001 C CNN "Spec"
F 5 "Farnell" H 8925 7825 60  0001 C CNN "Supply"
F 6 "1715855 " H 8925 7825 60  0001 C CNN "Part No."
F 7 "http://de.farnell.com/microchip/mcp6054-e-sl/op-amp-quad-1-8v-300khz-14soic/dp/1715855" H 8925 7825 60  0001 C CNN "URL"
F 8 "\"1,74\"" H 8925 7825 60  0001 C CNN "Price"
F 9 "SOIC14" H 8925 7825 60  0001 C CNN "Package"
	4    8925 7825
	1    0    0    -1  
$EndComp
$Comp
L MCP6054 U3
U 3 1 57AEF919
P 10075 9825
F 0 "U3" H 10125 10025 50  0000 C CNN
F 1 "MCP6054" H 10225 9625 50  0000 C CNN
F 2 "Housings_SOIC:SOIC-14_3.9x8.7mm_Pitch1.27mm" H 10025 9925 50  0001 C CNN
F 3 "http://www.farnell.com/datasheets/1062686.pdf" H 10125 10025 50  0001 C CNN
F 4 "MCP6054-E/SL" H 10075 9825 60  0001 C CNN "Spec"
F 5 "Farnell" H 10075 9825 60  0001 C CNN "Supply"
F 6 "1715855 " H 10075 9825 60  0001 C CNN "Part No."
F 7 "http://de.farnell.com/microchip/mcp6054-e-sl/op-amp-quad-1-8v-300khz-14soic/dp/1715855" H 10075 9825 60  0001 C CNN "URL"
F 8 "1.74" H 10075 9825 60  0001 C CNN "Price"
F 9 "SOIC 14" H 10075 9825 60  0001 C CNN "Package"
	3    10075 9825
	1    0    0    -1  
$EndComp
$Comp
L C_Small C12
U 1 1 57AF8658
P 4575 10250
F 0 "C12" H 4585 10320 50  0000 L CNN
F 1 "27p" H 4585 10170 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 4575 10250 50  0001 C CNN
F 3 "" H 4575 10250 50  0000 C CNN
F 4 "Farnell" H 4575 10250 60  0001 C CNN "Supply"
F 5 "7568584" H 4575 10250 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/avx/08052u270gat2a/hf-kondensator-c0g-np0-27pf-200v/dp/7568584" H 4575 10250 60  0001 C CNN "URL"
F 7 "\"0,227\"" H 4575 10250 60  0001 C CNN "Price"
F 8 "0805" H 4575 10250 60  0001 C CNN "Package"
	1    4575 10250
	-1   0    0    -1  
$EndComp
$Comp
L Hole PD2
U 1 1 57ADE2F6
P 950 10900
F 0 "PD2" H 950 10790 60  0000 C CNN
F 1 "Hole" H 950 11000 60  0000 C CNN
F 2 "Custom:MountingHole_3.3mm_M3" H 950 10900 60  0001 C CNN
F 3 "" H 950 10900 60  0000 C CNN
	1    950  10900
	1    0    0    -1  
$EndComp
$Comp
L Hole PD3
U 1 1 57ADE44A
P 1175 10900
F 0 "PD3" H 1175 10790 60  0000 C CNN
F 1 "Hole" H 1175 11000 60  0000 C CNN
F 2 "Custom:MountingHole_3.3mm_M3" H 1175 10900 60  0001 C CNN
F 3 "" H 1175 10900 60  0000 C CNN
	1    1175 10900
	1    0    0    -1  
$EndComp
$Comp
L Hole PD4
U 1 1 57ADE5A5
P 1400 10900
F 0 "PD4" H 1400 10790 60  0000 C CNN
F 1 "Hole" H 1400 11000 60  0000 C CNN
F 2 "Custom:MountingHole_3.3mm_M3" H 1400 10900 60  0001 C CNN
F 3 "" H 1400 10900 60  0000 C CNN
	1    1400 10900
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR102
U 1 1 57B41C6E
P 12775 3900
F 0 "#PWR102" H 12775 3750 50  0001 C CNN
F 1 "+3V3" H 12775 4040 50  0000 C CNN
F 2 "" H 12775 3900 50  0000 C CNN
F 3 "" H 12775 3900 50  0000 C CNN
	1    12775 3900
	1    0    0    -1  
$EndComp
Text Label 13125 9625 2    60   ~ 0
BUTTON3
$Comp
L GND #PWR113
U 1 1 57B4C158
P 13725 9625
F 0 "#PWR113" H 13725 9375 50  0001 C CNN
F 1 "GND" H 13725 9475 50  0000 C CNN
F 2 "" H 13725 9625 50  0000 C CNN
F 3 "" H 13725 9625 50  0000 C CNN
	1    13725 9625
	0    -1   -1   0   
$EndComp
$Comp
L SW_PUSH SW4
U 1 1 57B4C162
P 13425 9625
F 0 "SW4" H 13575 9735 50  0000 C CNN
F 1 "Button3" H 13425 9545 50  0000 C CNN
F 2 "Custom:BUTTON" H 13425 9625 50  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C200/JTP_1138_02.pdf" H 13425 9625 50  0001 C CNN
F 4 "Reichelt" H 13425 9625 60  0001 C CNN "Supply"
F 5 "TASTER 9314" H 13425 9625 60  0001 C CNN "Part No."
F 6 "http://www.reichelt.de/Kurzhubtaster/TASTER-9314/3/index.html?&ACTION=3&LA=2&ARTICLE=44510&GROUPID=7587&artnr=TASTER+9314" H 13425 9625 60  0001 C CNN "URL"
F 7 "\"0,22\"" H 13425 9625 60  0001 C CNN "Price"
	1    13425 9625
	1    0    0    -1  
$EndComp
Text Label 3750 6125 0    60   ~ 0
BUTTON1
Text Label 3750 6225 0    60   ~ 0
BUTTON2
Text Label 3750 6325 0    60   ~ 0
BUTTON3
Text Label 4950 1575 2    60   ~ 0
PPC_+V_PWR
$Comp
L R_Small R3
U 1 1 57B56772
P 4825 1250
F 0 "R3" H 4855 1270 50  0000 L CNN
F 1 "n.B." H 4855 1210 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 4825 1250 50  0001 C CNN
F 3 "" H 4825 1250 50  0000 C CNN
	1    4825 1250
	-1   0    0    1   
$EndComp
$Comp
L R_Small R8
U 1 1 57B57252
P 5025 1250
F 0 "R8" H 5055 1270 50  0000 L CNN
F 1 "0R" H 5055 1210 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 5025 1250 50  0001 C CNN
F 3 "" H 5025 1250 50  0000 C CNN
	1    5025 1250
	-1   0    0    1   
$EndComp
$Comp
L R_Small R29
U 1 1 57B5E6CC
P 7900 4725
F 0 "R29" H 7930 4745 50  0000 L CNN
F 1 "27k" H 7930 4685 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 7900 4725 50  0001 C CNN
F 3 "" H 7900 4725 50  0000 C CNN
	1    7900 4725
	0    -1   -1   0   
$EndComp
$Comp
L C_Small C21
U 1 1 57B5E9AE
P 8050 4900
F 0 "C21" H 8060 4970 50  0000 L CNN
F 1 "470n" H 8060 4820 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 8050 4900 50  0001 C CNN
F 3 "" H 8050 4900 50  0000 C CNN
	1    8050 4900
	1    0    0    -1  
$EndComp
$Comp
L R R23
U 1 1 5790E6B9
P 6675 4925
F 0 "R23" V 6755 4925 50  0000 C CNN
F 1 "11k" V 6675 4925 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 6605 4925 50  0001 C CNN
F 3 "" H 6675 4925 50  0000 C CNN
F 4 "\"0,1%\"" H 6675 4925 60  0001 C CNN "Spec"
	1    6675 4925
	1    0    0    -1  
$EndComp
Text Label 6050 5100 2    60   ~ 0
PPC_-V
Text Label 6050 5350 2    60   ~ 0
BATT2_-V_MEAS
$Comp
L R_Small R13
U 1 1 57B69877
P 5700 4275
F 0 "R13" H 5730 4295 50  0000 L CNN
F 1 "10k" H 5730 4235 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 5700 4275 50  0001 C CNN
F 3 "" H 5700 4275 50  0000 C CNN
	1    5700 4275
	1    0    0    -1  
$EndComp
$Comp
L R_Small R27
U 1 1 57B6B231
P 7400 4250
F 0 "R27" H 7430 4270 50  0000 L CNN
F 1 "10k" H 7430 4210 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 7400 4250 50  0001 C CNN
F 3 "" H 7400 4250 50  0000 C CNN
	1    7400 4250
	1    0    0    -1  
$EndComp
$Comp
L R_Small R9
U 1 1 57B6F94F
P 5050 8625
F 0 "R9" H 5080 8645 50  0000 L CNN
F 1 "100R" H 5080 8585 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 5050 8625 50  0001 C CNN
F 3 "" H 5050 8625 50  0000 C CNN
	1    5050 8625
	0    -1   -1   0   
$EndComp
$Comp
L R_Small R10
U 1 1 57B706AE
P 5050 9150
F 0 "R10" H 5080 9170 50  0000 L CNN
F 1 "100R" H 5080 9110 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 5050 9150 50  0001 C CNN
F 3 "" H 5050 9150 50  0000 C CNN
	1    5050 9150
	0    -1   -1   0   
$EndComp
$Comp
L R_Small R5
U 1 1 57B73220
P 5725 1925
F 0 "R5" H 5755 1945 50  0000 L CNN
F 1 "0R" H 5755 1885 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 5725 1925 50  0001 C CNN
F 3 "" H 5725 1925 50  0000 C CNN
	1    5725 1925
	0    -1   -1   0   
$EndComp
$Comp
L Hole PD5
U 1 1 57B799BF
P 1625 10900
F 0 "PD5" H 1625 10790 60  0000 C CNN
F 1 "Hole" H 1625 11000 60  0000 C CNN
F 2 "Custom:MountingHole_3.3mm_M3" H 1625 10900 60  0001 C CNN
F 3 "" H 1625 10900 60  0000 C CNN
	1    1625 10900
	1    0    0    -1  
$EndComp
$Comp
L Hole PD6
U 1 1 57B79B45
P 1850 10900
F 0 "PD6" H 1850 10790 60  0000 C CNN
F 1 "Hole" H 1850 11000 60  0000 C CNN
F 2 "Custom:MountingHole_3.3mm_M3" H 1850 10900 60  0001 C CNN
F 3 "" H 1850 10900 60  0000 C CNN
	1    1850 10900
	1    0    0    -1  
$EndComp
$Comp
L Q_PMOS_GSD Q5
U 1 1 57B44124
P 6775 3825
F 0 "Q5" H 7075 3875 50  0000 R CNN
F 1 "IRLML9301" V 7025 3800 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 6975 3925 50  0001 C CNN
F 3 "http://www.farnell.com/datasheets/1749827.pdf" H 6775 3825 50  0001 C CNN
F 4 "farnell" H 6775 3825 60  0001 C CNN "Supply"
F 5 "1831089" H 6775 3825 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/infineon/irlml9301trpbf/mosfet-diode-p-kanal-30v-3-6a/dp/1831089" H 6775 3825 60  0001 C CNN "URL"
F 7 "\"0,224\"" H 6775 3825 60  0001 C CNN "Price"
F 8 "SOT-23" H 6775 3825 60  0001 C CNN "Package"
	1    6775 3825
	-1   0    0    1   
$EndComp
NoConn ~ 7250 6625
$Comp
L LED D1
U 1 1 57B676C2
P 4975 7175
F 0 "D1" H 4975 7275 50  0000 C CNN
F 1 "LED" H 4975 7075 50  0000 C CNN
F 2 "Custom:LED_0603_HandSoldering" H 4975 7175 50  0001 C CNN
F 3 "" H 4975 7175 50  0000 C CNN
	1    4975 7175
	0    1    -1   0   
$EndComp
$Comp
L R_Small R7
U 1 1 57B676C9
P 4975 6875
F 0 "R7" H 5005 6895 50  0000 L CNN
F 1 "680R" H 5005 6835 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 4975 6875 50  0001 C CNN
F 3 "" H 4975 6875 50  0000 C CNN
	1    4975 6875
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR24
U 1 1 57B64CEE
P 5700 6050
F 0 "#PWR24" H 5700 5900 50  0001 C CNN
F 1 "+3V3" H 5700 6190 50  0000 C CNN
F 2 "" H 5700 6050 50  0000 C CNN
F 3 "" H 5700 6050 50  0000 C CNN
	1    5700 6050
	1    0    0    -1  
$EndComp
$Comp
L R_Small R25
U 1 1 57B62B74
P 6900 3575
F 0 "R25" H 6930 3595 50  0000 L CNN
F 1 "10k" H 6930 3535 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 6900 3575 50  0001 C CNN
F 3 "" H 6900 3575 50  0000 C CNN
	1    6900 3575
	0    1    1    0   
$EndComp
$Comp
L C_Small C19
U 1 1 57B62D4B
P 6900 3450
F 0 "C19" H 6910 3520 50  0000 L CNN
F 1 "100n" H 6910 3370 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 6900 3450 50  0001 C CNN
F 3 "" H 6900 3450 50  0000 C CNN
	1    6900 3450
	0    -1   -1   0   
$EndComp
$Comp
L Q_PMOS_GSD Q1
U 1 1 57B67A6F
P 5050 3825
F 0 "Q1" H 5350 3875 50  0000 R CNN
F 1 "IRLML9301" V 5300 3800 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 5250 3925 50  0001 C CNN
F 3 "http://www.farnell.com/datasheets/1749827.pdf" H 5050 3825 50  0001 C CNN
F 4 "farnell" H 5050 3825 60  0001 C CNN "Supply"
F 5 "1831089" H 5050 3825 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/infineon/irlml9301trpbf/mosfet-diode-p-kanal-30v-3-6a/dp/1831089" H 5050 3825 60  0001 C CNN "URL"
F 7 "\"0,224\"" H 5050 3825 60  0001 C CNN "Price"
F 8 "SOT-23" H 5050 3825 60  0001 C CNN "Package"
	1    5050 3825
	-1   0    0    1   
$EndComp
$Comp
L R_Small R11
U 1 1 57B6A599
P 5200 3575
F 0 "R11" H 5230 3595 50  0000 L CNN
F 1 "10k" H 5230 3535 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 5200 3575 50  0001 C CNN
F 3 "" H 5200 3575 50  0000 C CNN
	1    5200 3575
	0    1    1    0   
$EndComp
$Comp
L C_Small C13
U 1 1 57B6A5A0
P 5200 3450
F 0 "C13" H 5210 3520 50  0000 L CNN
F 1 "100n" H 5210 3370 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 5200 3450 50  0001 C CNN
F 3 "" H 5200 3450 50  0000 C CNN
	1    5200 3450
	0    -1   -1   0   
$EndComp
$Comp
L R R22
U 1 1 5790E6B3
P 6675 4350
F 0 "R22" V 6755 4350 50  0000 C CNN
F 1 "56k" V 6675 4350 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 6605 4350 50  0001 C CNN
F 3 "" H 6675 4350 50  0000 C CNN
F 4 "\"0,1%\"" H 6675 4350 60  0001 C CNN "Spec"
F 5 "VEN:" H 6675 4350 60  0001 C CNN "Supply"
	1    6675 4350
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X04 P9
U 1 1 57B8C05A
P 7750 7675
F 0 "P9" H 7750 7925 50  0000 C CNN
F 1 "Hal5" V 7850 7675 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x04" H 7750 7675 50  0001 C CNN
F 3 "" H 7750 7675 50  0000 C CNN
F 4 "1x4 Header" H 7750 7675 60  0001 C CNN "Package"
	1    7750 7675
	-1   0    0    -1  
$EndComp
$Comp
L MCP6054 U3
U 1 1 57B99A40
P 8075 9825
F 0 "U3" H 8125 10025 50  0000 C CNN
F 1 "MCP6054" H 8225 9625 50  0000 C CNN
F 2 "Housings_SOIC:SOIC-14_3.9x8.7mm_Pitch1.27mm" H 8025 9925 50  0001 C CNN
F 3 "http://www.farnell.com/datasheets/1062686.pdf" H 8125 10025 50  0001 C CNN
F 4 "MCP6054-E/SL" H 8075 9825 60  0001 C CNN "Spec"
F 5 "Farnell" H 8075 9825 60  0001 C CNN "Supply"
F 6 "1715855 " H 8075 9825 60  0001 C CNN "Part No."
F 7 "http://de.farnell.com/microchip/mcp6054-e-sl/op-amp-quad-1-8v-300khz-14soic/dp/1715855" H 8075 9825 60  0001 C CNN "URL"
F 8 "1.74" H 8075 9825 60  0001 C CNN "Price"
F 9 "SOIC 14" H 8075 9825 60  0001 C CNN "Package"
	1    8075 9825
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR65
U 1 1 57BB17AD
P 8725 5125
F 0 "#PWR65" H 8725 4875 50  0001 C CNN
F 1 "GNDA" H 8725 4975 50  0000 C CNN
F 2 "" H 8725 5125 50  0000 C CNN
F 3 "" H 8725 5125 50  0000 C CNN
	1    8725 5125
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR20
U 1 1 57BB1F50
P 5350 4400
F 0 "#PWR20" H 5350 4150 50  0001 C CNN
F 1 "GNDA" H 5350 4250 50  0000 C CNN
F 2 "" H 5350 4400 50  0000 C CNN
F 3 "" H 5350 4400 50  0000 C CNN
	1    5350 4400
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR23
U 1 1 57BB20DE
P 5700 4400
F 0 "#PWR23" H 5700 4150 50  0001 C CNN
F 1 "GNDA" H 5700 4250 50  0000 C CNN
F 2 "" H 5700 4400 50  0000 C CNN
F 3 "" H 5700 4400 50  0000 C CNN
	1    5700 4400
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR42
U 1 1 57BB2D05
P 7050 4375
F 0 "#PWR42" H 7050 4125 50  0001 C CNN
F 1 "GNDA" H 7050 4225 50  0000 C CNN
F 2 "" H 7050 4375 50  0000 C CNN
F 3 "" H 7050 4375 50  0000 C CNN
	1    7050 4375
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR47
U 1 1 57BB2E93
P 7400 4375
F 0 "#PWR47" H 7400 4125 50  0001 C CNN
F 1 "GNDA" H 7400 4225 50  0000 C CNN
F 2 "" H 7400 4375 50  0000 C CNN
F 3 "" H 7400 4375 50  0000 C CNN
	1    7400 4375
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR37
U 1 1 57BB3021
P 6675 5425
F 0 "#PWR37" H 6675 5175 50  0001 C CNN
F 1 "GNDA" H 6675 5275 50  0000 C CNN
F 2 "" H 6675 5425 50  0000 C CNN
F 3 "" H 6675 5425 50  0000 C CNN
	1    6675 5425
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR58
U 1 1 57BB3330
P 8050 5125
F 0 "#PWR58" H 8050 4875 50  0001 C CNN
F 1 "GNDA" H 8050 4975 50  0000 C CNN
F 2 "" H 8050 5125 50  0000 C CNN
F 3 "" H 8050 5125 50  0000 C CNN
	1    8050 5125
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR2
U 1 1 57BB6E40
P 575 3125
F 0 "#PWR2" H 575 2875 50  0001 C CNN
F 1 "GNDA" H 575 2975 50  0000 C CNN
F 2 "" H 575 3125 50  0000 C CNN
F 3 "" H 575 3125 50  0000 C CNN
	1    575  3125
	1    0    0    -1  
$EndComp
$Comp
L R_Small R1
U 1 1 57BBBEDF
P 975 4800
F 0 "R1" H 1005 4820 50  0000 L CNN
F 1 "0R" H 1005 4760 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 975 4800 50  0001 C CNN
F 3 "" H 975 4800 50  0000 C CNN
	1    975  4800
	0    1    1    0   
$EndComp
$Comp
L GND #PWR3
U 1 1 57BBC0BD
P 750 4875
F 0 "#PWR3" H 750 4625 50  0001 C CNN
F 1 "GND" H 750 4725 50  0000 C CNN
F 2 "" H 750 4875 50  0000 C CNN
F 3 "" H 750 4875 50  0000 C CNN
	1    750  4875
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR5
U 1 1 57BBCA09
P 1250 4875
F 0 "#PWR5" H 1250 4625 50  0001 C CNN
F 1 "GNDA" H 1250 4725 50  0000 C CNN
F 2 "" H 1250 4875 50  0000 C CNN
F 3 "" H 1250 4875 50  0000 C CNN
	1    1250 4875
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG1
U 1 1 57BBCBA1
P 1125 4725
F 0 "#FLG1" H 1125 4820 50  0001 C CNN
F 1 "PWR_FLAG" H 1125 4905 50  0000 C CNN
F 2 "" H 1125 4725 50  0000 C CNN
F 3 "" H 1125 4725 50  0000 C CNN
	1    1125 4725
	1    0    0    -1  
$EndComp
$Comp
L R_Small R50
U 1 1 57B6786D
P 14200 4825
F 0 "R50" H 14230 4845 50  0000 L CNN
F 1 "0R" H 14230 4785 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 14200 4825 50  0001 C CNN
F 3 "" H 14200 4825 50  0000 C CNN
	1    14200 4825
	1    0    0    -1  
$EndComp
$Comp
L R_Small R46
U 1 1 57B68531
P 13325 4825
F 0 "R46" H 13355 4845 50  0000 L CNN
F 1 "0R" H 13355 4785 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 13325 4825 50  0001 C CNN
F 3 "" H 13325 4825 50  0000 C CNN
	1    13325 4825
	1    0    0    -1  
$EndComp
$Comp
L R_Small R32
U 1 1 57BA327E
P 8325 7725
F 0 "R32" H 8355 7745 50  0000 L CNN
F 1 "2k7" H 8355 7685 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 8325 7725 50  0001 C CNN
F 3 "" H 8325 7725 50  0000 C CNN
	1    8325 7725
	0    -1   -1   0   
$EndComp
$Comp
L C_Small C24
U 1 1 57BA3285
P 8500 7900
F 0 "C24" H 8510 7970 50  0000 L CNN
F 1 "n.B." H 8510 7820 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 8500 7900 50  0001 C CNN
F 3 "" H 8500 7900 50  0000 C CNN
	1    8500 7900
	-1   0    0    -1  
$EndComp
$Comp
L GNDA #PWR71
U 1 1 57BA3299
P 8825 8125
F 0 "#PWR71" H 8825 7875 50  0001 C CNN
F 1 "GNDA" H 8825 7975 50  0000 C CNN
F 2 "" H 8825 8125 50  0000 C CNN
F 3 "" H 8825 8125 50  0000 C CNN
	1    8825 8125
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR61
U 1 1 57BA329F
P 8500 8125
F 0 "#PWR61" H 8500 7875 50  0001 C CNN
F 1 "GNDA" H 8500 7975 50  0000 C CNN
F 2 "" H 8500 8125 50  0000 C CNN
F 3 "" H 8500 8125 50  0000 C CNN
	1    8500 8125
	1    0    0    -1  
$EndComp
$Comp
L R_Small R28
U 1 1 57BA9ED0
P 8650 9825
F 0 "R28" H 8680 9845 50  0000 L CNN
F 1 "n.B." H 8680 9785 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 8650 9825 50  0001 C CNN
F 3 "" H 8650 9825 50  0000 C CNN
	1    8650 9825
	0    -1   -1   0   
$EndComp
$Comp
L R_Small R21
U 1 1 57BAAFDF
P 7725 10500
F 0 "R21" H 7755 10520 50  0000 L CNN
F 1 "n.B." H 7755 10460 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 7725 10500 50  0001 C CNN
F 3 "" H 7725 10500 50  0000 C CNN
	1    7725 10500
	1    0    0    -1  
$EndComp
$Comp
L R_Small R24
U 1 1 57BAB999
P 7925 9275
F 0 "R24" H 7955 9295 50  0000 L CNN
F 1 "n.B." H 7955 9235 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 7925 9275 50  0001 C CNN
F 3 "" H 7925 9275 50  0000 C CNN
	1    7925 9275
	0    -1   -1   0   
$EndComp
$Comp
L GNDA #PWR51
U 1 1 57BABE57
P 7975 10125
F 0 "#PWR51" H 7975 9875 50  0001 C CNN
F 1 "GNDA" H 7975 9975 50  0000 C CNN
F 2 "" H 7975 10125 50  0000 C CNN
F 3 "" H 7975 10125 50  0000 C CNN
	1    7975 10125
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR49
U 1 1 57BAEBF3
P 7725 10600
F 0 "#PWR49" H 7725 10350 50  0001 C CNN
F 1 "GNDA" H 7725 10450 50  0000 C CNN
F 2 "" H 7725 10600 50  0000 C CNN
F 3 "" H 7725 10600 50  0000 C CNN
	1    7725 10600
	1    0    0    -1  
$EndComp
$Comp
L R_Small R17
U 1 1 57BB257D
P 7350 10500
F 0 "R17" H 7380 10520 50  0000 L CNN
F 1 "10k" H 7380 10460 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 7350 10500 50  0001 C CNN
F 3 "" H 7350 10500 50  0000 C CNN
	1    7350 10500
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR46
U 1 1 57BB2793
P 7350 10600
F 0 "#PWR46" H 7350 10350 50  0001 C CNN
F 1 "GNDA" H 7350 10450 50  0000 C CNN
F 2 "" H 7350 10600 50  0000 C CNN
F 3 "" H 7350 10600 50  0000 C CNN
	1    7350 10600
	1    0    0    -1  
$EndComp
$Comp
L R_Small R16
U 1 1 57BB293C
P 7350 9125
F 0 "R16" H 7380 9145 50  0000 L CNN
F 1 "n.B." H 7380 9085 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 7350 9125 50  0001 C CNN
F 3 "" H 7350 9125 50  0000 C CNN
	1    7350 9125
	1    0    0    -1  
$EndComp
$Comp
L R_Small R19
U 1 1 57BB2B23
P 7550 9125
F 0 "R19" H 7580 9145 50  0000 L CNN
F 1 "n.B." H 7580 9085 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 7550 9125 50  0001 C CNN
F 3 "" H 7550 9125 50  0000 C CNN
	1    7550 9125
	1    0    0    -1  
$EndComp
$Comp
L C_Small C18
U 1 1 57BB5A45
P 7925 9025
F 0 "C18" H 7935 9095 50  0000 L CNN
F 1 "n.B." H 7935 8945 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 7925 9025 50  0001 C CNN
F 3 "" H 7925 9025 50  0000 C CNN
	1    7925 9025
	0    -1   -1   0   
$EndComp
$Comp
L R_Small R37
U 1 1 57B9F72C
P 10650 9825
F 0 "R37" H 10680 9845 50  0000 L CNN
F 1 "n.B." H 10680 9785 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 10650 9825 50  0001 C CNN
F 3 "" H 10650 9825 50  0000 C CNN
	1    10650 9825
	0    -1   -1   0   
$EndComp
$Comp
L R_Small R34
U 1 1 57B9F737
P 9725 10500
F 0 "R34" H 9755 10520 50  0000 L CNN
F 1 "n.B." H 9755 10460 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 9725 10500 50  0001 C CNN
F 3 "" H 9725 10500 50  0000 C CNN
	1    9725 10500
	1    0    0    -1  
$EndComp
$Comp
L R_Small R35
U 1 1 57B9F745
P 9925 9275
F 0 "R35" H 9955 9295 50  0000 L CNN
F 1 "n.B." H 9955 9235 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 9925 9275 50  0001 C CNN
F 3 "" H 9925 9275 50  0000 C CNN
	1    9925 9275
	0    -1   -1   0   
$EndComp
$Comp
L GNDA #PWR80
U 1 1 57B9F752
P 9975 10125
F 0 "#PWR80" H 9975 9875 50  0001 C CNN
F 1 "GNDA" H 9975 9975 50  0000 C CNN
F 2 "" H 9975 10125 50  0000 C CNN
F 3 "" H 9975 10125 50  0000 C CNN
	1    9975 10125
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR79
U 1 1 57B9F75D
P 9725 10600
F 0 "#PWR79" H 9725 10350 50  0001 C CNN
F 1 "GNDA" H 9725 10450 50  0000 C CNN
F 2 "" H 9725 10600 50  0000 C CNN
F 3 "" H 9725 10600 50  0000 C CNN
	1    9725 10600
	1    0    0    -1  
$EndComp
$Comp
L R_Small R31
U 1 1 57B9F763
P 9350 10500
F 0 "R31" H 9380 10520 50  0000 L CNN
F 1 "10k" H 9380 10460 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 9350 10500 50  0001 C CNN
F 3 "" H 9350 10500 50  0000 C CNN
	1    9350 10500
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR74
U 1 1 57B9F76A
P 9350 10600
F 0 "#PWR74" H 9350 10350 50  0001 C CNN
F 1 "GNDA" H 9350 10450 50  0000 C CNN
F 2 "" H 9350 10600 50  0000 C CNN
F 3 "" H 9350 10600 50  0000 C CNN
	1    9350 10600
	1    0    0    -1  
$EndComp
$Comp
L R_Small R30
U 1 1 57B9F770
P 9350 9125
F 0 "R30" H 9380 9145 50  0000 L CNN
F 1 "n.B." H 9380 9085 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 9350 9125 50  0001 C CNN
F 3 "" H 9350 9125 50  0000 C CNN
	1    9350 9125
	1    0    0    -1  
$EndComp
$Comp
L R_Small R33
U 1 1 57B9F777
P 9550 9125
F 0 "R33" H 9580 9145 50  0000 L CNN
F 1 "n.B." H 9580 9085 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 9550 9125 50  0001 C CNN
F 3 "" H 9550 9125 50  0000 C CNN
	1    9550 9125
	1    0    0    -1  
$EndComp
$Comp
L C_Small C25
U 1 1 57B9F790
P 9925 9025
F 0 "C25" H 9935 9095 50  0000 L CNN
F 1 "n.B." H 9935 8945 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 9925 9025 50  0001 C CNN
F 3 "" H 9925 9025 50  0000 C CNN
	1    9925 9025
	0    -1   -1   0   
$EndComp
$Comp
L +3V3 #PWR95
U 1 1 57BA075B
P 12100 5875
F 0 "#PWR95" H 12100 5725 50  0001 C CNN
F 1 "+3V3" H 12100 6015 50  0000 C CNN
F 2 "" H 12100 5875 50  0000 C CNN
F 3 "" H 12100 5875 50  0000 C CNN
	1    12100 5875
	0    -1   -1   0   
$EndComp
$Comp
L +5V #PWR30
U 1 1 57BA0928
P 6200 10425
F 0 "#PWR30" H 6200 10275 50  0001 C CNN
F 1 "+5V" H 6200 10565 50  0000 C CNN
F 2 "" H 6200 10425 50  0000 C CNN
F 3 "" H 6200 10425 50  0000 C CNN
	1    6200 10425
	0    1    1    0   
$EndComp
$Comp
L +5V #PWR115
U 1 1 57BA2BA1
P 13900 5725
F 0 "#PWR115" H 13900 5575 50  0001 C CNN
F 1 "+5V" H 13900 5865 50  0000 C CNN
F 2 "" H 13900 5725 50  0000 C CNN
F 3 "" H 13900 5725 50  0000 C CNN
	1    13900 5725
	0    1    1    0   
$EndComp
$Comp
L +5V #PWR117
U 1 1 57BA2F3C
P 13900 6300
F 0 "#PWR117" H 13900 6150 50  0001 C CNN
F 1 "+5V" H 13900 6440 50  0000 C CNN
F 2 "" H 13900 6300 50  0000 C CNN
F 3 "" H 13900 6300 50  0000 C CNN
	1    13900 6300
	0    1    1    0   
$EndComp
$Comp
L +3V3 #PWR131
U 1 1 57BA81F3
P 14975 5150
F 0 "#PWR131" H 14975 5000 50  0001 C CNN
F 1 "+3V3" H 14975 5290 50  0000 C CNN
F 2 "" H 14975 5150 50  0000 C CNN
F 3 "" H 14975 5150 50  0000 C CNN
	1    14975 5150
	0    -1   -1   0   
$EndComp
$Comp
L +5V #PWR146
U 1 1 57BA8C18
P 15475 5150
F 0 "#PWR146" H 15475 5000 50  0001 C CNN
F 1 "+5V" H 15475 5290 50  0000 C CNN
F 2 "" H 15475 5150 50  0000 C CNN
F 3 "" H 15475 5150 50  0000 C CNN
	1    15475 5150
	0    1    1    0   
$EndComp
$Comp
L +3V3 #PWR133
U 1 1 57BA96EF
P 14975 6050
F 0 "#PWR133" H 14975 5900 50  0001 C CNN
F 1 "+3V3" H 14975 6190 50  0000 C CNN
F 2 "" H 14975 6050 50  0000 C CNN
F 3 "" H 14975 6050 50  0000 C CNN
	1    14975 6050
	0    -1   -1   0   
$EndComp
$Comp
L +5V #PWR148
U 1 1 57BA98BC
P 15475 6050
F 0 "#PWR148" H 15475 5900 50  0001 C CNN
F 1 "+5V" H 15475 6190 50  0000 C CNN
F 2 "" H 15475 6050 50  0000 C CNN
F 3 "" H 15475 6050 50  0000 C CNN
	1    15475 6050
	0    1    1    0   
$EndComp
$Comp
L TSR_1- U5
U 1 1 57B46950
P 7025 2175
F 0 "U5" H 7025 2425 60  0000 C CNN
F 1 "TSR_1-2450" H 7025 2225 60  0000 C CNN
F 2 "Custom:TSR1" H 7025 2225 60  0001 C CNN
F 3 "http://www.tracopower.com/products/tsr1.pdf" H 7025 2225 60  0001 C CNN
F 4 "Farnell" H 7025 2175 60  0001 C CNN "Supply"
F 5 "1696320" H 7025 2175 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/tracopower/tsr-1-2450/wandler-dc-dc-5v-1a-sip/dp/1696320?selectedCategoryId=&exaMfpn=true&categoryId=&searchRef=SearchLookAhead&searchView=table&iscrfnonsku=false" H 7025 2175 60  0001 C CNN "URL"
F 7 "\"4,4\"" H 7025 2175 60  0001 C CNN "Price"
	1    7025 2175
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GSD Q2
U 1 1 57BDC724
P 5225 6725
F 0 "Q2" H 5525 6775 50  0000 R CNN
F 1 "IRLML6344" V 5475 6700 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 5425 6825 50  0001 C CNN
F 3 "http://www.farnell.com/datasheets/1911844.pdf" H 5225 6725 50  0001 C CNN
F 4 "Farnell" H 5225 6725 60  0001 C CNN "Supply"
F 5 "1857299" H 5225 6725 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/international-rectifier/irlml6344trpbf/mosfet-n-kanal-30v-5a-sot23/dp/1857299" H 5225 6725 60  0001 C CNN "URL"
F 7 "\"0,168\"" H 5225 6725 60  0001 C CNN "Price"
F 8 "SOT-23" H 5225 6725 60  0001 C CNN "Package"
	1    5225 6725
	1    0    0    -1  
$EndComp
$Comp
L R_Small R4
U 1 1 57BDC72B
P 4825 6875
F 0 "R4" H 4855 6895 50  0000 L CNN
F 1 "10k" H 4855 6835 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 4825 6875 50  0001 C CNN
F 3 "" H 4825 6875 50  0000 C CNN
	1    4825 6875
	-1   0    0    -1  
$EndComp
$Comp
L Q_PMOS_GSD Q4
U 1 1 57BDC737
P 5600 6450
F 0 "Q4" H 5900 6500 50  0000 R CNN
F 1 "IRLML9301" V 5850 6425 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 5800 6550 50  0001 C CNN
F 3 "http://www.farnell.com/datasheets/1749827.pdf" H 5600 6450 50  0001 C CNN
F 4 "farnell" H 5600 6450 60  0001 C CNN "Supply"
F 5 "1831089" H 5600 6450 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/infineon/irlml9301trpbf/mosfet-diode-p-kanal-30v-3-6a/dp/1831089" H 5600 6450 60  0001 C CNN "URL"
F 7 "\"0,224\"" H 5600 6450 60  0001 C CNN "Price"
F 8 "SOT-23" H 5600 6450 60  0001 C CNN "Package"
	1    5600 6450
	1    0    0    1   
$EndComp
$Comp
L R_Small R12
U 1 1 57BDC73E
P 5475 6200
F 0 "R12" H 5505 6220 50  0000 L CNN
F 1 "10k" H 5505 6160 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 5475 6200 50  0001 C CNN
F 3 "" H 5475 6200 50  0000 C CNN
	1    5475 6200
	0    -1   1    0   
$EndComp
$Comp
L C_Small C15
U 1 1 57BDC745
P 5475 6075
F 0 "C15" H 5485 6145 50  0000 L CNN
F 1 "100n" H 5485 5995 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 5475 6075 50  0001 C CNN
F 3 "" H 5475 6075 50  0000 C CNN
	1    5475 6075
	0    1    -1   0   
$EndComp
$Comp
L GND #PWR12
U 1 1 57BDCBA2
P 4825 7375
F 0 "#PWR12" H 4825 7125 50  0001 C CNN
F 1 "GND" H 4825 7225 50  0000 C CNN
F 2 "" H 4825 7375 50  0000 C CNN
F 3 "" H 4825 7375 50  0000 C CNN
	1    4825 7375
	-1   0    0    -1  
$EndComp
$Comp
L GND #PWR19
U 1 1 57BDD120
P 5325 7000
F 0 "#PWR19" H 5325 6750 50  0001 C CNN
F 1 "GND" H 5325 6850 50  0000 C CNN
F 2 "" H 5325 7000 50  0000 C CNN
F 3 "" H 5325 7000 50  0000 C CNN
	1    5325 7000
	-1   0    0    -1  
$EndComp
$Comp
L TEN_30- U2
U 1 1 57B4DF37
P 6600 6625
F 0 "U2" H 6600 7025 60  0000 C CNN
F 1 "TEN_30-1212" H 6600 6225 60  0000 C CNN
F 2 "Custom:TEN30" H 6650 6425 60  0001 C CNN
F 3 "http://www.farnell.com/datasheets/1763557.pdf" H 6650 6425 60  0001 C CNN
F 4 "Farnell" H 6600 6625 60  0001 C CNN "Supply"
F 5 "1772204" H 6600 6625 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/tracopower/ten-30-1212/wandler-dc-dc-30w-12v-2-5a/dp/1772204?selectedCategoryId=&exaMfpn=true&categoryId=&searchRef=SearchLookAhead&searchView=table&iscrfnonsku=false" H 6600 6625 60  0001 C CNN "URL"
F 7 "\"56,50\"" H 6600 6625 60  0001 C CNN "Price"
	1    6600 6625
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR13
U 1 1 57BE7BC6
P 4975 7375
F 0 "#PWR13" H 4975 7125 50  0001 C CNN
F 1 "GND" H 4975 7225 50  0000 C CNN
F 2 "" H 4975 7375 50  0000 C CNN
F 3 "" H 4975 7375 50  0000 C CNN
	1    4975 7375
	-1   0    0    -1  
$EndComp
$Comp
L R_Small R14
U 1 1 57BEA56E
P 5700 6975
F 0 "R14" H 5730 6995 50  0000 L CNN
F 1 "10k" H 5730 6935 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 5700 6975 50  0001 C CNN
F 3 "" H 5700 6975 50  0000 C CNN
	1    5700 6975
	-1   0    0    -1  
$EndComp
$Comp
L R_Small R20
U 1 1 57BEF0B4
P 6450 5350
F 0 "R20" H 6480 5370 50  0000 L CNN
F 1 "0R" H 6480 5310 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 6450 5350 50  0001 C CNN
F 3 "" H 6450 5350 50  0000 C CNN
	1    6450 5350
	0    -1   -1   0   
$EndComp
$Comp
L L_Small L2
U 1 1 57BEF226
P 6200 5350
F 0 "L2" H 6230 5390 50  0000 L CNN
F 1 "L_Small" H 6230 5310 50  0000 L CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 6200 5350 50  0001 C CNN
F 3 "" H 6200 5350 50  0000 C CNN
F 4 "Spule oder Ferritperle" H 6200 5350 60  0001 C CNN "Spec"
	1    6200 5350
	0    -1   1    0   
$EndComp
$Comp
L CP_Small C22
U 1 1 57A12EA0
P 8075 1275
F 0 "C22" H 8085 1345 50  0000 L CNN
F 1 "1m" H 8085 1195 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Radial_D10_L20_P5-7.5" H 8075 1275 50  0001 C CNN
F 3 "" H 8075 1275 50  0000 C CNN
F 4 "Farnell" H 8075 1275 60  0001 C CNN "Supply"
F 5 "2469429" H 8075 1275 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/rubycon/25zls1000mefc10x20/alu-elko-1000uf-25v-radial/dp/2469429" H 8075 1275 60  0001 C CNN "URL"
F 7 "\"0,661\"" H 8075 1275 60  0001 C CNN "Price"
F 8 "Elko 6V3" H 8075 1275 60  0001 C CNN "Spec"
	1    8075 1275
	1    0    0    -1  
$EndComp
Text Label 1425 2500 2    60   ~ 0
uC_VDDA
Text Label 1400 3275 2    60   ~ 0
uC_VDD
Text Label 825  5375 2    60   ~ 0
uC_VDD
Text Label 1350 5375 0    60   ~ 0
uC_VDDA
$Comp
L L_Small L3
U 1 1 57BBAA3D
P 950 5375
F 0 "L3" H 980 5415 50  0000 L CNN
F 1 "Ferrit" H 980 5335 50  0000 L CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 950 5375 50  0001 C CNN
F 3 "" H 950 5375 50  0000 C CNN
F 4 "Spule oder Ferritperle" H 950 5375 60  0001 C CNN "Spec"
	1    950  5375
	0    -1   1    0   
$EndComp
$Comp
L +3V3 #PWR6
U 1 1 57C092EE
P 1450 3225
F 0 "#PWR6" H 1450 3075 50  0001 C CNN
F 1 "+3V3" H 1450 3365 50  0000 C CNN
F 2 "" H 1450 3225 50  0000 C CNN
F 3 "" H 1450 3225 50  0000 C CNN
	1    1450 3225
	1    0    0    -1  
$EndComp
$Comp
L R_Small R58
U 1 1 57C47D6B
P 9425 7825
F 0 "R58" H 9455 7845 50  0000 L CNN
F 1 "0R" H 9455 7785 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 9425 7825 50  0001 C CNN
F 3 "" H 9425 7825 50  0000 C CNN
	1    9425 7825
	0    -1   -1   0   
$EndComp
$Comp
L R_Small R57
U 1 1 57C49465
P 9350 4825
F 0 "R57" H 9380 4845 50  0000 L CNN
F 1 "0R" H 9380 4785 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 9350 4825 50  0001 C CNN
F 3 "" H 9350 4825 50  0000 C CNN
	1    9350 4825
	0    -1   -1   0   
$EndComp
$Comp
L CONN_02X03 P5
U 1 1 57C56E32
P 8775 8950
F 0 "P5" H 8775 9150 50  0000 C CNN
F 1 "ADC12" H 8775 8750 50  0000 C CNN
F 2 "Custom:IDC_Header_Straight_6pins" H 8775 7750 50  0001 C CNN
F 3 "" H 8775 7750 50  0000 C CNN
F 4 "Reichelt" H 8775 8950 60  0001 C CNN "Supply"
F 5 "WSL 6G" H 8775 8950 60  0001 C CNN "Part No."
F 6 "http://www.reichelt.de/Pfosten-Wannenstecker/WSL-6G/3/index.html?ACTION=3&LA=2&ARTICLE=85732&GROUPID=7437&artnr=WSL+6G" H 8775 8950 60  0001 C CNN "URL"
F 7 "\"0,16\"" H 8775 8950 60  0001 C CNN "Price"
F 8 "2x3" H 8775 8950 60  0001 C CNN "Package"
	1    8775 8950
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR72
U 1 1 57C5807F
P 9025 8950
F 0 "#PWR72" H 9025 8800 50  0001 C CNN
F 1 "+5V" H 9025 9090 50  0000 C CNN
F 2 "" H 9025 8950 50  0000 C CNN
F 3 "" H 9025 8950 50  0000 C CNN
	1    9025 8950
	0    1    1    0   
$EndComp
$Comp
L GNDA #PWR25
U 1 1 57C5E0AA
P 5700 10525
F 0 "#PWR25" H 5700 10275 50  0001 C CNN
F 1 "GNDA" H 5700 10375 50  0000 C CNN
F 2 "" H 5700 10525 50  0000 C CNN
F 3 "" H 5700 10525 50  0000 C CNN
	1    5700 10525
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR31
U 1 1 57C5EC04
P 6200 10525
F 0 "#PWR31" H 6200 10275 50  0001 C CNN
F 1 "GNDA" H 6200 10375 50  0000 C CNN
F 2 "" H 6200 10525 50  0000 C CNN
F 3 "" H 6200 10525 50  0000 C CNN
	1    6200 10525
	1    0    0    -1  
$EndComp
Text Label 5050 10225 2    60   ~ 0
PD14
$Comp
L R_Small R52
U 1 1 57C6EA5B
P 5400 10225
F 0 "R52" H 5430 10245 50  0000 L CNN
F 1 "0R" H 5430 10185 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 5400 10225 50  0001 C CNN
F 3 "" H 5400 10225 50  0000 C CNN
	1    5400 10225
	0    1    -1   0   
$EndComp
$Comp
L C_Small C40
U 1 1 57C6EA64
P 5125 10350
F 0 "C40" H 5135 10420 50  0000 L CNN
F 1 "n.B." H 5135 10270 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 5125 10350 50  0001 C CNN
F 3 "" H 5125 10350 50  0000 C CNN
	1    5125 10350
	0    1    1    0   
$EndComp
$Comp
L R_Small R51
U 1 1 57C6EA70
P 5400 10000
F 0 "R51" H 5430 10020 50  0000 L CNN
F 1 "0R" H 5430 9960 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 5400 10000 50  0001 C CNN
F 3 "" H 5400 10000 50  0000 C CNN
	1    5400 10000
	0    1    -1   0   
$EndComp
$Comp
L C_Small C41
U 1 1 57C6EA77
P 5225 9850
F 0 "C41" H 5235 9920 50  0000 L CNN
F 1 "n.B." H 5235 9770 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 5225 9850 50  0001 C CNN
F 3 "" H 5225 9850 50  0000 C CNN
	1    5225 9850
	-1   0    0    -1  
$EndComp
$Comp
L GNDA #PWR14
U 1 1 57C6EA87
P 5025 10350
F 0 "#PWR14" H 5025 10100 50  0001 C CNN
F 1 "GNDA" H 5025 10200 50  0000 C CNN
F 2 "" H 5025 10350 50  0000 C CNN
F 3 "" H 5025 10350 50  0000 C CNN
	1    5025 10350
	0    1    1    0   
$EndComp
$Comp
L GNDA #PWR17
U 1 1 57C6EA8D
P 5225 9750
F 0 "#PWR17" H 5225 9500 50  0001 C CNN
F 1 "GNDA" H 5225 9600 50  0000 C CNN
F 2 "" H 5225 9750 50  0000 C CNN
F 3 "" H 5225 9750 50  0000 C CNN
	1    5225 9750
	1    0    0    1   
$EndComp
$Comp
L R_Small R54
U 1 1 57C77ED6
P 6475 10475
F 0 "R54" H 6505 10495 50  0000 L CNN
F 1 "0R" H 6505 10435 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 6475 10475 50  0001 C CNN
F 3 "" H 6475 10475 50  0000 C CNN
	1    6475 10475
	1    0    0    -1  
$EndComp
$Comp
L C_Small C43
U 1 1 57C77EDD
P 6475 10725
F 0 "C43" H 6485 10795 50  0000 L CNN
F 1 "n.B." H 6485 10645 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 6475 10725 50  0001 C CNN
F 3 "" H 6475 10725 50  0000 C CNN
	1    6475 10725
	1    0    0    -1  
$EndComp
$Comp
L R_Small R56
U 1 1 57C77EE5
P 6500 10225
F 0 "R56" H 6530 10245 50  0000 L CNN
F 1 "0R" H 6530 10185 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 6500 10225 50  0001 C CNN
F 3 "" H 6500 10225 50  0000 C CNN
	1    6500 10225
	0    -1   -1   0   
$EndComp
$Comp
L C_Small C45
U 1 1 57C77EED
P 6775 10350
F 0 "C45" H 6785 10420 50  0000 L CNN
F 1 "n.B." H 6785 10270 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 6775 10350 50  0001 C CNN
F 3 "" H 6775 10350 50  0000 C CNN
	1    6775 10350
	0    -1   1    0   
$EndComp
$Comp
L R_Small R55
U 1 1 57C77EF7
P 6500 10000
F 0 "R55" H 6530 10020 50  0000 L CNN
F 1 "0R" H 6530 9960 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 6500 10000 50  0001 C CNN
F 3 "" H 6500 10000 50  0000 C CNN
	1    6500 10000
	0    -1   -1   0   
$EndComp
$Comp
L C_Small C44
U 1 1 57C77EFE
P 6675 9850
F 0 "C44" H 6685 9920 50  0000 L CNN
F 1 "n.B." H 6685 9770 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 6675 9850 50  0001 C CNN
F 3 "" H 6675 9850 50  0000 C CNN
	1    6675 9850
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR33
U 1 1 57C77F07
P 6475 10825
F 0 "#PWR33" H 6475 10575 50  0001 C CNN
F 1 "GNDA" H 6475 10675 50  0000 C CNN
F 2 "" H 6475 10825 50  0000 C CNN
F 3 "" H 6475 10825 50  0000 C CNN
	1    6475 10825
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR38
U 1 1 57C77F0D
P 6675 9750
F 0 "#PWR38" H 6675 9500 50  0001 C CNN
F 1 "GNDA" H 6675 9600 50  0000 C CNN
F 2 "" H 6675 9750 50  0000 C CNN
F 3 "" H 6675 9750 50  0000 C CNN
	1    6675 9750
	-1   0    0    1   
$EndComp
$Comp
L GNDA #PWR39
U 1 1 57C7834E
P 6875 10350
F 0 "#PWR39" H 6875 10100 50  0001 C CNN
F 1 "GNDA" H 6875 10200 50  0000 C CNN
F 2 "" H 6875 10350 50  0000 C CNN
F 3 "" H 6875 10350 50  0000 C CNN
	1    6875 10350
	0    -1   -1   0   
$EndComp
Text Notes 4800 9550 0    60   ~ 0
Zusätzliche GPIO Analog
$Comp
L R_Small R59
U 1 1 57CBA8DF
P 1200 5375
F 0 "R59" H 1230 5395 50  0000 L CNN
F 1 "0R" H 1230 5335 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 1200 5375 50  0001 C CNN
F 3 "" H 1200 5375 50  0000 C CNN
	1    1200 5375
	0    1    1    0   
$EndComp
$Comp
L C_Small C46
U 1 1 57CCE3DA
P 1075 5775
F 0 "C46" H 1085 5845 50  0000 L CNN
F 1 "n.B." H 1085 5695 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 1075 5775 50  0001 C CNN
F 3 "" H 1075 5775 50  0000 C CNN
	1    1075 5775
	-1   0    0    -1  
$EndComp
$Comp
L GNDA #PWR4
U 1 1 57CCFA63
P 1075 5925
F 0 "#PWR4" H 1075 5675 50  0001 C CNN
F 1 "GNDA" H 1075 5775 50  0000 C CNN
F 2 "" H 1075 5925 50  0000 C CNN
F 3 "" H 1075 5925 50  0000 C CNN
	1    1075 5925
	1    0    0    -1  
$EndComp
$Comp
L DIP_SW_4x P14
U 1 1 57C695CF
P 6425 9125
F 0 "P14" H 6425 9375 50  0000 C CNN
F 1 "CAR-ID" H 6425 8875 50  0000 C CNN
F 2 "Buttons_Switches_ThroughHole:SW_DIP_x4_Slide" H 6425 7925 50  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C200/NT06%24_NT02%24_NT04%24_NT06%24_NT08%24_NT10%24%23IMP.pdf" H 6425 7925 50  0001 C CNN
F 4 "Reichelt" H 6425 9125 60  0001 C CNN "Supply"
F 5 "NT 04" H 6425 9125 60  0001 C CNN "Part No."
F 6 "http://www.reichelt.de/Dip-Kodierschalter/NT-04/3/index.html?&ACTION=3&LA=2&ARTICLE=13532&GROUPID=7598&artnr=NT+04" H 6425 9125 60  0001 C CNN "URL"
F 7 "\"0,24\"" H 6425 9125 60  0001 C CNN "Price"
F 8 "2x4 DIP SW" H 6425 9125 60  0001 C CNN "Package"
	1    6425 9125
	1    0    0    -1  
$EndComp
$Comp
L GNDPWR #PWR32
U 1 1 57C7512E
P 6275 7450
F 0 "#PWR32" H 6275 7250 50  0001 C CNN
F 1 "GNDPWR" H 6275 7320 50  0000 C CNN
F 2 "" H 6275 7400 50  0000 C CNN
F 3 "" H 6275 7400 50  0000 C CNN
	1    6275 7450
	1    0    0    -1  
$EndComp
$Comp
L GNDPWR #PWR35
U 1 1 57C7611B
P 6625 7650
F 0 "#PWR35" H 6625 7450 50  0001 C CNN
F 1 "GNDPWR" H 6625 7520 50  0000 C CNN
F 2 "" H 6625 7600 50  0000 C CNN
F 3 "" H 6625 7600 50  0000 C CNN
	1    6625 7650
	0    1    1    0   
$EndComp
$Comp
L GNDPWR #PWR43
U 1 1 57C7688F
P 7325 7650
F 0 "#PWR43" H 7325 7450 50  0001 C CNN
F 1 "GNDPWR" H 7325 7520 50  0000 C CNN
F 2 "" H 7325 7600 50  0000 C CNN
F 3 "" H 7325 7600 50  0000 C CNN
	1    7325 7650
	0    -1   -1   0   
$EndComp
$Comp
L +12V #PWR36
U 1 1 57C76A98
P 6625 7875
F 0 "#PWR36" H 6625 7725 50  0001 C CNN
F 1 "+12V" H 6625 8015 50  0000 C CNN
F 2 "" H 6625 7875 50  0000 C CNN
F 3 "" H 6625 7875 50  0000 C CNN
	1    6625 7875
	-1   0    0    1   
$EndComp
$Comp
L GNDPWR #PWR41
U 1 1 57C7DA2F
P 7025 2575
F 0 "#PWR41" H 7025 2375 50  0001 C CNN
F 1 "GNDPWR" H 7025 2445 50  0000 C CNN
F 2 "" H 7025 2525 50  0000 C CNN
F 3 "" H 7025 2525 50  0000 C CNN
	1    7025 2575
	1    0    0    -1  
$EndComp
$Comp
L C_Small C37
U 1 1 57C7E788
P 11475 2525
F 0 "C37" H 11485 2595 50  0000 L CNN
F 1 "4u7" H 11485 2445 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206_HandSoldering" H 11475 2525 50  0001 C CNN
F 3 "" H 11475 2525 50  0000 C CNN
F 4 "Farnell" H 11475 2525 60  0001 C CNN "Supply"
F 5 "1828835" H 11475 2525 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/murata/grm31cr71e475ka88l/kondensator-mlcc-x7r-4-7uf-25v/dp/1828835" H 11475 2525 60  0001 C CNN "URL"
F 7 "\"0,154\"" H 11475 2525 60  0001 C CNN "Price"
F 8 "1206" H 11475 2525 60  0001 C CNN "Package"
F 9 "25V" H 11475 2525 60  0001 C CNN "Spec"
	1    11475 2525
	1    0    0    -1  
$EndComp
$Comp
L C_Small C1
U 1 1 57C7FBCE
P 550 3750
F 0 "C1" H 560 3820 50  0000 L CNN
F 1 "4u7" H 560 3670 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206_HandSoldering" H 550 3750 50  0001 C CNN
F 3 "" H 550 3750 50  0000 C CNN
F 4 "Farnell" H 550 3750 60  0001 C CNN "Supply"
F 5 "1828835" H 550 3750 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/murata/grm31cr71e475ka88l/kondensator-mlcc-x7r-4-7uf-25v/dp/1828835" H 550 3750 60  0001 C CNN "URL"
F 7 "\"0,154\"" H 550 3750 60  0001 C CNN "Price"
F 8 "1206" H 550 3750 60  0001 C CNN "Package"
F 9 "25V" H 550 3750 60  0001 C CNN "Spec"
	1    550  3750
	1    0    0    -1  
$EndComp
$Comp
L C_Small C2
U 1 1 57C80521
P 575 2775
F 0 "C2" H 585 2845 50  0000 L CNN
F 1 "1u" H 585 2695 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 575 2775 50  0001 C CNN
F 3 "" H 575 2775 50  0000 C CNN
F 4 "Farnell" H 575 2775 60  0001 C CNN "Supply"
F 5 "2346901" H 575 2775 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/tdk/c1608x7r1e105k080ab/kondensator-mlcc-x7r-1uf-25v-0603/dp/2346901" H 575 2775 60  0001 C CNN "URL"
F 7 "\"0,0858\"" H 575 2775 60  0001 C CNN "Price"
F 8 "0603" H 575 2775 60  0001 C CNN "Package"
F 9 "6V3" H 575 2775 60  0001 C CNN "Spec"
	1    575  2775
	1    0    0    -1  
$EndComp
$Comp
L C_Small C6
U 1 1 57C8147E
P 1075 2800
F 0 "C6" H 1085 2870 50  0000 L CNN
F 1 "1u" H 1085 2720 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 1075 2800 50  0001 C CNN
F 3 "" H 1075 2800 50  0000 C CNN
F 4 "Farnell" H 1075 2800 60  0001 C CNN "Supply"
F 5 "2346901" H 1075 2800 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/tdk/c1608x7r1e105k080ab/kondensator-mlcc-x7r-1uf-25v-0603/dp/2346901" H 1075 2800 60  0001 C CNN "URL"
F 7 "\"0,0858\"" H 1075 2800 60  0001 C CNN "Price"
F 8 "0603" H 1075 2800 60  0001 C CNN "Package"
F 9 "6V3" H 1075 2800 60  0001 C CNN "Spec"
	1    1075 2800
	1    0    0    -1  
$EndComp
$Comp
L R_Small R15
U 1 1 57C82499
P 7475 9725
F 0 "R15" H 7505 9745 50  0000 L CNN
F 1 "n.B." H 7505 9685 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 7475 9725 50  0001 C CNN
F 3 "" H 7475 9725 50  0000 C CNN
	1    7475 9725
	0    1    1    0   
$EndComp
$Comp
L C_Small C47
U 1 1 57C828DD
P 7600 10125
F 0 "C47" H 7610 10195 50  0000 L CNN
F 1 "n.B." H 7610 10045 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 7600 10125 50  0001 C CNN
F 3 "" H 7600 10125 50  0000 C CNN
	1    7600 10125
	-1   0    0    1   
$EndComp
$Comp
L GNDA #PWR48
U 1 1 57C84141
P 7600 10225
F 0 "#PWR48" H 7600 9975 50  0001 C CNN
F 1 "GNDA" H 7600 10075 50  0000 C CNN
F 2 "" H 7600 10225 50  0000 C CNN
F 3 "" H 7600 10225 50  0000 C CNN
	1    7600 10225
	1    0    0    -1  
$EndComp
$Comp
L LED D6
U 1 1 57C85B77
P 12175 9725
F 0 "D6" H 12175 9825 50  0000 C CNN
F 1 "LED" H 12175 9625 50  0000 C CNN
F 2 "Custom:LED_0603_HandSoldering" H 12175 9725 50  0001 C CNN
F 3 "" H 12175 9725 50  0000 C CNN
	1    12175 9725
	-1   0    0    -1  
$EndComp
$Comp
L R_Small R60
U 1 1 57C85B7E
P 11850 9725
F 0 "R60" H 11880 9745 50  0000 L CNN
F 1 "680R" H 11880 9685 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 11850 9725 50  0001 C CNN
F 3 "" H 11850 9725 50  0000 C CNN
	1    11850 9725
	0    -1   -1   0   
$EndComp
Text Label 11700 9725 2    60   ~ 0
LED4
Text Label 3750 2325 0    60   ~ 0
LED4
$Comp
L R_Small R61
U 1 1 57C91B66
P 9475 9725
F 0 "R61" H 9505 9745 50  0000 L CNN
F 1 "n.B." H 9505 9685 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 9475 9725 50  0001 C CNN
F 3 "" H 9475 9725 50  0000 C CNN
	1    9475 9725
	0    1    1    0   
$EndComp
$Comp
L C_Small C48
U 1 1 57C91B6D
P 9600 10125
F 0 "C48" H 9610 10195 50  0000 L CNN
F 1 "n.B." H 9610 10045 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 9600 10125 50  0001 C CNN
F 3 "" H 9600 10125 50  0000 C CNN
	1    9600 10125
	-1   0    0    1   
$EndComp
$Comp
L GNDA #PWR76
U 1 1 57C91B78
P 9600 10225
F 0 "#PWR76" H 9600 9975 50  0001 C CNN
F 1 "GNDA" H 9600 10075 50  0000 C CNN
F 2 "" H 9600 10225 50  0000 C CNN
F 3 "" H 9600 10225 50  0000 C CNN
	1    9600 10225
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR144
U 1 1 57C99B79
P 15225 7900
F 0 "#PWR144" H 15225 7650 50  0001 C CNN
F 1 "GND" H 15225 7750 50  0000 C CNN
F 2 "" H 15225 7900 50  0000 C CNN
F 3 "" H 15225 7900 50  0000 C CNN
	1    15225 7900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR128
U 1 1 57C99B85
P 14725 7900
F 0 "#PWR128" H 14725 7650 50  0001 C CNN
F 1 "GND" H 14725 7750 50  0000 C CNN
F 2 "" H 14725 7900 50  0000 C CNN
F 3 "" H 14725 7900 50  0000 C CNN
	1    14725 7900
	1    0    0    -1  
$EndComp
$Comp
L CONN_02X02 P29
U 1 1 57C99B8C
P 14975 7850
F 0 "P29" H 14975 8000 50  0000 C CNN
F 1 "5V_Servo" H 14975 7700 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x02" H 14975 6650 50  0001 C CNN
F 3 "" H 14975 6650 50  0000 C CNN
F 4 "2x2 Header" H 14975 7850 60  0001 C CNN "Package"
	1    14975 7850
	1    0    0    -1  
$EndComp
$Comp
L SERVO_+5V #PWR127
U 1 1 57C9A988
P 14725 7800
F 0 "#PWR127" H 14725 7650 50  0001 C CNN
F 1 "SERVO_+5V" H 14725 7940 50  0000 C CNN
F 2 "" H 14725 7800 50  0000 C CNN
F 3 "" H 14725 7800 50  0000 C CNN
	1    14725 7800
	1    0    0    -1  
$EndComp
$Comp
L SERVO_+5V #PWR143
U 1 1 57C9ABAD
P 15225 7800
F 0 "#PWR143" H 15225 7650 50  0001 C CNN
F 1 "SERVO_+5V" H 15225 7940 50  0000 C CNN
F 2 "" H 15225 7800 50  0000 C CNN
F 3 "" H 15225 7800 50  0000 C CNN
	1    15225 7800
	1    0    0    -1  
$EndComp
Text Label 5025 7700 2    60   ~ 0
PPC_+V_PWR
Text Label 5025 7800 2    60   ~ 0
PPC_-V_PWR
$Comp
L LD1117_S33TR U6
U 1 1 57CA41C5
P 8700 2175
F 0 "U6" H 8700 2425 60  0000 C CNN
F 1 "LD1117_S33TR" H 8700 2225 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-223" H 8700 2225 60  0001 C CNN
F 3 "http://www.farnell.com/datasheets/1776449.pdf" H 8700 2225 60  0001 C CNN
F 4 "Farnell" H 8700 2175 60  0001 C CNN "Supply"
F 5 "1202826" H 8700 2175 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/stmicroelectronics/ld1117s33tr/ldo-spann-regler-3-3v-smd-1117/dp/1202826" H 8700 2175 60  0001 C CNN "URL"
F 7 "\"0,297\"" H 8700 2175 60  0001 C CNN "Price"
F 8 "SOT-223" H 8700 2175 60  0001 C CNN "Package"
	1    8700 2175
	1    0    0    -1  
$EndComp
$Comp
L C_Small C50
U 1 1 57CEF93C
P 9475 9600
F 0 "C50" H 9485 9670 50  0000 L CNN
F 1 "n.B." H 9485 9520 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 9475 9600 50  0001 C CNN
F 3 "" H 9475 9600 50  0000 C CNN
	1    9475 9600
	0    -1   -1   0   
$EndComp
$Comp
L R_Small R63
U 1 1 57CF4C5D
P 14100 975
F 0 "R63" H 14130 995 50  0000 L CNN
F 1 "100R" H 14130 935 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 14100 975 50  0001 C CNN
F 3 "" H 14100 975 50  0000 C CNN
	1    14100 975 
	0    -1   -1   0   
$EndComp
$Comp
L R_Small R64
U 1 1 57CF50BF
P 14100 1225
F 0 "R64" H 14130 1245 50  0000 L CNN
F 1 "100R" H 14130 1185 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 14100 1225 50  0001 C CNN
F 3 "" H 14100 1225 50  0000 C CNN
	1    14100 1225
	0    -1   -1   0   
$EndComp
$Comp
L R_Small R62
U 1 1 57CF9903
P 10450 1500
F 0 "R62" H 10480 1520 50  0000 L CNN
F 1 "0R" H 10480 1460 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 10450 1500 50  0001 C CNN
F 3 "" H 10450 1500 50  0000 C CNN
	1    10450 1500
	0    -1   1    0   
$EndComp
$Comp
L R_Small R26
U 1 1 57CFBF70
P 7475 9400
F 0 "R26" H 7505 9420 50  0000 L CNN
F 1 "n.B." H 7505 9360 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 7475 9400 50  0001 C CNN
F 3 "" H 7475 9400 50  0000 C CNN
	1    7475 9400
	0    -1   -1   0   
$EndComp
$Comp
L R_Small R36
U 1 1 57CFFCBF
P 9475 9400
F 0 "R36" H 9505 9420 50  0000 L CNN
F 1 "n.B." H 9505 9360 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 9475 9400 50  0001 C CNN
F 3 "" H 9475 9400 50  0000 C CNN
	1    9475 9400
	0    -1   -1   0   
$EndComp
$Comp
L C_Small C49
U 1 1 57CEF439
P 7475 9625
F 0 "C49" H 7485 9695 50  0000 L CNN
F 1 "n.B." H 7485 9545 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 7475 9625 50  0001 C CNN
F 3 "" H 7475 9625 50  0000 C CNN
	1    7475 9625
	0    -1   -1   0   
$EndComp
NoConn ~ 5700 10325
Text Label 3750 9725 0    60   ~ 0
MPU_INT
Text Label 12675 7625 0    60   ~ 0
MPU_INT
Text Label 13975 7425 0    60   ~ 0
MPU_INT
$Comp
L GND #PWR29
U 1 1 57CF0AFF
P 6125 8950
F 0 "#PWR29" H 6125 8700 50  0001 C CNN
F 1 "GND" H 6125 8800 50  0000 C CNN
F 2 "" H 6125 8950 50  0000 C CNN
F 3 "" H 6125 8950 50  0000 C CNN
	1    6125 8950
	-1   0    0    1   
$EndComp
$Comp
L PWR_FLAG #FLG6
U 1 1 57CF1641
P 6175 975
F 0 "#FLG6" H 6175 1070 50  0001 C CNN
F 1 "PWR_FLAG" H 6175 1155 50  0000 C CNN
F 2 "" H 6175 975 50  0000 C CNN
F 3 "" H 6175 975 50  0000 C CNN
	1    6175 975 
	1    0    0    -1  
$EndComp
$Comp
L R_Small R53
U 1 1 57CF9BEE
P 12550 7625
F 0 "R53" H 12580 7645 50  0000 L CNN
F 1 "0R" H 12580 7585 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 12550 7625 50  0001 C CNN
F 3 "" H 12550 7625 50  0000 C CNN
	1    12550 7625
	0    1    1    0   
$EndComp
$Comp
L R_Small R65
U 1 1 57CFAF0E
P 13850 7425
F 0 "R65" H 13880 7445 50  0000 L CNN
F 1 "n.B." H 13880 7385 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 13850 7425 50  0001 C CNN
F 3 "" H 13850 7425 50  0000 C CNN
	1    13850 7425
	0    -1   -1   0   
$EndComp
Text Label 8725 4425 0    60   ~ 0
uC_VDDA
Text Label 8825 7450 0    60   ~ 0
uC_VDDA
$Comp
L R_Small R66
U 1 1 57D03C54
P 6450 5100
F 0 "R66" H 6480 5120 50  0000 L CNN
F 1 "n.B." H 6480 5060 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 6450 5100 50  0001 C CNN
F 3 "" H 6450 5100 50  0000 C CNN
	1    6450 5100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8450 5325 9175 5325
Wire Wire Line
	8450 4925 8450 5325
Wire Wire Line
	8525 4925 8450 4925
Wire Wire Line
	11950 9450 11975 9450
Wire Wire Line
	11700 9450 11750 9450
Wire Wire Line
	11950 8900 11975 8900
Wire Wire Line
	11700 8900 11750 8900
Wire Wire Line
	11950 9175 11975 9175
Wire Wire Line
	11700 9175 11750 9175
Wire Notes Line
	12625 9950 11450 9950
Wire Notes Line
	12625 8675 12625 9950
Wire Notes Line
	11450 8675 12625 8675
Wire Notes Line
	11450 9950 11450 8675
Wire Wire Line
	7250 6375 7350 6375
Wire Wire Line
	7350 6925 7250 6925
Wire Wire Line
	7350 7375 7350 6925
Wire Wire Line
	5650 7375 5700 7375
Wire Wire Line
	5700 7375 5900 7375
Wire Wire Line
	5900 7375 6275 7375
Wire Wire Line
	6275 7375 7350 7375
Wire Wire Line
	14075 4175 14075 4425
Wire Wire Line
	12675 4175 14075 4175
Wire Wire Line
	13500 4525 13575 4525
Wire Wire Line
	13500 4275 13500 4525
Wire Wire Line
	12675 4275 13500 4275
Wire Wire Line
	13500 4725 13575 4725
Wire Wire Line
	13500 5000 13500 4725
Wire Wire Line
	14150 5000 13500 5000
Wire Wire Line
	14150 4725 14150 5000
Wire Wire Line
	14075 4725 14150 4725
Wire Wire Line
	14200 4625 14075 4625
Wire Wire Line
	13325 4425 13575 4425
Wire Wire Line
	11450 5050 13325 5050
Wire Wire Line
	13325 5050 14200 5050
Wire Wire Line
	11450 4575 11450 5050
Wire Wire Line
	11575 4575 11450 4575
Wire Wire Line
	5550 8725 4900 8725
Wire Wire Line
	4425 10100 4575 10100
Wire Wire Line
	4200 10100 4225 10100
Connection ~ 4575 10100
Connection ~ 4575 9850
Wire Wire Line
	4575 9800 4575 9850
Wire Wire Line
	4575 9850 4575 9875
Wire Wire Line
	4575 10075 4575 10100
Wire Wire Line
	4575 10100 4575 10150
Wire Wire Line
	4200 10025 4200 10100
Wire Wire Line
	3750 10025 4200 10025
Wire Wire Line
	4200 9925 3750 9925
Wire Wire Line
	4200 9850 4200 9925
Wire Wire Line
	4575 9850 4200 9850
Wire Notes Line
	12900 5325 12900 6200
Wire Notes Line
	15475 6575 14500 6575
Wire Notes Line
	15450 8675 15475 6575
Wire Notes Line
	14475 8675 15450 8675
Wire Notes Line
	14500 6575 14475 8675
Wire Notes Line
	13950 8750 12675 8750
Wire Notes Line
	13950 9775 13950 8750
Wire Notes Line
	12675 9775 13950 9775
Wire Notes Line
	12675 8750 12675 9775
Wire Notes Line
	7225 10800 7225 8650
Wire Notes Line
	11325 10800 7225 10800
Wire Notes Line
	11325 8650 11325 10800
Wire Notes Line
	7225 8650 11325 8650
Wire Notes Line
	15975 8750 14000 8750
Wire Notes Line
	15975 9900 15975 8750
Wire Notes Line
	14000 9900 15975 9900
Wire Notes Line
	14000 8750 14000 9900
Connection ~ 1075 3025
Wire Wire Line
	8175 7625 7950 7625
Wire Wire Line
	8175 7525 8175 7625
Wire Wire Line
	13900 7125 13700 7125
Wire Wire Line
	13700 8075 13750 8075
Connection ~ 8075 1025
Wire Wire Line
	8075 1025 8075 1175
Wire Wire Line
	8075 1375 8075 1575
Wire Wire Line
	7950 7525 8000 7525
Wire Wire Line
	13900 6100 13900 6100
Wire Wire Line
	15475 4650 15475 4650
Connection ~ 1525 3975
Wire Wire Line
	1500 2125 1550 2125
Wire Wire Line
	1075 2625 1075 2700
Wire Wire Line
	775  2225 1500 2225
Wire Wire Line
	1500 2225 1550 2225
Wire Wire Line
	1075 2625 1275 2625
Wire Wire Line
	1275 2625 1425 2625
Wire Wire Line
	1425 2625 1500 2625
Wire Wire Line
	1500 2625 1550 2625
Wire Wire Line
	1400 3275 1450 3275
Wire Wire Line
	1450 3275 1525 3275
Wire Wire Line
	1525 3275 1550 3275
Connection ~ 1525 3375
Connection ~ 1525 3475
Wire Wire Line
	1525 3375 1550 3375
Connection ~ 1525 3575
Wire Wire Line
	1525 3475 1550 3475
Wire Wire Line
	1525 3575 1525 3475
Wire Wire Line
	1525 3475 1525 3375
Wire Wire Line
	1525 3375 1525 3275
Connection ~ 1500 575 
Wire Wire Line
	1500 575  1500 775 
Wire Wire Line
	1325 575  1500 575 
Wire Wire Line
	1500 575  1600 575 
Wire Wire Line
	1325 600  1325 575 
Connection ~ 1500 1225
Wire Wire Line
	1500 1225 1500 975 
Connection ~ 1325 1225
Wire Wire Line
	1325 1200 1325 1225
Wire Wire Line
	575  2875 575  3025
Wire Wire Line
	575  3025 575  3125
Wire Wire Line
	1075 3025 1075 2900
Connection ~ 775  3025
Wire Wire Line
	775  3025 775  2875
Connection ~ 775  2625
Wire Wire Line
	775  2225 775  2625
Wire Wire Line
	775  2625 775  2675
Wire Wire Line
	575  2625 575  2675
Wire Wire Line
	575  2625 775  2625
Connection ~ 725  3975
Wire Wire Line
	725  3850 725  3975
Connection ~ 950  3975
Wire Wire Line
	950  3975 950  3850
Connection ~ 1175 3975
Wire Wire Line
	1175 3975 1175 3850
Connection ~ 1400 3975
Wire Wire Line
	1400 3975 1400 3850
Wire Wire Line
	550  3850 550  3975
Wire Wire Line
	550  3975 550  4075
Connection ~ 1175 3575
Wire Wire Line
	1175 3575 1175 3650
Connection ~ 950  3575
Wire Wire Line
	950  3575 950  3650
Connection ~ 725  3575
Wire Wire Line
	725  3650 725  3575
Connection ~ 1400 3575
Wire Wire Line
	550  3575 550  3650
Wire Wire Line
	1400 3575 1400 3650
Connection ~ 1525 4075
Wire Wire Line
	1525 4075 1550 4075
Connection ~ 1525 4175
Wire Wire Line
	1525 4175 1550 4175
Wire Wire Line
	1525 4275 1550 4275
Wire Wire Line
	1525 3975 1525 4075
Wire Wire Line
	1525 4075 1525 4175
Wire Wire Line
	1525 4175 1525 4275
Wire Wire Line
	550  3975 725  3975
Wire Wire Line
	725  3975 950  3975
Wire Wire Line
	950  3975 1175 3975
Wire Wire Line
	1175 3975 1400 3975
Wire Wire Line
	1400 3975 1525 3975
Wire Wire Line
	1525 3975 1550 3975
Wire Wire Line
	550  3575 725  3575
Wire Wire Line
	725  3575 950  3575
Wire Wire Line
	950  3575 1175 3575
Wire Wire Line
	1175 3575 1400 3575
Wire Wire Line
	1400 3575 1525 3575
Wire Wire Line
	1525 3575 1550 3575
Connection ~ 1275 3025
Wire Wire Line
	1275 3025 1275 2900
Wire Wire Line
	575  3025 775  3025
Wire Wire Line
	775  3025 1075 3025
Wire Wire Line
	1075 3025 1275 3025
Wire Wire Line
	1275 3025 1550 3025
Connection ~ 1275 2625
Wire Wire Line
	1275 2625 1275 2700
Connection ~ 11350 6475
Wire Wire Line
	11350 6475 11250 6475
Connection ~ 10150 6475
Wire Wire Line
	10150 6475 10200 6475
Connection ~ 10150 6375
Wire Wire Line
	10200 6375 10150 6375
Wire Wire Line
	10150 6075 10150 6375
Wire Wire Line
	10150 6375 10150 6475
Wire Wire Line
	10150 6475 10150 6550
Wire Wire Line
	10200 6075 10150 6075
Wire Wire Line
	10400 6075 10750 6075
Wire Wire Line
	10750 6375 10400 6375
Wire Wire Line
	10400 6475 10750 6475
Wire Wire Line
	11350 5575 11350 5525
Wire Wire Line
	11250 5575 11350 5575
Wire Wire Line
	10650 5575 10750 5575
Wire Wire Line
	10650 5525 10650 5575
Wire Wire Line
	1250 1325 1500 1325
Wire Wire Line
	1500 1325 1550 1325
Wire Wire Line
	1250 1225 1325 1225
Wire Wire Line
	1325 1225 1500 1225
Wire Wire Line
	1500 1225 1550 1225
Connection ~ 10700 1500
Wire Wire Line
	10700 1275 10700 1500
Wire Wire Line
	10700 1500 10700 1600
Wire Wire Line
	10250 1475 10250 1500
Connection ~ 11475 2775
Wire Wire Line
	11475 2775 11475 2625
Connection ~ 11475 2275
Wire Wire Line
	11475 2275 11475 2425
Wire Wire Line
	11525 1675 11525 1775
Connection ~ 11525 1375
Wire Wire Line
	11525 1375 12750 1375
Wire Wire Line
	11525 1300 11525 1375
Wire Wire Line
	11525 1375 11525 1475
Wire Wire Line
	10650 1275 10700 1275
Wire Wire Line
	11750 1125 11950 1125
Wire Wire Line
	11950 1125 12750 1125
Wire Wire Line
	6675 9175 6675 9175
Connection ~ 11950 1125
Wire Wire Line
	15075 8975 15075 9025
Wire Wire Line
	15075 9625 15075 9675
Wire Notes Line
	7575 5750 4400 5750
Wire Notes Line
	7575 8075 7575 5750
Wire Notes Line
	4400 8075 7575 8075
Wire Notes Line
	4400 5750 4400 8075
Wire Notes Line
	10000 5325 10000 6775
Wire Notes Line
	11750 5325 10000 5325
Wire Notes Line
	11750 6775 11750 5325
Wire Notes Line
	10000 6775 11750 6775
Connection ~ 11350 6375
Wire Wire Line
	11350 6375 11250 6375
Connection ~ 11350 6275
Wire Wire Line
	11350 6275 11250 6275
Connection ~ 11350 6175
Wire Wire Line
	11350 6175 11250 6175
Connection ~ 11350 6075
Wire Wire Line
	11350 6075 11250 6075
Connection ~ 11350 5975
Wire Wire Line
	11350 5975 11250 5975
Connection ~ 11350 5875
Wire Wire Line
	11350 5875 11250 5875
Connection ~ 11350 5775
Wire Wire Line
	11250 5775 11350 5775
Wire Wire Line
	11350 5675 11350 5775
Wire Wire Line
	11350 5775 11350 5875
Wire Wire Line
	11350 5875 11350 5975
Wire Wire Line
	11350 5975 11350 6075
Wire Wire Line
	11350 6075 11350 6175
Wire Wire Line
	11350 6175 11350 6275
Wire Wire Line
	11350 6275 11350 6375
Wire Wire Line
	11350 6375 11350 6475
Wire Wire Line
	11350 6475 11350 6550
Wire Wire Line
	11250 5675 11350 5675
Wire Notes Line
	11825 5325 12900 5325
Wire Notes Line
	12900 6200 11825 6200
Wire Notes Line
	11825 6200 11825 5325
Wire Notes Line
	15900 3650 14500 3650
Wire Notes Line
	15900 4250 15900 3650
Wire Notes Line
	14500 4250 15900 4250
Wire Notes Line
	14500 3650 14500 4250
Wire Notes Line
	5875 9425 5875 8750
Wire Notes Line
	7150 9425 5875 9425
Wire Notes Line
	7150 8750 7150 9425
Wire Notes Line
	5875 8750 7150 8750
Wire Wire Line
	6675 8975 6675 8975
Wire Wire Line
	6675 9075 6675 9075
Wire Wire Line
	6675 9275 6675 9275
Connection ~ 6125 9175
Wire Wire Line
	6125 9275 6175 9275
Connection ~ 6125 9075
Wire Wire Line
	6125 9175 6175 9175
Connection ~ 6125 8975
Wire Wire Line
	6125 9075 6175 9075
Wire Wire Line
	6125 8975 6175 8975
Wire Notes Line
	16000 4400 14500 4400
Wire Notes Line
	16000 6350 16000 4400
Wire Notes Line
	14500 6350 16000 6350
Wire Notes Line
	14500 4400 14500 6350
Wire Notes Line
	12975 5325 12975 5400
Wire Notes Line
	14350 5325 12975 5325
Wire Notes Line
	14350 6600 14350 5325
Wire Notes Line
	12975 6600 14350 6600
Wire Notes Line
	12975 5375 12975 6600
Wire Notes Line
	7600 7300 7600 7425
Wire Notes Line
	9975 7300 7600 7300
Wire Notes Line
	9975 8375 9975 7300
Wire Notes Line
	7600 8375 9975 8375
Wire Notes Line
	7600 7350 7600 8375
Wire Notes Line
	13300 7850 13300 7925
Wire Notes Line
	14350 7850 13300 7850
Wire Notes Line
	14350 8600 14350 7850
Wire Notes Line
	13300 8600 14350 8600
Wire Notes Line
	13300 7900 13300 8600
Wire Notes Line
	5800 9425 4500 9425
Wire Notes Line
	5800 8375 5800 9425
Wire Notes Line
	4500 8375 5800 8375
Wire Notes Line
	4500 9425 4500 8375
Wire Notes Line
	7650 7200 9300 7200
Wire Notes Line
	7650 5750 7650 7200
Wire Notes Line
	9300 5750 7650 5750
Wire Notes Line
	9300 7200 9300 5750
Wire Wire Line
	5100 8825 4900 8825
Wire Wire Line
	4900 9250 5100 9250
Wire Wire Line
	13725 8475 13700 8475
Wire Notes Line
	14350 6775 13300 6775
Wire Notes Line
	14350 7725 14350 6775
Wire Notes Line
	13300 7725 14350 7725
Wire Notes Line
	13300 6775 13300 7725
Wire Wire Line
	13700 7225 13700 7225
Wire Wire Line
	13700 7325 13700 7325
Wire Wire Line
	13700 7425 13750 7425
Wire Wire Line
	13700 7525 13700 7525
Wire Wire Line
	13700 7625 13700 7625
Wire Wire Line
	13900 7025 13900 7125
Wire Wire Line
	13700 7025 13725 7025
Wire Notes Line
	13225 7050 10000 7050
Wire Notes Line
	13225 8600 13225 7050
Wire Notes Line
	10000 8600 13225 8600
Wire Notes Line
	10000 7050 10000 8600
Wire Wire Line
	12425 8025 12675 8025
Wire Wire Line
	12425 7925 12675 7925
Wire Wire Line
	12425 7825 12675 7825
Wire Wire Line
	12425 7425 12675 7425
Wire Wire Line
	12425 7525 12450 7525
Connection ~ 10250 7425
Connection ~ 10575 7525
Wire Wire Line
	10875 8300 10875 8350
Connection ~ 10250 7825
Connection ~ 10925 7425
Wire Wire Line
	10250 7300 10250 7425
Wire Wire Line
	10250 7425 10250 7575
Connection ~ 10575 7825
Wire Wire Line
	10250 7775 10250 7825
Wire Wire Line
	10250 7825 10250 8350
Wire Wire Line
	10250 7425 10925 7425
Wire Wire Line
	10925 7425 11025 7425
Wire Wire Line
	10925 7325 10925 7425
Wire Wire Line
	11025 7325 10925 7325
Wire Wire Line
	10575 7300 10575 7525
Wire Wire Line
	10575 7525 10575 7575
Wire Wire Line
	11025 7525 10575 7525
Connection ~ 10925 7825
Wire Wire Line
	10575 7825 10575 7775
Wire Wire Line
	10250 7825 10575 7825
Wire Wire Line
	10575 7825 10925 7825
Wire Wire Line
	10925 7825 11025 7825
Wire Wire Line
	10925 7725 10925 7825
Wire Wire Line
	11025 7725 10925 7725
Wire Wire Line
	10875 8025 11025 8025
Wire Wire Line
	10875 8100 10875 8025
Wire Wire Line
	4950 3025 4950 3450
Wire Wire Line
	4950 3450 4950 3575
Wire Wire Line
	4950 3575 4950 3625
Wire Wire Line
	5050 3025 4950 3025
Wire Wire Line
	6675 3025 6675 3450
Wire Wire Line
	6675 3450 6675 3575
Wire Wire Line
	6675 3575 6675 3625
Wire Wire Line
	6775 3025 6675 3025
Wire Wire Line
	8025 2025 8025 1975
Wire Wire Line
	9700 2025 9700 1975
Wire Wire Line
	9700 1025 9700 975 
Wire Notes Line
	9925 750  4350 750 
Wire Notes Line
	9925 2800 9925 750 
Wire Notes Line
	4350 2800 9925 2800
Wire Notes Line
	4350 750  4350 2800
Connection ~ 8800 1025
Wire Wire Line
	8800 1025 8800 1175
Connection ~ 8425 1025
Wire Wire Line
	8425 1025 8425 1175
Wire Wire Line
	8800 1375 8800 1575
Wire Wire Line
	8425 1575 8425 1375
Wire Wire Line
	9550 2575 9550 2375
Wire Wire Line
	7825 2575 7825 2375
Wire Wire Line
	5275 1375 5275 1575
Wire Wire Line
	6025 1575 6025 1375
Wire Wire Line
	5925 1025 6025 1025
Wire Wire Line
	6025 1025 6175 1025
Wire Wire Line
	6175 1025 6350 1025
Wire Wire Line
	6350 1025 6425 1025
Wire Wire Line
	4825 1025 5025 1025
Wire Wire Line
	5025 1025 5275 1025
Wire Wire Line
	5275 1025 5425 1025
Wire Wire Line
	5425 1025 5650 1025
Wire Wire Line
	5650 1025 5725 1025
Connection ~ 6025 1025
Wire Wire Line
	6025 1175 6025 1025
Connection ~ 6350 1025
Wire Wire Line
	6350 2025 6425 2025
Wire Wire Line
	6350 1025 6350 2025
Wire Wire Line
	9300 2025 9375 2025
Wire Wire Line
	9375 2025 9550 2025
Wire Wire Line
	9550 2025 9700 2025
Connection ~ 8025 2025
Wire Wire Line
	7625 2025 7825 2025
Wire Wire Line
	7825 2025 8025 2025
Wire Wire Line
	8025 2025 8100 2025
Wire Wire Line
	5775 2550 5925 2550
Wire Wire Line
	5925 2550 6050 2550
Wire Wire Line
	6050 2550 6150 2550
Connection ~ 5650 1025
Wire Wire Line
	7625 1025 8075 1025
Wire Wire Line
	8075 1025 8425 1025
Wire Wire Line
	8425 1025 8800 1025
Wire Wire Line
	8800 1025 9700 1025
Wire Wire Line
	5650 1375 5650 1575
Wire Wire Line
	5650 1025 5650 1175
Wire Notes Line
	9925 2900 4350 2900
Wire Notes Line
	9925 5600 9925 2900
Wire Notes Line
	4350 5600 9925 5600
Wire Notes Line
	4350 2900 4350 5600
Wire Wire Line
	7350 4100 7400 4100
Wire Wire Line
	7400 4100 7450 4100
Wire Wire Line
	5650 4100 5700 4100
Wire Wire Line
	5700 4100 5750 4100
Wire Notes Line
	14350 5225 14350 3650
Wire Notes Line
	10000 5225 14350 5225
Wire Notes Line
	10000 3650 10000 5225
Wire Notes Line
	14350 3650 10000 3650
Wire Wire Line
	12675 4075 13200 4075
Wire Wire Line
	13200 4075 13250 4075
Wire Wire Line
	12675 3975 12775 3975
Wire Wire Line
	12775 3975 12950 3975
Wire Wire Line
	11000 4700 11000 4750
Wire Wire Line
	11000 4475 11000 4500
Wire Wire Line
	11575 4475 11000 4475
Wire Wire Line
	10950 4375 11575 4375
Wire Wire Line
	10950 4525 10950 4375
Wire Wire Line
	10725 4525 10950 4525
Wire Wire Line
	10725 4500 10725 4525
Wire Wire Line
	10725 4275 10725 4300
Wire Wire Line
	11575 4275 10725 4275
Wire Wire Line
	10575 4075 10625 4075
Wire Wire Line
	10575 4125 10575 4075
Wire Wire Line
	10675 4175 11575 4175
Wire Wire Line
	10675 4325 10675 4175
Wire Wire Line
	10125 4325 10675 4325
Wire Wire Line
	10125 4225 10125 4325
Wire Wire Line
	10125 3975 10125 4025
Wire Wire Line
	11575 3975 10125 3975
Wire Wire Line
	10825 4075 11575 4075
Wire Notes Line
	15400 750  10000 750 
Wire Notes Line
	15400 3525 15400 750 
Wire Notes Line
	10000 3525 15400 3525
Wire Notes Line
	10000 750  10000 3525
Wire Wire Line
	11525 975  11525 1100
Wire Wire Line
	14850 2025 14900 2025
Connection ~ 11075 2275
Wire Wire Line
	11750 2275 11750 1125
Wire Wire Line
	11075 2275 11075 2425
Wire Wire Line
	10675 2275 11075 2275
Wire Wire Line
	11075 2275 11475 2275
Wire Wire Line
	11475 2275 11750 2275
Wire Wire Line
	10675 2425 10675 2275
Connection ~ 11075 2775
Wire Wire Line
	11075 2775 11075 2625
Wire Wire Line
	10675 2625 10675 2775
Wire Wire Line
	12750 1225 11950 1225
Connection ~ 11950 2275
Wire Wire Line
	11950 2275 12750 2275
Wire Wire Line
	11950 1225 11950 2275
Wire Wire Line
	11950 2275 11950 2425
Wire Wire Line
	11950 1075 11950 1125
Wire Wire Line
	10675 2775 11075 2775
Wire Wire Line
	11075 2775 11475 2775
Wire Wire Line
	11475 2775 11950 2775
Wire Wire Line
	11950 2775 12050 2775
Wire Wire Line
	12050 2775 12750 2775
Wire Wire Line
	10650 1175 10950 1175
Wire Wire Line
	10650 1075 10950 1075
Wire Wire Line
	10650 975  11525 975 
Connection ~ 14000 2725
Wire Wire Line
	13850 2725 14000 2725
Connection ~ 14000 2825
Wire Wire Line
	14000 2825 13850 2825
Connection ~ 14000 2925
Wire Wire Line
	14000 2925 13850 2925
Wire Wire Line
	14000 2575 14000 2725
Wire Wire Line
	14000 2725 14000 2825
Wire Wire Line
	14000 2825 14000 2925
Wire Wire Line
	14000 2925 14000 3175
Wire Wire Line
	13850 2575 14000 2575
Wire Wire Line
	14200 2025 14450 2025
Wire Wire Line
	13850 2025 14000 2025
Wire Wire Line
	14200 1525 14450 1525
Wire Wire Line
	13850 1525 14000 1525
Wire Wire Line
	14200 1325 14450 1325
Wire Wire Line
	13850 1325 14000 1325
Wire Wire Line
	12475 9450 12375 9450
Wire Wire Line
	12475 8900 12475 9175
Wire Wire Line
	12475 9175 12475 9450
Wire Wire Line
	12475 9450 12475 9725
Wire Wire Line
	12475 9725 12475 9750
Wire Wire Line
	12475 9175 12375 9175
Connection ~ 12475 9450
Wire Wire Line
	12475 8900 12375 8900
Connection ~ 12475 9175
Wire Wire Line
	12775 3900 12775 3975
Connection ~ 12775 3975
Wire Wire Line
	4725 1425 4825 1425
Wire Wire Line
	4825 1425 4825 1350
Wire Wire Line
	4950 1575 5025 1575
Wire Wire Line
	5025 1575 5025 1350
Wire Wire Line
	4825 1150 4825 1025
Wire Wire Line
	5025 1150 5025 1025
Connection ~ 5025 1025
Wire Wire Line
	6675 5075 6675 5100
Wire Wire Line
	6675 5100 6675 5350
Wire Wire Line
	6675 5350 6675 5425
Wire Wire Line
	13200 4075 13200 3975
Wire Wire Line
	13200 3975 13150 3975
Connection ~ 13200 4075
Wire Wire Line
	5700 4375 5700 4375
Wire Wire Line
	7400 4100 7400 4150
Connection ~ 7400 4100
Wire Wire Line
	7400 4375 7400 4350
Wire Wire Line
	4900 8625 4950 8625
Wire Wire Line
	5150 8625 5200 8625
Wire Wire Line
	4950 9150 4900 9150
Wire Wire Line
	5150 9150 5200 9150
Wire Wire Line
	6100 5350 6050 5350
Wire Wire Line
	8050 5125 8050 5000
Wire Wire Line
	8050 4800 8050 4725
Wire Wire Line
	8000 4725 8050 4725
Wire Wire Line
	8050 4725 8525 4725
Connection ~ 8050 4725
Wire Wire Line
	5950 6925 5900 6925
Wire Wire Line
	5900 6925 5900 7375
Connection ~ 5900 7375
Wire Wire Line
	6800 3575 6675 3575
Connection ~ 6675 3575
Wire Wire Line
	6800 3450 6675 3450
Connection ~ 6675 3450
Wire Wire Line
	7000 3575 7050 3575
Wire Wire Line
	7050 3450 7050 3575
Wire Wire Line
	7050 3575 7050 3825
Wire Wire Line
	7050 3825 7050 3900
Wire Wire Line
	7050 3450 7000 3450
Wire Wire Line
	7050 3825 6975 3825
Connection ~ 7050 3575
Wire Wire Line
	7050 4375 7050 4300
Connection ~ 7050 3825
Wire Wire Line
	5300 3575 5350 3575
Wire Wire Line
	5350 3450 5350 3575
Wire Wire Line
	5350 3575 5350 3825
Wire Wire Line
	5350 3825 5350 3900
Wire Wire Line
	5350 3450 5300 3450
Connection ~ 5350 3575
Wire Wire Line
	5100 3575 4950 3575
Connection ~ 4950 3575
Wire Wire Line
	5100 3450 4950 3450
Connection ~ 4950 3450
Wire Wire Line
	5350 4400 5350 4300
Wire Wire Line
	5700 4100 5700 4175
Connection ~ 5700 4100
Wire Wire Line
	5700 4400 5700 4375
Wire Wire Line
	5250 3825 5350 3825
Connection ~ 5350 3825
Wire Wire Line
	6675 4200 6675 4025
Wire Wire Line
	4950 4200 4950 4025
Wire Wire Line
	4950 4500 4950 4675
Connection ~ 575  3025
Connection ~ 550  3975
Wire Wire Line
	750  4800 750  4875
Wire Wire Line
	750  4800 875  4800
Wire Wire Line
	1075 4800 1125 4800
Wire Wire Line
	1125 4800 1250 4800
Wire Wire Line
	14200 5050 14200 4925
Wire Wire Line
	14200 4625 14200 4725
Wire Wire Line
	13325 5050 13325 4925
Connection ~ 13325 5050
Wire Wire Line
	13325 4725 13325 4425
Wire Wire Line
	8600 8325 9275 8325
Wire Wire Line
	8600 7925 8600 8325
Wire Wire Line
	8625 7925 8600 7925
Wire Wire Line
	9225 7825 9275 7825
Wire Wire Line
	9275 7825 9325 7825
Wire Wire Line
	8425 7725 8500 7725
Wire Wire Line
	8500 7725 8625 7725
Wire Wire Line
	7950 7725 8150 7725
Wire Wire Line
	8150 7725 8225 7725
Wire Wire Line
	8800 9825 8750 9825
Wire Wire Line
	8375 9825 8475 9825
Wire Wire Line
	8475 9825 8550 9825
Wire Wire Line
	7725 9925 7775 9925
Wire Wire Line
	7550 9275 7725 9275
Wire Wire Line
	7725 9275 7775 9275
Wire Wire Line
	7775 9275 7825 9275
Wire Wire Line
	7550 9225 7550 9275
Wire Wire Line
	7350 9225 7350 9400
Wire Wire Line
	7350 9400 7350 9625
Wire Wire Line
	7350 9625 7350 9725
Wire Wire Line
	7350 9725 7350 10400
Wire Wire Line
	7350 9025 7350 8975
Wire Wire Line
	7350 8975 7450 8975
Wire Wire Line
	7450 8975 7550 8975
Wire Wire Line
	7550 8975 7550 9025
Wire Wire Line
	7725 9275 7725 9625
Wire Wire Line
	7725 9625 7725 9925
Wire Wire Line
	7725 9925 7725 10400
Connection ~ 7725 9275
Connection ~ 7725 9925
Wire Wire Line
	7775 9275 7775 9025
Wire Wire Line
	7775 9025 7825 9025
Connection ~ 7775 9275
Wire Wire Line
	8025 9275 8125 9275
Wire Wire Line
	8125 9275 8475 9275
Wire Wire Line
	8125 9275 8125 9025
Wire Wire Line
	8125 9025 8025 9025
Connection ~ 8125 9275
Wire Wire Line
	10800 9825 10750 9825
Wire Wire Line
	10375 9825 10475 9825
Wire Wire Line
	10475 9825 10550 9825
Wire Wire Line
	9725 9925 9775 9925
Wire Wire Line
	9550 9275 9725 9275
Wire Wire Line
	9725 9275 9775 9275
Wire Wire Line
	9775 9275 9825 9275
Wire Wire Line
	9550 9225 9550 9275
Wire Wire Line
	9350 9225 9350 9400
Wire Wire Line
	9350 9400 9350 9600
Wire Wire Line
	9350 9600 9350 9725
Wire Wire Line
	9350 9725 9350 10400
Wire Wire Line
	9350 9025 9350 8975
Wire Wire Line
	9350 8975 9450 8975
Wire Wire Line
	9450 8975 9550 8975
Wire Wire Line
	9550 8975 9550 9025
Wire Wire Line
	9725 9275 9725 9600
Wire Wire Line
	9725 9600 9725 9925
Wire Wire Line
	9725 9925 9725 10400
Connection ~ 9725 9275
Connection ~ 9725 9925
Wire Wire Line
	9775 9275 9775 9025
Wire Wire Line
	9775 9025 9825 9025
Connection ~ 9775 9275
Wire Wire Line
	10025 9275 10125 9275
Wire Wire Line
	10125 9275 10475 9275
Wire Wire Line
	10125 9275 10125 9025
Wire Wire Line
	10125 9025 10025 9025
Connection ~ 10125 9275
Wire Wire Line
	5925 2600 5925 2550
Connection ~ 5925 2550
Wire Wire Line
	6050 2500 6050 2550
Connection ~ 6050 2550
Wire Wire Line
	6350 2550 6425 2550
Wire Wire Line
	6425 2550 6500 2550
Wire Wire Line
	6425 2550 6425 2500
Wire Wire Line
	6500 2550 6500 2600
Connection ~ 6425 2550
Wire Wire Line
	7825 2175 7825 2025
Connection ~ 7825 2025
Wire Wire Line
	9550 2175 9550 2025
Connection ~ 9550 2025
Wire Wire Line
	5275 1175 5275 1025
Connection ~ 5275 1025
Wire Wire Line
	5425 975  5425 1025
Connection ~ 5425 1025
Wire Wire Line
	11950 2625 11950 2775
Connection ~ 11950 2775
Wire Wire Line
	12050 3225 12050 2775
Connection ~ 12050 2775
Wire Wire Line
	1250 4800 1250 4875
Wire Wire Line
	1125 4725 1125 4800
Connection ~ 1125 4800
Wire Wire Line
	4775 6725 4825 6725
Wire Wire Line
	4825 6725 4975 6725
Wire Wire Line
	4975 6725 5025 6725
Wire Wire Line
	4825 6725 4825 6775
Connection ~ 4825 6725
Wire Wire Line
	4825 7375 4825 6975
Wire Wire Line
	5375 6200 5325 6200
Wire Wire Line
	5325 6075 5325 6200
Wire Wire Line
	5325 6200 5325 6450
Wire Wire Line
	5325 6450 5325 6525
Wire Wire Line
	5325 6075 5375 6075
Wire Wire Line
	5325 6450 5400 6450
Connection ~ 5325 6200
Wire Wire Line
	5325 7000 5325 6925
Connection ~ 5325 6450
Wire Wire Line
	5950 5850 5950 6000
Wire Wire Line
	5950 6000 5950 6375
Wire Wire Line
	5950 5850 5650 5850
Wire Wire Line
	5700 6825 5950 6825
Wire Wire Line
	4975 6775 4975 6725
Connection ~ 4975 6725
Wire Wire Line
	5700 6200 5575 6200
Wire Wire Line
	5700 6050 5700 6075
Wire Wire Line
	5700 6075 5700 6200
Wire Wire Line
	5700 6200 5700 6250
Wire Wire Line
	5575 6075 5700 6075
Connection ~ 5700 6200
Connection ~ 5700 6075
Connection ~ 5700 6825
Wire Wire Line
	5700 7075 5700 7375
Connection ~ 5700 7375
Wire Wire Line
	5700 6650 5700 6825
Wire Wire Line
	5700 6825 5700 6875
Wire Wire Line
	6350 5350 6300 5350
Wire Wire Line
	6675 4500 6675 4675
Wire Wire Line
	6675 4675 6675 4725
Wire Wire Line
	6675 4725 6675 4775
Wire Wire Line
	7800 4725 6675 4725
Connection ~ 6675 4725
Wire Wire Line
	4950 4675 6675 4675
Connection ~ 6675 4675
Wire Wire Line
	6550 5350 6675 5350
Connection ~ 6675 5350
Wire Wire Line
	1425 2500 1425 2625
Connection ~ 1425 2625
Connection ~ 1525 3275
Wire Wire Line
	825  5375 850  5375
Wire Wire Line
	1450 3225 1450 3275
Connection ~ 1450 3275
Wire Wire Line
	1500 2625 1500 2225
Connection ~ 1500 2225
Connection ~ 1500 2625
Wire Wire Line
	9275 8325 9275 7825
Connection ~ 9275 7825
Wire Wire Line
	9450 4825 9475 4825
Wire Wire Line
	9125 4825 9175 4825
Wire Wire Line
	9175 4825 9250 4825
Wire Wire Line
	9175 5325 9175 4825
Connection ~ 9175 4825
Wire Wire Line
	9025 8850 9450 8850
Wire Wire Line
	9450 8850 9450 8975
Connection ~ 9450 8975
Wire Wire Line
	8525 8850 7450 8850
Wire Wire Line
	7450 8850 7450 8975
Connection ~ 7450 8975
Wire Wire Line
	5700 10225 5500 10225
Wire Wire Line
	5700 10125 5550 10125
Wire Wire Line
	5550 10125 5550 10000
Wire Wire Line
	5550 10000 5500 10000
Wire Wire Line
	5050 10000 5225 10000
Wire Wire Line
	5225 10000 5300 10000
Wire Wire Line
	5225 10000 5225 9950
Connection ~ 5225 10000
Wire Wire Line
	5225 10350 5225 10225
Wire Wire Line
	5050 10225 5225 10225
Wire Wire Line
	5225 10225 5300 10225
Connection ~ 5225 10225
Wire Wire Line
	6475 10325 6200 10325
Wire Wire Line
	6200 10225 6400 10225
Wire Wire Line
	6200 10125 6350 10125
Wire Wire Line
	6350 10125 6350 10000
Wire Wire Line
	6350 10000 6400 10000
Wire Wire Line
	6600 10000 6675 10000
Wire Wire Line
	6675 10000 6850 10000
Wire Wire Line
	6675 10000 6675 9950
Connection ~ 6675 10000
Wire Wire Line
	6825 10600 6475 10600
Wire Wire Line
	6475 10575 6475 10600
Wire Wire Line
	6475 10600 6475 10625
Connection ~ 6475 10600
Wire Wire Line
	6475 10375 6475 10325
Wire Wire Line
	6675 10350 6675 10225
Wire Wire Line
	6600 10225 6675 10225
Wire Wire Line
	6675 10225 6850 10225
Connection ~ 6675 10225
Wire Notes Line
	4800 9550 4800 11025
Wire Notes Line
	4800 11025 7125 11025
Wire Notes Line
	7125 11025 7125 9550
Wire Notes Line
	7125 9550 4800 9550
Wire Wire Line
	1050 5375 1075 5375
Wire Wire Line
	1075 5375 1100 5375
Wire Wire Line
	1300 5375 1325 5375
Wire Wire Line
	1325 5375 1350 5375
Wire Wire Line
	1075 5925 1075 5875
Wire Wire Line
	1075 5675 1075 5375
Connection ~ 1075 5375
Wire Wire Line
	6275 7450 6275 7375
Connection ~ 6275 7375
Wire Wire Line
	7325 7875 7325 7750
Wire Wire Line
	7325 7750 7225 7750
Wire Wire Line
	6725 7750 6625 7750
Wire Wire Line
	6625 7750 6625 7875
Wire Wire Line
	6625 7650 6725 7650
Wire Wire Line
	7325 7650 7225 7650
Wire Wire Line
	7350 9725 7375 9725
Connection ~ 7350 9725
Wire Wire Line
	7575 9725 7600 9725
Wire Wire Line
	7600 9725 7775 9725
Wire Wire Line
	7600 9725 7600 10025
Connection ~ 7600 9725
Wire Wire Line
	11950 9725 11975 9725
Wire Wire Line
	11700 9725 11750 9725
Wire Wire Line
	12475 9725 12375 9725
Connection ~ 12475 9725
Wire Wire Line
	9575 9725 9600 9725
Wire Wire Line
	9600 9725 9775 9725
Wire Wire Line
	9600 9725 9600 10025
Connection ~ 9600 9725
Wire Wire Line
	9375 9725 9350 9725
Connection ~ 9350 9725
Wire Wire Line
	9300 2225 9375 2225
Wire Wire Line
	9375 2225 9375 2025
Connection ~ 9375 2025
Wire Wire Line
	9375 9600 9350 9600
Connection ~ 9350 9600
Connection ~ 7725 9625
Wire Wire Line
	9575 9600 9725 9600
Connection ~ 9725 9600
Wire Wire Line
	13850 1125 13925 1125
Wire Wire Line
	13925 1125 13925 975 
Wire Wire Line
	13925 975  14000 975 
Wire Wire Line
	14200 975  14450 975 
Wire Wire Line
	14450 1225 14200 1225
Wire Wire Line
	14000 1225 13850 1225
Wire Wire Line
	10250 1500 10350 1500
Wire Wire Line
	10550 1500 10700 1500
Wire Wire Line
	7975 9525 7975 9400
Wire Wire Line
	7575 9400 7975 9400
Wire Wire Line
	7975 9400 8025 9400
Connection ~ 7975 9400
Wire Wire Line
	7375 9400 7350 9400
Connection ~ 7350 9400
Wire Wire Line
	9975 9525 9975 9400
Wire Wire Line
	9575 9400 9975 9400
Wire Wire Line
	9975 9400 10025 9400
Connection ~ 9975 9400
Wire Wire Line
	9375 9400 9350 9400
Connection ~ 9350 9400
Wire Wire Line
	7350 9625 7375 9625
Connection ~ 7350 9625
Wire Wire Line
	7575 9625 7725 9625
Wire Wire Line
	6125 8950 6125 8975
Wire Wire Line
	6125 8975 6125 9075
Wire Wire Line
	6125 9075 6125 9175
Wire Wire Line
	6125 9175 6125 9275
Wire Wire Line
	6175 975  6175 1025
Connection ~ 6175 1025
Wire Wire Line
	6050 6000 5950 6000
Connection ~ 5950 6000
Wire Wire Line
	12425 7625 12450 7625
Wire Wire Line
	12650 7625 12675 7625
Wire Wire Line
	13950 7425 13975 7425
Wire Wire Line
	6050 5100 6350 5100
Wire Wire Line
	6550 5100 6675 5100
Connection ~ 6675 5100
Text Label 8025 9400 0    60   ~ 0
uC_VDDA
Text Label 10025 9400 0    60   ~ 0
uC_VDDA
Text Label 5625 10425 2    60   ~ 0
uC_VDDA
Text Label 7950 8200 0    60   ~ 0
uC_VDDA
Wire Wire Line
	8475 9275 8475 9825
Connection ~ 8475 9825
Wire Wire Line
	10475 9275 10475 9825
Connection ~ 10475 9825
Wire Wire Line
	8825 7525 8825 7450
Wire Wire Line
	8725 4425 8725 4525
Wire Wire Line
	5625 10425 5700 10425
$Comp
L GND #PWR26
U 1 1 57D17E71
P 5925 1950
F 0 "#PWR26" H 5925 1700 50  0001 C CNN
F 1 "GND" H 5925 1800 50  0000 C CNN
F 2 "" H 5925 1950 50  0000 C CNN
F 3 "" H 5925 1950 50  0000 C CNN
	1    5925 1950
	1    0    0    -1  
$EndComp
Text Label 5575 1925 2    60   ~ 0
PPC_-V
Wire Wire Line
	5575 1925 5625 1925
Wire Wire Line
	5825 1925 5925 1925
Wire Wire Line
	5925 1925 5925 1950
$Comp
L PWR_FLAG #FLG2
U 1 1 57D1BD54
P 1325 5300
F 0 "#FLG2" H 1325 5395 50  0001 C CNN
F 1 "PWR_FLAG" H 1325 5480 50  0000 C CNN
F 2 "" H 1325 5300 50  0000 C CNN
F 3 "" H 1325 5300 50  0000 C CNN
	1    1325 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	1325 5300 1325 5375
Connection ~ 1325 5375
Text Label 8525 8950 2    60   ~ 0
uC_VDDA
$Comp
L GNDA #PWR63
U 1 1 57D03606
P 8525 9050
F 0 "#PWR63" H 8525 8800 50  0001 C CNN
F 1 "GNDA" H 8525 8900 50  0000 C CNN
F 2 "" H 8525 9050 50  0000 C CNN
F 3 "" H 8525 9050 50  0000 C CNN
	1    8525 9050
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR73
U 1 1 57D03833
P 9025 9050
F 0 "#PWR73" H 9025 8800 50  0001 C CNN
F 1 "GNDA" H 9025 8900 50  0000 C CNN
F 2 "" H 9025 9050 50  0000 C CNN
F 3 "" H 9025 9050 50  0000 C CNN
	1    9025 9050
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR60
U 1 1 57D040C7
P 8175 7525
F 0 "#PWR60" H 8175 7275 50  0001 C CNN
F 1 "GNDA" H 8175 7375 50  0000 C CNN
F 2 "" H 8175 7525 50  0000 C CNN
F 3 "" H 8175 7525 50  0000 C CNN
	1    8175 7525
	-1   0    0    1   
$EndComp
$Comp
L R_Small R67
U 1 1 57D2BEFE
P 1500 1500
F 0 "R67" H 1530 1520 50  0000 L CNN
F 1 "10k" H 1530 1460 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 1500 1500 50  0001 C CNN
F 3 "" H 1500 1500 50  0000 C CNN
	1    1500 1500
	-1   0    0    1   
$EndComp
Wire Wire Line
	1500 1400 1500 1325
Connection ~ 1500 1325
$Comp
L GND #PWR7
U 1 1 57D2CAA3
P 1500 1650
F 0 "#PWR7" H 1500 1400 50  0001 C CNN
F 1 "GND" H 1500 1500 50  0000 C CNN
F 2 "" H 1500 1650 50  0000 C CNN
F 3 "" H 1500 1650 50  0000 C CNN
	1    1500 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	1500 1600 1500 1650
$Comp
L R_Small R68
U 1 1 57F838E8
P 8150 7925
F 0 "R68" H 8180 7945 50  0000 L CNN
F 1 "10k" H 8180 7885 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 8150 7925 50  0001 C CNN
F 3 "" H 8150 7925 50  0000 C CNN
	1    8150 7925
	1    0    0    -1  
$EndComp
Wire Wire Line
	8500 8125 8500 8000
Wire Wire Line
	8500 7800 8500 7725
Connection ~ 8500 7725
Wire Wire Line
	8150 7825 8150 7725
Connection ~ 8150 7725
Wire Wire Line
	7950 7825 7950 8075
Wire Wire Line
	7950 8075 7950 8200
Wire Wire Line
	7950 8075 8150 8075
Wire Wire Line
	8150 8075 8150 8025
Connection ~ 7950 8075
$EndSCHEMATC
