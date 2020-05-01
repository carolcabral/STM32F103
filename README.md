# STM32F103


* STM32F103C8T6 microcontroller (ARM Cortex-M3)
* 72MHz
* Flash: 64/128 KB
* SRAM: 20 KB
* 3 x General purpose + 1 x Advanced control timer
* 2 x SPI 
* 2 x I2C
* 3 x USART
* 1 x USB
* 1 x CAN
* 37 x GPIO
* 2 x ADC / 10 channels (12 bits)
* LQFP48 / UFQFPN48 package
* Little endian LSB first



![Bluepill pinout](https://solovjov.net/reblag.dk/The-Generic-STM32F103-Pinout-Diagram.jpg)




###OpenOCD Configurations
Config options: 

    -f /usr/share/openocd/scripts/board/bluepill.cfg
	-f interface/stlink-v2.cfg 


###Putty command (Debugging via UART)
- Baud rate = 9600
- Data bit = 8
- Stop bits = 1
- Parity = None
- Flow control = None

    sudo putty /dev/ttyUSB0 -serial -sercfg 9600,8,n,1,N
