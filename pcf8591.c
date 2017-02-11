/*
 * soft_i2c.c
 *      Prueba de I2C por software usando WiringPi
 *      Reinoso G. 09/02/2017
 *
 *      Motivado por la imposibilidad de leer datos en streaming
 *      del ADC.
 *      I2C no es un protocolo orientado a tiempos sino a reloj por 
 *      lo que no hay problema de sincronización ni de retardos
 *      en el tiempo de espera.
 *
 *      gcc -lwiringPi soft_i2c.c pcf8591.c -o pcf8591
 */
 
#include <stdio.h>
#include <wiringPi.h>

#include "soft_i2c.h"


int main (void)
{

	puts ("Software emulated I2C Test Program for Raspberry Pi");

	if (wiringPiSetup () == -1)
		return 1;

	i2c_t i2c = i2c_init(9,8);

	// Send control register
	i2c_start(i2c);
	i2c_send_byte(i2c, 0x48 << 1 | 0);
	i2c_send_byte(i2c, 1);
	i2c_stop(i2c);

	// Read A/D values
	i2c_start(i2c);
	i2c_send_byte(i2c, 0x48 << 1 | 1);

	while(1) {
		int byte = i2c_read_byte(i2c);
		i2c_send_bit(i2c, 0);
		printf("-%d-\n", byte);
//		delayMicroseconds(100000);
	}

	i2c_stop(i2c);

	puts("");

	return 0;
}
