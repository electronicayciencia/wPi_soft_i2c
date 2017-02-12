/*
 * i2cli.c
 *      Prueba de I2C por software usando WiringPi
 *      Reinoso G. 09/02/2017
 *
 *      gcc -lwiringPi soft_i2c.c i2cli.c -o i2cli
 */
 
#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>

#include "soft_i2c.h"


int main (int argc, char **argv)
{
	puts ("I2C Command Line Interface");

	// Check arguments
	if (argc != 3) {
		printf("Usage: %s scl sda\n", argv[0]);
		printf("  scl and sda are SCL and SDA pins in WiringPi numeration.\n");
		return(0);
	}

	int scl = atoi(argv[1]);
	int sda = atoi(argv[2]);

	if (scl == 0 || sda == 0) {
		puts("SCL or SDA pins cannot be 0");
		return(0);
	}
	else if (scl == sda) {
		puts("SCL and SDA pins must be different");
		return(0);
	}

	// Init
	if (wiringPiSetup () == -1)
		return 1;

	i2c_t i2c = i2c_init(scl,sda);
	
	printf("I2C ready. SCL: %d, SDA: %d\n", scl, sda);
	
	// Let's scan
	int addr;
	for (addr = 0; addr < 128; addr++) {
		i2c_start(i2c);
		if (i2c_send_byte(i2c, addr << 1 | I2C_READ) == I2C_ACK)
			printf (" * Device found at %0xh\n", addr);
		i2c_stop(i2c);
	}
	
	// Start CLI
	puts ("\ns: start   p: stop   a: ack   n: nak   wHH: write byte HH   r: read byte   q: quit");
	printf("i2cli> ");

	char cmd[5];
	char byte;
	int terminate = 0;
	
	while (!terminate && fgets(cmd, sizeof cmd, stdin)) {

		switch (cmd[0]) {
			case 'q':
				terminate = 1;
				break;

			case 's':
				i2c_start(i2c);
				break;

			case 'p':
				i2c_stop(i2c);
				break;

			case 'a':
				i2c_send_byte(i2c, I2C_ACK);
				break;

			case 'n':
				i2c_send_byte(i2c, I2C_NACK);
				break;

			case 'w':
				byte = strtol(cmd + 1, NULL, 16);
				if(i2c_send_byte(i2c, byte) == I2C_ACK)
					puts("ACK");
				else
					puts("NACK");
				break;

			case 'r':
				printf("%02x\n", i2c_read_byte(i2c));
				break;

			default :
				puts ("s: start   p: stop   a: ack   n: nak   wHH: write byte HH   r: read byte   q: quit");
		}

		printf("i2cli> ");
	}	

	i2c_stop(i2c);
	
	puts("Bye!");

	return 0;
}
