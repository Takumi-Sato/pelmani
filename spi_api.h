#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <stdio.h>

#define SS_PORT 8
#define IODIRA 0x00
#define IODIRB 0x01
#define IOCON 0x0a
#define GPIOA 0x12
#define GPIOB 0x13
#define GPPUA 0x0c
#define GPPUB 0x0d
#define DEVICE_ADDR 0x40
#define DEVICE_0 0x40
#define DEVICE_1 0x42
#define DEVICE_2 0x44
#define DEVICE_3 0x45
#define DEVICE_RESET_PORT 16
#define IS_LED 1
#define IS_SWITCH 0
#define SPI_CHANNEL 0
#define CLOCK 10000000

void write2spi(unsigned char device, unsigned char regaddr, unsigned char tx_data);
void read_from_spi(unsigned char device, unsigned char resgaddr, unsigned char* res);
int init_io_expander();
void write_led(int button_num, int value);
int read_switch(int button_num);
unsigned char get_device_by_button(int button_num);
int get_pin_by_button(int button_num, int is_led);
