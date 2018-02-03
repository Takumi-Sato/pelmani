#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "../../spi_api.h"

int main(void)
{
  unsigned char buf[3] = {0x40, 0x0a, 0xff};
  unsigned char res[3] = {0x41, 0xaa, 0xee};
  unsigned char b = 0x24;

  if(wiringPiSPISetup(SPI_CHANNEL, 10000000) < 0) return -1;
  if(wiringPiSetupGpio() < 0 ) return -1;

  pinMode(SS_PORT, OUTPUT);
  digitalWrite(SS_PORT, 1);

  printf("0x%x, 0x%x, 0x%x\n", buf[0], buf[1], buf[2]);
  digitalWrite(SS_PORT, 0);
  wiringPiSPIDataRW(SPI_CHANNEL, buf, sizeof(buf));
  wiringPiSPIDataRW(SPI_CHANNEL, &b, sizeof(b));
  digitalWrite(SS_PORT, 1);
  printf("0x%x, 0x%x, 0x%x\n", buf[0], buf[1], buf[2]);
  printf("0x%x\n", b);
}
