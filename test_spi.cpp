#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "spi_api.h"

int main(void)
{
  if(init_io_expander() == -1) {
    fputs("init error\n", stderr);
    return -1;
  }

  getchar();

  for(int i=0; i<16; ++i) {
    write_led(i, 1);
  }

  return 0;
}
