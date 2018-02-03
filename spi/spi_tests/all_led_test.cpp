#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "../../spi_api.h"

int main(void)
{
  //unsigned char buf[3] = {0x40, 0x0a, 0xff};
  //unsigned char res[3] = {0x41, 0xaa, 0xee};
  //unsigned char b = 0x24;

  if(wiringPiSPISetup(SPI_CHANNEL, CLOCK) < 0) return -1;
  if(wiringPiSetupGpio() < 0 ) return -1;

  pinMode(SS_PORT, OUTPUT);
  digitalWrite(SS_PORT, 1);

//  write2spi(DEVICE_0, IOCON, 0x08);
  write2spi(DEVICE_0, IODIRA, 0x00);
  write2spi(DEVICE_0, IODIRB, 0x00);
  write2spi(DEVICE_0, GPIOA, 0xff);
  write2spi(DEVICE_0, GPIOB, 0xff);

  unsigned char buffer[3];
  read_from_spi(DEVICE_0, GPIOA, buffer);
  read_from_spi(DEVICE_0, GPIOB, buffer);

  return 0;
}
