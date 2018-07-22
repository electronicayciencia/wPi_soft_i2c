/*
 * soft_i2c
 *      This is a basic software implementation of I2C protocol 
 *      using WiringPi library GPIO.
 *
 *      https://github.com/electronicayciencia/wPi_soft_i2c
 *
 *      Reinoso G.
 */

#ifndef _SOFT_I2C_H
#define _SOFT_I2C_H

#include <stdint.h>

#define I2C_READ   1
#define I2C_WRITE  0
#define I2C_ACK    0
#define I2C_NACK   1

#define I2C_FREQ 100000    /* I2C clock max frequency 100kHz */
#define WARN     1         /* print warning message via stderr */

typedef struct {
	int scl;
	int sda;
} i2c_t;


/* Pull: drives the line to level LOW */
void _i2c_pull(int pin);

/* Release: releases the line */
int _i2c_release(int pin);

/* Release_wait: releases the line and wait until it goes up */
void _i2c_release_wait(int pin);

/* Initializes software emulated i2c */
i2c_t i2c_init(int scl, int sda);

/* Start: pull SDA while SCL is up*/
/* Best practice is test the bus before start */
void i2c_start(i2c_t port);

/* Stop: release SDA while SCL is up */
void i2c_stop(i2c_t port);

/* Reset: software reset bus sequence */
void i2c_reset(i2c_t port);

/* Sends 0 or 1: 
 * Clock down, send bit, clock up, wait, clock down again */
void i2c_send_bit(i2c_t port, int bit);

/* Reads a bit from sda */
int i2c_read_bit(i2c_t port);

/* Sends 8 bit in a row, MSB first and reads ACK.
 * Returns I2C_ACK if device ack'ed */
int i2c_send_byte(i2c_t port, uint8_t byte);

/* Reads a byte, MSB first */
uint8_t i2c_read_byte(i2c_t port);

#endif
