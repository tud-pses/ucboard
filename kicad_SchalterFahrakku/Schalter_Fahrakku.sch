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
L GND #PWR3
U 1 1 5783BA47
P 4450 4350
F 0 "#PWR3" H 4450 4100 50  0001 C CNN
F 1 "GND" H 4450 4200 50  0000 C CNN
F 2 "" H 4450 4350 50  0000 C CNN
F 3 "" H 4450 4350 50  0000 C CNN
	1    4450 4350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR4
U 1 1 5783BA63
P 4850 4150
F 0 "#PWR4" H 4850 3900 50  0001 C CNN
F 1 "GND" H 4850 4000 50  0000 C CNN
F 2 "" H 4850 4150 50  0000 C CNN
F 3 "" H 4850 4150 50  0000 C CNN
	1    4850 4150
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X03 uC_IN1
U 1 1 5783BAEB
P 3750 3800
F 0 "uC_IN1" H 3750 4000 50  0000 C CNN
F 1 "SIL 3" V 3850 3800 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03" H 3750 3800 50  0001 C CNN
F 3 "" H 3750 3800 50  0000 C CNN
F 4 "1x3 Header" H 3750 3800 60  0001 C CNN "Package"
	1    3750 3800
	-1   0    0    -1  
$EndComp
$Comp
L GND #PWR2
U 1 1 5783C373
P 4000 3275
F 0 "#PWR2" H 4000 3025 50  0001 C CNN
F 1 "GND" H 4000 3125 50  0000 C CNN
F 2 "" H 4000 3275 50  0000 C CNN
F 3 "" H 4000 3275 50  0000 C CNN
	1    4000 3275
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR1
U 1 1 5783C4FA
P 3950 3800
F 0 "#PWR1" H 3950 3550 50  0001 C CNN
F 1 "GND" H 3950 3650 50  0000 C CNN
F 2 "" H 3950 3800 50  0000 C CNN
F 3 "" H 3950 3800 50  0000 C CNN
	1    3950 3800
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR5
U 1 1 5783C5FB
P 5750 3350
F 0 "#PWR5" H 5750 3100 50  0001 C CNN
F 1 "GND" H 5750 3200 50  0000 C CNN
F 2 "" H 5750 3350 50  0000 C CNN
F 3 "" H 5750 3350 50  0000 C CNN
	1    5750 3350
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
	4000 2900 5150 2900
Wire Wire Line
	5750 2900 5550 2900
Wire Wire Line
	5750 3350 5750 3350
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
L CONN_01X01 P1
U 1 1 57ADC701
P 3800 2900
F 0 "P1" H 3800 3000 50  0000 C CNN
F 1 "V+_IN" V 3900 2900 50  0000 C CNN
F 2 "Wire_Pads:SolderWirePad_single_SMD_5x10mm" H 3800 2900 50  0001 C CNN
F 3 "" H 3800 2900 50  0000 C CNN
F 4 "Pad" H 3800 2900 60  0001 C CNN "Package"
	1    3800 2900
	-1   0    0    1   
$EndComp
$Comp
L CONN_01X01 P2
U 1 1 57ADC760
P 3800 3275
F 0 "P2" H 3800 3375 50  0000 C CNN
F 1 "V-_IN" V 3900 3275 50  0000 C CNN
F 2 "Wire_Pads:SolderWirePad_single_SMD_5x10mm" H 3800 3275 50  0001 C CNN
F 3 "" H 3800 3275 50  0000 C CNN
F 4 "Pad" H 3800 3275 60  0001 C CNN "Package"
	1    3800 3275
	-1   0    0    1   
$EndComp
$Comp
L CONN_01X01 P3
U 1 1 57ADC7D4
P 5950 2900
F 0 "P3" H 5950 3000 50  0000 C CNN
F 1 "V+_OUT" V 6050 2900 50  0000 C CNN
F 2 "Wire_Pads:SolderWirePad_single_SMD_5x10mm" H 5950 2900 50  0001 C CNN
F 3 "" H 5950 2900 50  0000 C CNN
F 4 "Pad" H 5950 2900 60  0001 C CNN "Package"
	1    5950 2900
	1    0    0    1   
$EndComp
$Comp
L CONN_01X01 P4
U 1 1 57ADC84B
P 5950 3350
F 0 "P4" H 5950 3450 50  0000 C CNN
F 1 "V-_OUT" V 6050 3350 50  0000 C CNN
F 2 "Wire_Pads:SolderWirePad_single_SMD_5x10mm" H 5950 3350 50  0001 C CNN
F 3 "" H 5950 3350 50  0000 C CNN
F 4 "Pad" H 5950 3350 60  0001 C CNN "Package"
	1    5950 3350
	1    0    0    1   
$EndComp
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
L PWR_FLAG #FLG1
U 1 1 57ADD4B4
P 4000 2900
F 0 "#FLG1" H 4000 2995 50  0001 C CNN
F 1 "PWR_FLAG" H 4000 3080 50  0000 C CNN
F 2 "" H 4000 2900 50  0000 C CNN
F 3 "" H 4000 2900 50  0000 C CNN
	1    4000 2900
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG2
U 1 1 57ADD513
P 4000 3275
F 0 "#FLG2" H 4000 3370 50  0001 C CNN
F 1 "PWR_FLAG" H 4000 3455 50  0000 C CNN
F 2 "" H 4000 3275 50  0000 C CNN
F 3 "" H 4000 3275 50  0000 C CNN
	1    4000 3275
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG3
U 1 1 57ADD53F
P 5750 2900
F 0 "#FLG3" H 5750 2995 50  0001 C CNN
F 1 "PWR_FLAG" H 5750 3080 50  0000 C CNN
F 2 "" H 5750 2900 50  0000 C CNN
F 3 "" H 5750 2900 50  0000 C CNN
	1    5750 2900
	1    0    0    -1  
$EndComp
$Comp
L Hole PD1
U 1 1 57ADE0AF
P 3650 2475
F 0 "PD1" H 3650 2365 60  0000 C CNN
F 1 "Hole" H 3650 2575 60  0000 C CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3" H 3650 2475 60  0001 C CNN
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
F 2 "Mounting_Holes:MountingHole_3.2mm_M3" H 3850 2475 60  0001 C CNN
F 3 "" H 3850 2475 60  0000 C CNN
	1    3850 2475
	1    0    0    -1  
$EndComp
$Comp
L Hole PD3
U 1 1 57ADE2E0
P 4050 2475
F 0 "PD3" H 4050 2365 60  0000 C CNN
F 1 "Hole" H 4050 2575 60  0000 C CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3" H 4050 2475 60  0001 C CNN
F 3 "" H 4050 2475 60  0000 C CNN
	1    4050 2475
	1    0    0    -1  
$EndComp
$Comp
L Hole PD4
U 1 1 57ADE320
P 4250 2475
F 0 "PD4" H 4250 2365 60  0000 C CNN
F 1 "Hole" H 4250 2575 60  0000 C CNN
F 2 "Mounting_Holes:MountingHole_3.2mm_M3" H 4250 2475 60  0001 C CNN
F 3 "" H 4250 2475 60  0000 C CNN
	1    4250 2475
	1    0    0    -1  
$EndComp
$EndSCHEMATC
