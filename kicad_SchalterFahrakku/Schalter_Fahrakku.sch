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
LIBS:Schalter_Fahrakku-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Q_NMOS_GSD Q1
U 1 1 5783B834
P 4750 3900
F 0 "Q1" H 4650 3950 50  0000 R CNN
F 1 "IRLML6344" V 5000 4100 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 4950 4000 50  0001 C CNN
F 3 "http://www.farnell.com/datasheets/1911844.pdf" H 4750 3900 50  0001 C CNN
F 4 "Farnell" H 4750 3900 60  0001 C CNN "Supply"
F 5 "1857299" H 4750 3900 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/international-rectifier/irlml6344trpbf/mosfet-n-kanal-30v-5a-sot23/dp/1857299" H 4750 3900 60  0001 C CNN "URL"
F 7 "\"0,168\"" H 4750 3900 60  0001 C CNN "Price"
F 8 "SOT-23" H 4750 3900 60  0001 C CNN "Package"
	1    4750 3900
	1    0    0    -1  
$EndComp
$Comp
L Q_PMOS_GDS Q2
U 1 1 5783B8B5
P 5350 3000
F 0 "Q2" H 5250 2950 50  0000 R CNN
F 1 "IPD90P03" V 5550 3200 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:TO-252-2Lead" H 5550 3100 50  0001 C CNN
F 3 "http://www.farnell.com/datasheets/1932578.pdf" H 5350 3000 50  0001 C CNN
F 4 "Farnell" H 5350 3000 60  0001 C CNN "Supply"
F 5 "2480839" H 5350 3000 60  0001 C CNN "Part No."
F 6 "http://de.farnell.com/infineon/ipd90p03p4-04/mosfet-p-kanal-30v-90a-to-252/dp/2480839" H 5350 3000 60  0001 C CNN "URL"
F 7 "\"1,39\"" H 5350 3000 60  0001 C CNN "Price"
F 8 "TO252" H 5350 3000 60  0001 C CNN "Package"
	1    5350 3000
	0    1    -1   0   
$EndComp
$Comp
L GND #PWR01
U 1 1 5783BA47
P 4450 4350
F 0 "#PWR01" H 4450 4100 50  0001 C CNN
F 1 "GND" H 4450 4200 50  0000 C CNN
F 2 "" H 4450 4350 50  0000 C CNN
F 3 "" H 4450 4350 50  0000 C CNN
	1    4450 4350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 5783BA63
P 4850 4150
F 0 "#PWR02" H 4850 3900 50  0001 C CNN
F 1 "GND" H 4850 4000 50  0000 C CNN
F 2 "" H 4850 4150 50  0000 C CNN
F 3 "" H 4850 4150 50  0000 C CNN
	1    4850 4150
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X03 P3
U 1 1 5783BAEB
P 3750 3800
F 0 "P3" H 3750 4000 50  0000 C CNN
F 1 "uC_IN" V 3850 3800 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03" H 3750 3800 50  0001 C CNN
F 3 "http://cdn-reichelt.de/documents/datenblatt/C151/PS25_xxWBR.pdf" H 3750 3800 50  0001 C CNN
F 4 "Reichelt" H 3750 3800 60  0001 C CNN "Supply"
F 5 "PS 25/3W BR" H 3750 3800 60  0001 C CNN "Part No."
F 6 "http://www.reichelt.de/PS-25-3W-BR/3/index.html?ACTION=3&LA=446&ARTICLE=14828&artnr=PS+25%2F3W+BR&SEARCH=PS+25" H 3750 3800 60  0001 C CNN "URL"
F 7 "\"0,42\"" H 3750 3800 60  0001 C CNN "Price"
F 8 "1x3 Angled" H 3750 3800 60  0001 C CNN "Package"
	1    3750 3800
	-1   0    0    -1  
$EndComp
$Comp
L GND #PWR03
U 1 1 5783C373
P 4000 3325
F 0 "#PWR03" H 4000 3075 50  0001 C CNN
F 1 "GND" H 4000 3175 50  0000 C CNN
F 2 "" H 4000 3325 50  0000 C CNN
F 3 "" H 4000 3325 50  0000 C CNN
	1    4000 3325
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR04
U 1 1 5783C4FA
P 3950 3800
F 0 "#PWR04" H 3950 3550 50  0001 C CNN
F 1 "GND" H 3950 3650 50  0000 C CNN
F 2 "" H 3950 3800 50  0000 C CNN
F 3 "" H 3950 3800 50  0000 C CNN
	1    3950 3800
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR05
U 1 1 5783C5FB
P 5925 3350
F 0 "#PWR05" H 5925 3100 50  0001 C CNN
F 1 "GND" H 5925 3200 50  0000 C CNN
F 2 "" H 5925 3350 50  0000 C CNN
F 3 "" H 5925 3350 50  0000 C CNN
	1    5925 3350
	1    0    0    -1  
$EndComp
Text Label 4050 3900 3    60   ~ 0
CTL_DRIVE
Wire Wire Line
	4450 3900 4450 4000
Wire Wire Line
	4850 3250 4850 3700
Wire Wire Line
	4850 3350 5350 3350
Wire Wire Line
	5350 3350 5350 3200
Connection ~ 4850 3350
Wire Wire Line
	4450 4200 4450 4350
Wire Wire Line
	4850 4150 4850 4100
Connection ~ 4450 3900
Wire Wire Line
	4450 3700 3950 3700
Wire Wire Line
	4450 2900 4450 3700
Wire Wire Line
	4850 2900 4850 3050
Connection ~ 4850 2900
Wire Wire Line
	3950 3900 4550 3900
Connection ~ 4450 2900
Wire Wire Line
	3950 2900 5150 2900
Wire Wire Line
	5550 2900 6025 2900
Wire Wire Line
	5925 3350 5925 3350
$Comp
L C_Small C1
U 1 1 57ADC1FD
P 4175 4100
F 0 "C1" H 4185 4170 50  0000 L CNN
F 1 "100n" H 4185 4020 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 4175 4100 50  0001 C CNN
F 3 "" H 4175 4100 50  0000 C CNN
F 4 "4V" H 4175 4100 60  0001 C CNN "Spec"
	1    4175 4100
	1    0    0    -1  
$EndComp
$Comp
L R_Small R1
U 1 1 57ADC260
P 4450 4100
F 0 "R1" H 4480 4120 50  0000 L CNN
F 1 "10k" H 4480 4060 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 4450 4100 50  0001 C CNN
F 3 "" H 4450 4100 50  0000 C CNN
	1    4450 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 4300 4175 4300
Wire Wire Line
	4175 4300 4175 4200
Connection ~ 4450 4300
Wire Wire Line
	4175 4000 4175 3900
Connection ~ 4175 3900
$Comp
L R_Small R2
U 1 1 57ADCAFA
P 4850 3150
F 0 "R2" H 4880 3170 50  0000 L CNN
F 1 "10k" H 4880 3110 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 4850 3150 50  0001 C CNN
F 3 "" H 4850 3150 50  0000 C CNN
	1    4850 3150
	1    0    0    -1  
$EndComp
$Comp
L C_Small C2
U 1 1 57ADCC7D
P 5075 3150
F 0 "C2" H 5085 3220 50  0000 L CNN
F 1 "470n" H 5085 3070 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 5075 3150 50  0001 C CNN
F 3 "" H 5075 3150 50  0000 C CNN
F 4 "10V" H 5075 3150 60  0001 C CNN "Spec"
	1    5075 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	5075 3250 5075 3350
Connection ~ 5075 3350
Wire Wire Line
	5075 3050 5075 2900
Connection ~ 5075 2900
$Comp
L PWR_FLAG #FLG06
U 1 1 57ADD4B4
P 4000 2900
F 0 "#FLG06" H 4000 2995 50  0001 C CNN
F 1 "PWR_FLAG" H 4000 3080 50  0000 C CNN
F 2 "" H 4000 2900 50  0000 C CNN
F 3 "" H 4000 2900 50  0000 C CNN
	1    4000 2900
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG07
U 1 1 57ADD513
P 4200 3250
F 0 "#FLG07" H 4200 3345 50  0001 C CNN
F 1 "PWR_FLAG" H 4200 3430 50  0000 C CNN
F 2 "" H 4200 3250 50  0000 C CNN
F 3 "" H 4200 3250 50  0000 C CNN
	1    4200 3250
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG08
U 1 1 57ADD53F
P 5750 2825
F 0 "#FLG08" H 5750 2920 50  0001 C CNN
F 1 "PWR_FLAG" H 5750 3005 50  0000 C CNN
F 2 "" H 5750 2825 50  0000 C CNN
F 3 "" H 5750 2825 50  0000 C CNN
	1    5750 2825
	1    0    0    -1  
$EndComp
$Comp
L Hole PD1
U 1 1 57ADE0AF
P 3650 2475
F 0 "PD1" H 3650 2365 60  0000 C CNN
F 1 "Hole" H 3650 2575 60  0000 C CNN
F 2 "Custom:MountingHole_3.3mm_M3" H 3650 2475 60  0001 C CNN
F 3 "" H 3650 2475 60  0000 C CNN
	1    3650 2475
	1    0    0    -1  
$EndComp
$Comp
L Hole PD2
U 1 1 57ADE295
P 3850 2475
F 0 "PD2" H 3850 2365 60  0000 C CNN
F 1 "Hole" H 3850 2575 60  0000 C CNN
F 2 "Custom:MountingHole_3.3mm_M3" H 3850 2475 60  0001 C CNN
F 3 "" H 3850 2475 60  0000 C CNN
	1    3850 2475
	1    0    0    -1  
$EndComp
Connection ~ 4000 2900
Wire Wire Line
	3950 3275 4200 3275
Wire Wire Line
	4000 3275 4000 3325
Wire Wire Line
	4200 3275 4200 3250
Connection ~ 4000 3275
Wire Wire Line
	5750 2825 5750 2900
Connection ~ 5750 2900
Wire Wire Line
	6025 3275 5925 3275
Wire Wire Line
	5925 3275 5925 3350
$Comp
L R_Small R3
U 1 1 57C815D8
P 5600 3175
F 0 "R3" H 5630 3195 50  0000 L CNN
F 1 "3k6" H 5630 3135 50  0000 L CNN
F 2 "" H 5600 3175 50  0001 C CNN
F 3 "" H 5600 3175 50  0000 C CNN
	1    5600 3175
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 P1
U 1 1 57C8181A
P 3750 2900
F 0 "P1" H 3750 3000 50  0000 C CNN
F 1 "V_IN+" V 3850 2900 50  0000 C CNN
F 2 "" H 3750 2900 50  0001 C CNN
F 3 "" H 3750 2900 50  0000 C CNN
	1    3750 2900
	-1   0    0    1   
$EndComp
$Comp
L CONN_01X01 P2
U 1 1 57C8188D
P 3750 3275
F 0 "P2" H 3750 3375 50  0000 C CNN
F 1 "V_IN-" V 3850 3275 50  0000 C CNN
F 2 "" H 3750 3275 50  0001 C CNN
F 3 "" H 3750 3275 50  0000 C CNN
	1    3750 3275
	-1   0    0    1   
$EndComp
$Comp
L CONN_01X01 P4
U 1 1 57C8199A
P 6225 2900
F 0 "P4" H 6225 3000 50  0000 C CNN
F 1 "V_OUT+" V 6325 2900 50  0000 C CNN
F 2 "" H 6225 2900 50  0001 C CNN
F 3 "" H 6225 2900 50  0000 C CNN
	1    6225 2900
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 P5
U 1 1 57C81A43
P 6225 3275
F 0 "P5" H 6225 3375 50  0000 C CNN
F 1 "V_OUT+" V 6325 3275 50  0000 C CNN
F 2 "" H 6225 3275 50  0001 C CNN
F 3 "" H 6225 3275 50  0000 C CNN
	1    6225 3275
	1    0    0    -1  
$EndComp
$Comp
L LED D1
U 1 1 57C81BD8
P 5600 3550
F 0 "D1" H 5600 3650 50  0000 C CNN
F 1 "LED" H 5600 3450 50  0000 C CNN
F 2 "Custom:LED_0603_HandSoldering" H 5600 3550 50  0001 C CNN
F 3 "" H 5600 3550 50  0000 C CNN
	1    5600 3550
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR09
U 1 1 57C81C76
P 5600 3825
F 0 "#PWR09" H 5600 3575 50  0001 C CNN
F 1 "GND" H 5600 3675 50  0000 C CNN
F 2 "" H 5600 3825 50  0000 C CNN
F 3 "" H 5600 3825 50  0000 C CNN
	1    5600 3825
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 3825 5600 3750
Wire Wire Line
	5600 3350 5600 3275
Wire Wire Line
	5600 3075 5600 2900
Connection ~ 5600 2900
$EndSCHEMATC
