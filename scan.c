/*
 * scan.c
 *      Prueba de I2C por software usando WiringPi
 *      Reinoso G. 09/02/2017
 *
 *      gcc -lwiringPi soft_i2c.c scan.c -o scan
 */
 
#include <stdio.h>
#include <wiringPi.h>

#include "soft_i2c.h"


int main (void)
{
	int addr;

	puts ("I2C scan for soft_i2c project");

	if (wiringPiSetup () == -1)
		return 1;

	i2c_t i2c = i2c_init(23,24);
	//i2c_t i2c = i2c_init(9,8);

	for (addr = 0; addr < 128; addr++) {
		i2c_start(i2c);
		if (i2c_send_byte(i2c, addr << 1 | I2C_READ) == I2C_ACK)
			printf (" * Device found at %0xh\n", addr);
		i2c_stop(i2c);
	}


	return 0;
}
