#include "spi_api.h"

// expander_test 用ボタンNo. 2, 3, 4, 5
int main() {
  init_io_expander();

  for(int i=0; i<16; ++i) {
    getchar();
    write_led(i, 1);
    printf("on %d th button\n", i);
  }

 for(int i=0; i<16; ++i) {
    getchar();
    write_led(i, 0);
    printf("off %d th button\n", i);
  }

/* output test
  for(int i=0; i<4; ++i) {
    getchar();
    write_led(i%4, 1);
    printf("on %d th button\n", i%4 + 2);
  }

  for(int i=0; i<4; ++i) {
    getchar();
    write_led(i%4 + 2, 0);
    printf("off %d th button \n", i%4 + 2);
  }
*/

///* input
  for(int i=0; i<16; ++i) {
    getchar();
    int res = read_switch(i);
    printf("%d th button is %d\n", i, res);
  }
//*/

/*
  while(1) {
    getchar();
    int res = read_switch(2);
    printf("res: %d\n", res);
  }
*/
  return 0;
}
