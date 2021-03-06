#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <stdio.h>

#define SS_PORT 8
#define IOCON 0x0a
#define IODIRA 0x00
#define IODIRB 0x01
#define IOCON 0x0a
#define GPIOA 0x12
#define GPIOB 0x13
#define DEVICE_ADDR 0x40
#define DEVICE_0 0x40
#define DEVICE_1 0x42
#define SPI_CHANNEL 0
#define CLOCK 10000000

void write2spi(unsigned char device, unsigned char regaddr, unsigned char tx_data);

int main(void)
{
  if((wiringPiSPISetup (SPI_CHANNEL, CLOCK)) < 0) {
    printf("wiringPiSPISetup error \n");
    return -1;
  }
  if(wiringPiSetupGpio() == -1) {
    printf("wiringPiSetupGpio error\n");
    return -1; 
  }

  pinMode(SS_PORT, OUTPUT);
  digitalWrite(SS_PORT, 1);

  pinMode(16, OUTPUT);
  digitalWrite(16, 0);
  delay(100);
  digitalWrite(16, 1);
  delay(100);

  write2spi(DEVICE_ADDR, IOCON, 0x08);
  write2spi(DEVICE_0, IODIRA, 0x00);
  write2spi(DEVICE_1, IODIRB, 0x00);
  write2spi(DEVICE_0, GPIOA, 0x00);
  write2spi(DEVICE_1, GPIOB, 0x00);
  write2spi(DEVICE_0, GPIOA, 0xff);
  write2spi(DEVICE_1, GPIOB, 0xff);

  return 0;
}

void write2spi(unsigned char device, unsigned char regaddr, unsigned char tx_data)
{
  unsigned char buf[3];
  device = device & 0xfe; // I/O bit = write to spi device
  buf[0] = device;
  buf[1] = regaddr;
  buf[2] = tx_data;
  printf("[BEFORE]\ndevice:0x%x, regaddr:0x%x, data:0x%x\n", buf[0], buf[1], buf[2]);
  digitalWrite(SS_PORT, 0);
  wiringPiSPIDataRW(SPI_CHANNEL, buf, sizeof(buf));
  digitalWrite(SS_PORT, 1);

  printf("[AFTER]\ndevice:0x%x, regaddr:0x%x, data:0x%x\n", buf[0], buf[1], buf[2]);
}


