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


void commands_help (void) { 
	printf(
		"Valid commands are:\n"\
		"   s: start\n"
		"   p: stop\n"\
		"   R: Reset bus\n"\
		"   a: send ack\n"\
		"   n: send nak\n"\
		"   wHH: write byte HH\n"\
		"   r: read byte\n"\
		"   txxx: wait xxx ms\n"\
		"   C: sCan\n"\
		"   q: quit\n"\
		"\n"\
	);
}

void prompt () {
	printf("i2cli> ");
}


int main (int argc, char **argv)
{
	int interactive = 0;

	if (isatty(fileno(stdin))) interactive = 1;

	if (interactive) puts ("I2C Command Line Interface");

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
	
	if (interactive) printf("I2C ready. SCL: %d, SDA: %d\n", scl, sda);
	
	// Start CLI
	if (interactive) commands_help();
	if (interactive) prompt();

	char cmd[10];
	char byte;
	long ms;
	int terminate = 0;
	int addr;
	
	while (!terminate && fgets(cmd, sizeof cmd, stdin)) {

		switch (cmd[0]) {
			case '\'':
			case ';':
			case '%':
			case '#':
			case '/':
				// comment line, do nothing
				break;

			case 'q':
				terminate = 1;
				break;

			case 's':
				i2c_start(i2c);
				break;

			case 'p':
				i2c_stop(i2c);
				break;

			case 'R':
				i2c_reset(i2c);
				break;

			case 'a':
				i2c_send_bit(i2c, I2C_ACK);
				break;

			case 'n':
				i2c_send_bit(i2c, I2C_NACK);
				break;

			case 'w':
				byte = strtol(cmd + 1, NULL, 16);
				printf("%02x -> ", byte);
				if(i2c_send_byte(i2c, byte) == I2C_ACK)
					puts("ACK");
				else
					puts("NACK");
				break;

			case 'r':
				printf("r -> %02x\n", i2c_read_byte(i2c));
				break;

			case 'C':
				for (addr = 0; addr < 128; addr++) {
					i2c_start(i2c);
					// Some devices best scanned using read, other using write
					if (i2c_send_byte(i2c, addr << 1 | I2C_WRITE) == I2C_ACK)
						printf (" * Device found at %02xh  (R: %02x, W: %02x)\n",
							addr,
							addr << 1 | 1,
							addr << 1 | 0);
					i2c_stop(i2c);
				}
				break;
			
			case 't':
				ms = strtol(cmd + 1, NULL, 10);
				printf("wait %dms...\n", ms);
				usleep(ms * 1000);
				break;

			default :
				if (interactive) commands_help();
		}

		if (interactive) prompt();
	}	

	i2c_stop(i2c);
	
	puts("Bye!");

	return 0;
}
