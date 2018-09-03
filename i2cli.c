/*
 * i2cli.c
 *      Prueba de I2C por software usando WiringPi
 *      Reinoso G. 09/02/2017
 *
 *      gcc -o i2cli -lwiringPi -lreadline soft_i2c.c i2cli.c
 */
 
#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>

#include <readline/readline.h>
#include <readline/history.h>

#define HISTFILE ".i2cli_history"

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
		"   rxx: read xx bytes\n"\
		"   txxx: wait xxx ms\n"\
		"   C: sCan\n"\
		"   q: quit\n"\
		"\n"\
	);
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
	
	// Start CLI
	if (interactive) {
		printf("I2C ready. SCL: %d, SDA: %d\n", scl, sda);
		commands_help();
		read_history(HISTFILE);
	}

	long ms;
	int addr;
	int terminate = 0;
	int active = 0;
	
	while (!terminate) {
		char prompt[20];
		snprintf(prompt, sizeof(prompt), "i2cli%s> ", active ? "*" : " ");
		char *cmd = readline(prompt);

		if (!cmd)
			break;

		switch (cmd[0]) {
			case '\'':
			case ';':
			case '%':
			case '#':
			case '/':
				// comment line, do nothing
				break;

			case 'q':
				if (interactive) add_history(cmd);
				terminate = 1;
				break;

			case 's':
				if (interactive) add_history(cmd);
				i2c_start(i2c);
				active = 1;
				break;

			case 'p':
				if (interactive) add_history(cmd);
				if (!active) {
					printf("Already stopped.\n");
					break;
				}
				i2c_stop(i2c);
				active = 0;
				break;

			case 'R':
				if (interactive) add_history(cmd);
				i2c_reset(i2c);
				active = 0;
				break;

			case 'a':
				if (interactive) add_history(cmd);
				i2c_send_bit(i2c, I2C_ACK);
				break;

			case 'n':
				if (interactive) add_history(cmd);
				i2c_send_bit(i2c, I2C_NACK);
				break;

			case 'w':
				if (cmd[1] == 0) {
					if (interactive) commands_help();
					break;
				}

				if (interactive) add_history(cmd);
				
				if (!active) {
					printf("Bus in idle state. Try start command.\n");
					break;
				}

				char byte = strtoul(cmd + 1, NULL, 16);
				printf("%02x -> ", byte);
				
				if(i2c_send_byte(i2c, byte) == I2C_ACK)
					puts("ACK");
				else
					puts("NACK");
				break;

			case 'r':
				if (interactive) add_history(cmd);
				if (!active) {
					printf("Bus in idle state. Try start command.\n");
					break;
				}

				if (cmd[1] != 0) {
					int n = strtoul(cmd+1, NULL, 10);
					if (n < 1) 
						break;

					printf("r ->");
					printf(" %02x", i2c_read_byte(i2c));
					
					int i;
					for (i = 1; i < n; i++) {
						i2c_send_bit(i2c, I2C_ACK);
						printf(" %02x", i2c_read_byte(i2c));
					}
					i2c_send_bit(i2c, I2C_NACK);
					printf("\n");
				}

				else {
					printf("r -> %02x\n", i2c_read_byte(i2c));
				}
				break;

			case 'C':
				if (interactive) add_history(cmd);
				if (active) {
					printf("Bus is still active. Try stop command.\n");
					break;
				}
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
				if (interactive) add_history(cmd);
				ms = strtol(cmd + 1, NULL, 10);
				printf("wait %dms...\n", ms);
				usleep(ms * 1000);
				break;

			default :
				if (interactive) commands_help();
		}

		free(cmd);
	}	

	i2c_stop(i2c);
	
	if (interactive) {
		write_history(HISTFILE);
		puts("Bye!");
	}

	return 0;
}
