EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 4
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
L Device:R R?
U 1 1 5F38F7DF
P 3700 2750
F 0 "R?" V 3493 2750 50  0000 C CNN
F 1 "1M" V 3584 2750 50  0000 C CNN
F 2 "" V 3630 2750 50  0001 C CNN
F 3 "~" H 3700 2750 50  0001 C CNN
	1    3700 2750
	0    1    1    0   
$EndComp
$Comp
L Device:C C?
U 1 1 5F38FC82
P 6800 1950
F 0 "C?" H 6915 1996 50  0000 L CNN
F 1 "C" H 6915 1905 50  0000 L CNN
F 2 "" H 6838 1800 50  0001 C CNN
F 3 "~" H 6800 1950 50  0001 C CNN
	1    6800 1950
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5F39077B
P 5550 3150
F 0 "R?" H 5480 3104 50  0000 R CNN
F 1 "3.6k" H 5480 3195 50  0000 R CNN
F 2 "" V 5480 3150 50  0001 C CNN
F 3 "~" H 5550 3150 50  0001 C CNN
	1    5550 3150
	-1   0    0    1   
$EndComp
$Comp
L Device:R R?
U 1 1 5F3909D4
P 5850 3150
F 0 "R?" H 5780 3104 50  0000 R CNN
F 1 "24k" H 5780 3195 50  0000 R CNN
F 2 "" V 5780 3150 50  0001 C CNN
F 3 "~" H 5850 3150 50  0001 C CNN
	1    5850 3150
	-1   0    0    1   
$EndComp
$Comp
L Device:R R?
U 1 1 5F390BF5
P 6150 3150
F 0 "R?" H 6220 3196 50  0000 L CNN
F 1 "75k" H 6220 3105 50  0000 L CNN
F 2 "" V 6080 3150 50  0001 C CNN
F 3 "~" H 6150 3150 50  0001 C CNN
	1    6150 3150
	1    0    0    -1  
$EndComp
$Comp
L Analog_Switch:CD4052B U?
U 1 1 5F3993D3
P 4750 2950
F 0 "U?" H 4750 3831 50  0000 C CNN
F 1 "CD4052B" H 4750 3740 50  0000 C CNN
F 2 "" H 4900 2200 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/cd4052b.pdf" H 4730 3150 50  0001 C CNN
	1    4750 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3850 2750 4250 2750
Wire Wire Line
	5250 2950 5550 2950
Wire Wire Line
	5550 2950 5550 3000
Wire Wire Line
	5250 2850 5850 2850
Wire Wire Line
	5850 2850 5850 3000
Wire Wire Line
	5250 2750 6150 2750
Wire Wire Line
	6150 2750 6150 3000
Wire Wire Line
	5550 3300 5850 3300
Connection ~ 5850 3300
Wire Wire Line
	5850 3300 6150 3300
$Comp
L power:GND #PWR?
U 1 1 5F3A9C86
P 5850 3300
F 0 "#PWR?" H 5850 3050 50  0001 C CNN
F 1 "GND" H 5855 3127 50  0000 C CNN
F 2 "" H 5850 3300 50  0001 C CNN
F 3 "" H 5850 3300 50  0001 C CNN
	1    5850 3300
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J?
U 1 1 5F3AA4C8
P 2550 2750
F 0 "J?" H 2468 2525 50  0000 C CNN
F 1 "V_Ω" H 2468 2616 50  0000 C CNN
F 2 "" H 2550 2750 50  0001 C CNN
F 3 "~" H 2550 2750 50  0001 C CNN
	1    2550 2750
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J?
U 1 1 5F3AB37C
P 2550 1800
F 0 "J?" H 2468 1575 50  0000 C CNN
F 1 "GROUND" H 2468 1666 50  0000 C CNN
F 2 "" H 2550 1800 50  0001 C CNN
F 3 "~" H 2550 1800 50  0001 C CNN
	1    2550 1800
	-1   0    0    1   
$EndComp
Wire Wire Line
	2750 2750 3550 2750
$Comp
L Connector_Generic:Conn_01x01 J?
U 1 1 5F3AB9B1
P 2550 3800
F 0 "J?" H 2468 3575 50  0000 C CNN
F 1 "Current" H 2468 3666 50  0000 C CNN
F 2 "" H 2550 3800 50  0001 C CNN
F 3 "~" H 2550 3800 50  0001 C CNN
	1    2550 3800
	-1   0    0    1   
$EndComp
$EndSCHEMATC
