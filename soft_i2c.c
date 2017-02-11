/*
 * soft_i2c
 *      This is a basic software implementation of I2C protocol 
 *      using WiringPi library GPIO.
 *
 *      Reinoso G. 09/02/2017
 */
 
#include <wiringPi.h>
#include <stdint.h>
#include <stdio.h>

#include "soft_i2c.h"


/* Pull: drives the line to level LOW */
void _i2c_pull(int pin) {
	pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
	delayMicroseconds((1e6/I2C_FREQ)/2);
}

/* Release: releases the line and return line status */
void _i2c_release(int pin) {
	pinMode(pin, INPUT);
	delayMicroseconds((1e6/I2C_FREQ)/2);
}

/* In case of clock stretching or busy bus we must wait */
void _i2c_release_wait(int pin) {
	pinMode(pin, INPUT);
	delayMicroseconds((1e6/I2C_FREQ)/2);
	while (!digitalRead(pin))
		delayMicroseconds((1e6/I2C_FREQ)/2);
	delayMicroseconds((1e6/I2C_FREQ)/2);
}

/* Initializes software emulated i2c */
i2c_t i2c_init(int scl, int sda) {
	i2c_t port;
	
	pinMode(scl, INPUT);
	pinMode(sda, INPUT);
	pullUpDnControl(scl, PUD_UP);
	pullUpDnControl(sda, PUD_UP);

	port.scl = scl;
	port.sda = sda;

	return port;
}

/* Start: pull SDA while SCL is up*/
/* Best practice is to ensure the bus is not busy before start */
void i2c_start(i2c_t port) {
    _i2c_release_wait(port.sda);
    _i2c_release_wait(port.scl);
	_i2c_pull(port.sda);
	_i2c_pull(port.scl);
}

/* Stop: release SDA while SCL is up */
void i2c_stop(i2c_t port) {
	_i2c_release_wait(port.scl);
	_i2c_release_wait(port.sda);
}

/* Sends 0 or 1: 
 * Clock down, send bit, clock up, wait, clock down again 
 * In clock stretching, slave holds the clock line down in order
 * to force master to wait before send more data */
void i2c_send_bit(i2c_t port, int bit) {
	if (bit)
		_i2c_release(port.sda);
	else
		_i2c_pull(port.sda);
		
	_i2c_release_wait(port.scl);
	_i2c_pull(port.scl);
}

/* Reads a bit from sda */
int i2c_read_bit(i2c_t port) {
	int s;

	_i2c_release(port.sda);
	_i2c_release(port.scl);
	s = digitalRead(port.sda);
	_i2c_pull(port.scl);
	
	return s;
}

/* Sends 8 bit in a row, MSB first and reads ACK.
 * Returns 1 if device ack'ed */
int i2c_send_byte(i2c_t port, uint8_t byte) {
	int i;

	for (i = 0; i < 8; i++) {
		i2c_send_bit(port, byte & 0x80);
		byte = byte << 1;
	}

	return !i2c_read_bit(port);
}

/* Reads a byte, MSB first */
uint8_t i2c_read_byte(i2c_t port) {
	int byte = 0x00;
	int i;

	for (i=0; i<8; i++) 
		byte = (byte << 1) | i2c_read_bit(port);

	return byte;
}


