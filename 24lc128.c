/*
 * 24lc128.c
 *      Prueba de I2C por software usando WiringPi
 *      Reinoso G. 09/02/2017
 *
 *      gcc -lwiringPi soft_i2c.c 24lc128.c -o 24lc128
 */
 
#include <stdio.h>
#include <wiringPi.h>

#include "soft_i2c.h"


int main (void)
{


	puts ("EEPROM reader soft_i2c example");

	if (wiringPiSetup () == -1)
		return 1;

	i2c_t i2c = i2c_init(23,24);

	i2c_start(i2c);
	
	if (i2c_send_byte(i2c, 0x50 << 1 | I2C_WRITE) == I2C_ACK) {
		puts("Device present and ready");
	}
	else {
		puts("Device not found.");
		return(1);
	}


	// Write string at address 0000
	puts("Write step");

	int i;
	char string[] = "En un lugar de la mancha...";
	
	i2c_send_byte(i2c, 0);
	i2c_send_byte(i2c, 0);

	for (i = 0; i < sizeof string; i++) {
		printf("%c", string[i]);
		i2c_send_byte(i2c, string[i]);
	}

	i2c_stop(i2c);

	// Write cycle time is 5ms typical
	delayMicroseconds(5000);

	
	// Read some bytes back
	puts("\n\nRead step");

	i2c_start(i2c);

	if (i2c_send_byte(i2c, 0x50 << 1 | I2C_WRITE) == I2C_ACK) {
		puts("Device is ready");
	}
	else {
		puts("Device is still writing.");
		return(1);
	}


	i2c_send_byte(i2c, 0);
	i2c_send_byte(i2c, 0);

	i2c_start(i2c);

	
	if (i2c_send_byte(i2c, 0x50 << 1 | I2C_READ) == I2C_ACK)
		puts("Read command ack");

	for (i = 0; i < sizeof string; i++) {
		int byte = i2c_read_byte(i2c);
		printf("%c ", byte);
		i2c_send_bit(i2c, I2C_ACK);
	}

	// Must not ack the last byte to stop the streaming
	i2c_read_byte(i2c);
	i2c_stop(i2c);

	puts("");

	return 0;
}
