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
#define DEVICE_RESET_PORT 16  // IOエキスパンダの^RST端子に接続されている、raspberry pi の GPIO ピン番号
#define IS_LED 1
#define IS_SWITCH 0
#define SPI_CHANNEL 0
#define CLOCK 10000000

void write2spi(unsigned char device, unsigned char regaddr, unsigned char tx_data);
void read_from_spi(unsigned char device, unsigned char resgaddr, unsigned char* res);
void init_io_expander();
void write_led(int button_num, int value);
int read_switch(int button_num);

int main(void)
{
  init_io_expander();
  return 0;
}

void write2spi(unsigned char device, unsigned char regaddr, unsigned char tx_data)
{
  unsigned char buf[3];
  device = device & 0xfe; // I/O bit = write to spi device
  buf[0] = device;
  buf[1] = regaddr;
  buf[2] = tx_data;

  printf("[WRITE]\ndevice:0x%x, regaddr:0x%x, data:0x%x\n", buf[0], buf[1], buf[2]);
  digitalWrite(SS_PORT, 0);
  wiringPiSPIDataRW(SPI_CHANNEL, buf, sizeof(buf));
  digitalWrite(SS_PORT, 1);
}

void read_from_spi(unsigned char device, unsigned char regaddr, unsigned char* res)
{
  device = device | 0x01; // read from spi device
  res[0] = device;
  res[1] = regaddr;
  res[2] = 0x0;

  printf("[READ]\ndevice:0x%x, regaddr:0x%x ", res[0], res[1]);
  digitalWrite(SS_PORT, 0);
  wiringPiSPIDataRW(SPI_CHANNEL, res, 3);
  digitalWrite(SS_PORT, 1);

  printf("result : 0x%x\n", res[2]);
}

void init_io_expander()
{
  if((wiringPiSPISetup (SPI_CHANNEL, CLOCK)) < 0) {
    printf("wiringPiSPISetup error \n");
    return -1;
  }
  if(wiringPiSetupGpio() == -1) {
    printf("wiringPiSetupGpio error\n");
    return -1; 
  }

  // Reset I/O Expanders
  pinMode(DEVICE_RESET_PORT, OUTPUT);
  digitalWrite(DEVICE_RESET_PORT, 0);
  delay(100);
  degitalWrite(DEVICE_RESET_PORT, 1);

  pinMode(SS_PORT, OUTPUT);
  digitalWrite(SS_PORT, 1);

  // Enabled device address 
  write2spi(DEVICE_ADDR, IOCON, 0x08);

  write2spi(DEVICE_0, IODIRA, 0x00);
  write2spi(DEVICE_0, GPPUB, 0xff);
  write2spi(DEVICE_0, IODIRB, 0xff);
  write2spi(DEVICE_0, GPIOA, 0x40);
  write2spi(DEVICE_1, IODIRA, 0x00);
  write2spi(DEVICE_1, GPPUB, 0xff);
  write2spi(DEVICE_1, IODIRB, 0xff);
  write2spi(DEVICE_1, GPIOA, 0x40);
  write2spi(DEVICE_2, IODIRA, 0x00);
  write2spi(DEVICE_2, GPPUB, 0xff);
  write2spi(DEVICE_2, IODIRB, 0xff);
  write2spi(DEVICE_2, GPIOA, 0x40);
  write2spi(DEVICE_3, IODIRA, 0x00);
  write2spi(DEVICE_3, GPPUB, 0xff);
  write2spi(DEVICE_3, IODIRB, 0xff);
  write2spi(DEVICE_3, GPIOA, 0x40);
}

void write_led(int button_num, int value)
{
  unsigned char device = get_device_by_button(button_num);
  int bit_num = get_pin_by_button(button_num, IS_LED);
  unsigned char write_value = 0x01 << bit_num;
  unsigned char buf[3];
  read_from_spi(device, GPIOA, buf);

  if(value != 0)
  {
    write2spi(device, GPIOA, write_value | buf[2]);
  }
  else 
  {
    write2spi(device ,GPIOA, ~write_value & buf[2]);
  }
}

int read_switch(int button_num)
{
  unsigned char device = get_device_by_button(button_num);
  int bit_num = get_pin_by_button(button_num, IS_SWITCH);
  unsigned char cond = 0x01 << bit_num;

  unsigned char buf[3];
  read_from_spi(device, GPIOB, buf);

  if(buf[2] & cond != 0) return 1;
  return 0; 
}

// return: ID of I/O Expander which contains the button
unsigned char get_device_by_button(int button_num)
{
  switch(button_num)
  {
    case 0:
    case 1:
    case 2:
    case 3:
      return DEVICE_0;
    case 4:
    case 5:
    case 6:
    case 7:
      return DEVICE_1; 
    case 8:
    case 9:
    case 10:
    case 11:
      return DEVICE_2; 
    case 12:
    case 13:
    case 14:
    case 15:
      return DEVICE_3;
    default:
      break;
  }
  return -1;
}

// return: count of left-shift
int get_pin_by_button(int button_num, int is_led)
{
  int res;
  switch(button_num)
  {
    case 0:
    case 4:
    case 8:
    case 12:
      res = 1;
      break;
    case 1:
    case 5:
    case 9:
    case 13:
      res = 3;
      break;
    case 2:
    case 6:
    case 10:
    case 14:
      res = 5;
      break;
    case 3:
    case 7:
    case 11:
    case 15:
      res = 7;
      break;
    default:
      return -1;
  }

  if(is_led != 0) return res;
  return 7 - res;
}
