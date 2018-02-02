CC := g++
CFLAGS := -g -Wall -Wextra
WIRINGPI := -l wiringPi

pelmanion: pelmanism_test.o spi_api.o
	$(CC) $(CFLAGS) -o pelmanion pelmanism_test.o spi_api.o $(WIRINGPI)

pelmanism_test.o: pelmanism_test.cpp
	$(CC) $(CFLAGS) -c pelmanism_test.cpp $(WIRINGPI)

spi_api.o: spi_api.cpp
	$(CC) $(CFLAGS) -c spi_api.cpp $(WIRINGPI)

clean:
	rm -f *.o pelmanion
