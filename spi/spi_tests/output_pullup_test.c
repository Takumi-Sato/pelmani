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
#define GPPUA 0x0c
#define GPPUB 0x0d
#define DEVICE_ADDR 0x40
#define DEVICE_0 0x40
#define DEVICE_1 0x42
#define SPI_CHANNEL 0
#define CLOCK 10000000

void write2spi(unsigned char device, unsigned char regaddr, unsigned char tx_data);
void read_from_spi(unsigned char device, unsigned char resgaddr, unsigned char* res);

int main(void)
{
  unsigned char res[3];

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

  write2spi(DEVICE_ADDR, IOCON, 0x08);
  write2spi(DEVICE_0, GPPUA, 0xff);
  write2spi(DEVICE_1, GPPUB, 0xff);
  write2spi(DEVICE_0, IODIRA, 0x00);
  write2spi(DEVICE_1, IODIRB, 0xff);
  write2spi(DEVICE_0, GPIOA, 0x40);
  read_from_spi(DEVICE_1, GPIOB, res);
  printf("res = 0x%x, 0x%x, 0x%x\n", res[0], res[1], res[2]);
  write2spi(DEVICE_0, GPIOA, 0xff);

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

void read_from_spi(unsigned char device, unsigned char regaddr, unsigned char* res)
{
  device = device | 0x01; // read from spi device
  res[0] = device;
  res[1] = regaddr;
  res[2] = 0x0;
  res[3] = 0x0;

  printf("[BEFORE]\ndevice:0x%x, regaddr:0x%x\n", res[0], res[1]);
  digitalWrite(SS_PORT, 0);
  wiringPiSPIDataRW(SPI_CHANNEL, res, 3);
  digitalWrite(SS_PORT, 1);

  printf("[AFTER]\nbuf[0]:0x%x, buf[1]:0x%x, buf[2]:0x%x, buf[3]:0x%x\n", res[0], res[1], res[2], res[3]);
}
