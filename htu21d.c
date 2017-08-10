/*
 * HTU21D data logger.
 *
 *      gcc -lwiringPi soft_i2c.c htu21d.c -o htu21d
 */
 
#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <time.h>

#include "soft_i2c.h"

#define ADDRESS 0x40


struct timeval GetTimeStamp() {
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return tv;
}

float read_rht (i2c_t i2c, char cmd) {
	int value;
	int vcheck;

	// Send command
	i2c_start(i2c);

	if (i2c_send_byte(i2c, ADDRESS << 1 | I2C_WRITE) != I2C_ACK) {
		fprintf(stderr, "No device found at address %02x.\n", ADDRESS);
		exit(2);
	}

	i2c_send_byte(i2c, cmd);

	// Read values
	i2c_start(i2c);
	i2c_send_byte(i2c, ADDRESS << 1 | I2C_READ);

	value  = (i2c_read_byte(i2c) << 8);
	i2c_send_bit(i2c, I2C_ACK);

	value  = value + i2c_read_byte(i2c) & 0xFFFC; // status bits = 0
	i2c_send_bit(i2c, I2C_ACK);

	vcheck = i2c_read_byte(i2c);
	i2c_send_bit(i2c, I2C_NACK);

	i2c_stop(i2c);


	// Verify checksum:
	// too lazy :(


	return (float)value/(float)0x10000;
}


int main (int argc, char **argv)
{

	if (wiringPiSetup () == -1)
		return 1;

	// Check arguments
	if (argc != 3) {
		printf("Usage: %s scl sda\n", argv[0]);
		printf("  scl and sda are SCL and SDA pins in WiringPi numeration.\n");
		return(0);
	}

	int scl = atoi(argv[1]);
	int sda = atoi(argv[2]);

	i2c_t i2c = i2c_init(scl, sda);

	struct timeval tv = GetTimeStamp();

	printf ("%u.%03u\t%.2f\t%.2f\n", 
			tv.tv_sec, tv.tv_usec/1000,
			-46.85 + 175.72 * read_rht(i2c, 0xe3),
			 -6    + 125    * read_rht(i2c, 0xe5)
		);

	return 0;

}

