OKDO E1 PID DC MOTOR

Module info: <br>
LCD 128 x 160 SPI ST7735S <br>
H-BRIDGE TB6612FNG <br>
Incremental Encoder 20 Pulse / 360* <br>
N20 DC MOTOR 100RPM 1:298 12V

Connection schematic: <br>
TFT (FLXC_8  - SPI)
* LED - VDD (3.3V)
* SCK - PIO1_2 (SCK)
* SDA - PIO0_26 (MOSI)
* A0 -  PIO1_26
* RESET - PIO1_27
* CS - PIO1_1 (SSEL 1)

[Encoder]
* CHA - PIO0_7 (INT)
* CHB - PIO1_0 
* SW - PIO0_31 (INT)

[H-BRIDGE]
* PWMA - PIO1_10 (Ctimer 0 Match 1)
* AIN1 - PIO0_15
* AIN2 - PIO1_5
* STBY - VDD (3.3V)
*Any other - Not used

[DC MOTOR]
* VMOT - 12V <br>
* MOT_A - BO1 (H-Bridge)
* MOT_B - B02 (H-Bridge)
* ENC_A - PIO1_9 (INT)
* ENC_B - PIO1_8

Common: <br>
* VDD - VDD(3.3V) <br>
* GND – GND