EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 2
Title "Multimeter"
Date "2020-07-25"
Rev "V0.1"
Comp "espressif"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Device:R R1
U 1 1 5F1C13A3
P 4000 2100
F 0 "R1" H 4070 2146 50  0000 L CNN
F 1 "10K" H 4070 2055 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 3930 2100 50  0001 C CNN
F 3 "~" H 4000 2100 50  0001 C CNN
	1    4000 2100
	1    0    0    -1  
$EndComp
$Comp
L RF_Module:ESP32-WROOM-32D U1
U 1 1 5F1C2A6F
P 5400 3450
F 0 "U1" H 5550 4950 50  0000 C CNN
F 1 "ESP32-WROOM-32D" H 5900 4850 50  0000 C CNN
F 2 "RF_Module:ESP32-WROOM-32" H 5400 1950 50  0001 C CNN
F 3 "https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf" H 5100 3500 50  0001 C CNN
	1    5400 3450
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x06 J1
U 1 1 5F1CB114
P 1100 7150
F 0 "J1" H 1180 7142 50  0000 L CNN
F 1 "Conn_01x06" H 1180 7051 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 1100 7150 50  0001 C CNN
F 3 "~" H 1100 7150 50  0001 C CNN
	1    1100 7150
	-1   0    0    1   
$EndComp
$Comp
L power:+3.3V #PWR03
U 1 1 5F1CEB52
P 4000 1950
F 0 "#PWR03" H 4000 1800 50  0001 C CNN
F 1 "+3.3V" V 4015 2078 50  0000 L CNN
F 2 "" H 4000 1950 50  0001 C CNN
F 3 "" H 4000 1950 50  0001 C CNN
	1    4000 1950
	1    0    0    -1  
$EndComp
Text Label 4450 2450 0    50   ~ 0
ADC_CH0
$Comp
L Device:C C1
U 1 1 5F1D0928
P 4000 2400
F 0 "C1" H 4115 2446 50  0000 L CNN
F 1 "1uf" H 4115 2355 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4038 2250 50  0001 C CNN
F 3 "~" H 4000 2400 50  0001 C CNN
	1    4000 2400
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 2450 4800 2450
Text Label 4450 2550 0    50   ~ 0
ADC_CH3
Text Label 6100 2250 0    50   ~ 0
IO_0
Text Label 6100 2450 0    50   ~ 0
IO_2
$Comp
L power:GND #PWR07
U 1 1 5F1D3DBE
P 5400 4850
F 0 "#PWR07" H 5400 4600 50  0001 C CNN
F 1 "GND" H 5405 4677 50  0000 C CNN
F 2 "" H 5400 4850 50  0001 C CNN
F 3 "" H 5400 4850 50  0001 C CNN
	1    5400 4850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 5F1D4ECF
P 1300 7350
F 0 "#PWR02" H 1300 7100 50  0001 C CNN
F 1 "GND" H 1305 7177 50  0000 C CNN
F 2 "" H 1300 7350 50  0001 C CNN
F 3 "" H 1300 7350 50  0001 C CNN
	1    1300 7350
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR01
U 1 1 5F1D55B5
P 1300 7250
F 0 "#PWR01" H 1300 7100 50  0001 C CNN
F 1 "+3.3V" V 1315 7378 50  0000 L CNN
F 2 "" H 1300 7250 50  0001 C CNN
F 3 "" H 1300 7250 50  0001 C CNN
	1    1300 7250
	0    1    1    0   
$EndComp
Text Label 1300 7050 0    50   ~ 0
IO_0
Text Label 1300 7150 0    50   ~ 0
IO_2
Wire Wire Line
	6000 2450 6100 2450
Wire Wire Line
	6000 2250 6100 2250
Text Label 6100 2350 0    50   ~ 0
TXD0
Text Label 6100 2550 0    50   ~ 0
RXD0
Wire Wire Line
	6000 2550 6100 2550
Wire Wire Line
	6000 2350 6100 2350
Text Label 1300 6850 0    50   ~ 0
TXD0
Text Label 1300 6950 0    50   ~ 0
RXD0
$Sheet
S 8350 3100 550  500 
U 5F2186C0
F0 "power" 50
F1 "power.sch" 50
F2 "CHRG" O L 8350 3250 50 
$EndSheet
Text Label 6100 4450 0    50   ~ 0
ADC_CH6
Text Label 6100 4550 0    50   ~ 0
ADC_CH7
Text Label 6100 4250 0    50   ~ 0
ADC_CH4
Text Label 6100 4350 0    50   ~ 0
ADC_CH5
Wire Wire Line
	6000 4250 6100 4250
Wire Wire Line
	6000 4350 6100 4350
Wire Wire Line
	6000 4450 6100 4450
Wire Wire Line
	6000 4550 6100 4550
Wire Wire Line
	4450 2550 4800 2550
Text Label 7550 4500 0    50   ~ 0
ADC_CH6
Text Label 7550 4600 0    50   ~ 0
ADC_CH7
Text Label 7550 4300 0    50   ~ 0
ADC_CH4
Text Label 7550 4400 0    50   ~ 0
ADC_CH5
Text Label 7550 4900 0    50   ~ 0
ADC_CH0
Text Label 7550 5150 0    50   ~ 0
ADC_CH3
Wire Wire Line
	4800 2250 4000 2250
Connection ~ 4000 2250
$Comp
L power:GND #PWR04
U 1 1 5F257552
P 4000 2550
F 0 "#PWR04" H 4000 2300 50  0001 C CNN
F 1 "GND" H 4005 2377 50  0000 C CNN
F 2 "" H 4000 2550 50  0001 C CNN
F 3 "" H 4000 2550 50  0001 C CNN
	1    4000 2550
	1    0    0    -1  
$EndComp
$Comp
L Device:C C3
U 1 1 5F258386
P 5250 1850
F 0 "C3" V 5150 1700 50  0000 L CNN
F 1 "0.1uf" V 5350 1650 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 5288 1700 50  0001 C CNN
F 3 "~" H 5250 1850 50  0001 C CNN
	1    5250 1850
	0    1    1    0   
$EndComp
$Comp
L Device:C C2
U 1 1 5F25A910
P 5250 1550
F 0 "C2" V 5150 1400 50  0000 L CNN
F 1 "10uf" V 5350 1350 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 5288 1400 50  0001 C CNN
F 3 "~" H 5250 1550 50  0001 C CNN
	1    5250 1550
	0    1    1    0   
$EndComp
Wire Wire Line
	5400 2050 5400 1850
Connection ~ 5400 1850
Wire Wire Line
	5400 1850 5400 1550
Wire Wire Line
	5100 1850 4950 1850
Wire Wire Line
	4950 1850 4950 1700
Wire Wire Line
	4950 1550 5100 1550
$Comp
L power:GND #PWR05
U 1 1 5F25B54A
P 4950 1700
F 0 "#PWR05" H 4950 1450 50  0001 C CNN
F 1 "GND" H 4955 1527 50  0000 C CNN
F 2 "" H 4950 1700 50  0001 C CNN
F 3 "" H 4950 1700 50  0001 C CNN
	1    4950 1700
	0    1    1    0   
$EndComp
Connection ~ 4950 1700
Wire Wire Line
	4950 1700 4950 1550
$Comp
L power:+3.3V #PWR06
U 1 1 5F25BCA2
P 5400 1450
F 0 "#PWR06" H 5400 1300 50  0001 C CNN
F 1 "+3.3V" V 5415 1578 50  0000 L CNN
F 2 "" H 5400 1450 50  0001 C CNN
F 3 "" H 5400 1450 50  0001 C CNN
	1    5400 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 1450 5400 1550
Connection ~ 5400 1550
NoConn ~ 4800 3450
NoConn ~ 4800 3550
NoConn ~ 4800 3650
NoConn ~ 4800 3750
NoConn ~ 4800 3850
NoConn ~ 4800 3950
NoConn ~ 6000 3950
NoConn ~ 6000 4050
NoConn ~ 6000 2650
NoConn ~ 6000 2750
NoConn ~ 6000 2850
NoConn ~ 6000 2950
NoConn ~ 6000 3050
NoConn ~ 6000 3150
NoConn ~ 6000 3250
NoConn ~ 6000 3350
NoConn ~ 6000 3450
NoConn ~ 6000 3550
NoConn ~ 6000 3650
NoConn ~ 6000 3750
NoConn ~ 6000 3850
Wire Wire Line
	6000 4150 7200 4150
Wire Wire Line
	7200 4150 7200 3250
Wire Wire Line
	7200 3250 8350 3250
NoConn ~ 6000 4250
NoConn ~ 6000 4350
NoConn ~ 6000 4450
NoConn ~ 6000 4550
$Comp
L Device:R R5
U 1 1 5F2710D2
P 8150 4900
F 0 "R5" V 8050 4850 50  0000 C CNN
F 1 "1K" V 8150 4850 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 8080 4900 50  0001 C CNN
F 3 "~" H 8150 4900 50  0001 C CNN
	1    8150 4900
	0    1    1    0   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J4
U 1 1 5F273439
P 8600 5000
F 0 "J4" H 8680 5042 50  0000 L CNN
F 1 "Conn_01x03" H 8680 4951 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Horizontal" H 8600 5000 50  0001 C CNN
F 3 "~" H 8600 5000 50  0001 C CNN
	1    8600 5000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 5F2780AF
P 8400 5100
F 0 "#PWR0101" H 8400 4850 50  0001 C CNN
F 1 "GND" H 8405 4927 50  0000 C CNN
F 2 "" H 8400 5100 50  0001 C CNN
F 3 "" H 8400 5100 50  0001 C CNN
	1    8400 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	8300 4900 8400 4900
Wire Wire Line
	7550 4900 8000 4900
Wire Wire Line
	7550 5150 8200 5150
Wire Wire Line
	8200 5150 8200 5000
Wire Wire Line
	8200 5000 8400 5000
$EndSCHEMATC
