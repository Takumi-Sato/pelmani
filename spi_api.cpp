#include "spi_api.h"

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
  wiringPiSPIDataRW(SPI_CHANNEL, res, sizeof(res));
  digitalWrite(SS_PORT, 1);

  printf("result : 0x%x\n", res[2]);
}

int init_io_expander()
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
  digitalWrite(DEVICE_RESET_PORT, 1);

  pinMode(SS_PORT, OUTPUT);
  digitalWrite(SS_PORT, 1);

  // Enabled device address 
  write2spi(DEVICE_ADDR, IOCON, 0x08);

  write2spi(DEVICE_0, IODIRA, 0x00);
  write2spi(DEVICE_0, GPPUB, 0xff);
  write2spi(DEVICE_0, IODIRB, 0xff);
  write2spi(DEVICE_0, GPIOA, 0x00);
  write2spi(DEVICE_1, IODIRA, 0x00);
  write2spi(DEVICE_1, GPPUB, 0xff);
  write2spi(DEVICE_1, IODIRB, 0xff);
  write2spi(DEVICE_1, GPIOA, 0x00);
  write2spi(DEVICE_2, IODIRA, 0x00);
  write2spi(DEVICE_2, GPPUB, 0xff);
  write2spi(DEVICE_2, IODIRB, 0xff);
  write2spi(DEVICE_2, GPIOA, 0x00);
  write2spi(DEVICE_3, IODIRA, 0x00);
  write2spi(DEVICE_3, GPPUB, 0xff);
  write2spi(DEVICE_3, IODIRB, 0xff);
  write2spi(DEVICE_3, GPIOA, 0x00);

  return 0;
}

void write_led(int button_num, int value)
{
  unsigned char device = get_device_by_button(button_num);
  int bit_num = get_pin_by_button(button_num, IS_LED);
  unsigned char bit_position = 0x01 << bit_num;
  printf("bit_num:%d write_value:0x%x\n", bit_num, bit_position);
  unsigned char buf[3];
  read_from_spi(device, GPIOA, buf);

  unsigned char write_value;
  if(value != 0)
  {
    write_value = bit_position | buf[2];
    printf("[WRITE_LED] bit_num:%d, device:0x%x, bit_position:0x%x, write_value:0x%x\n", bit_num, device, bit_position, write_value);
    write2spi(device, GPIOA, write_value);
  }
  else 
  {
    write_value = ~bit_position & buf[2]; 
    printf("[WRITE_LED] bit_num:%d, device:0x%x, bit_position:0x%x, write_value:0x%x\n", bit_num, device, bit_position, write_value);
    write2spi(device ,GPIOA, write_value);
  }
}

int read_switch(int button_num)
{
  unsigned char device = get_device_by_button(button_num);
  int bit_num = get_pin_by_button(button_num, IS_SWITCH);
  unsigned char bit_position = 0x01 << bit_num;
  printf("[READ_SWITCH] bit_num:%d, device:0x%x, bit_position:0x%x ", bit_num, device, bit_position);

  unsigned char buf[3];
  read_from_spi(device, GPIOB, buf);
  printf("result:0x%x\n", buf[2]);

  unsigned char value = buf[2] & bit_position;
  if(value != 0) return 1;
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
  return 'e';
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
