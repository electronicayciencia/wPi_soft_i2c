test: soft_i2c.c test.c
	gcc -o test -lwiringPi soft_i2c.c test.c -I.

scan: soft_i2c.c scan.c
	gcc -o scan -lwiringPi soft_i2c.c scan.c -I.

24lc128: soft_i2c.c 24lc128.c
	gcc -o 24lc128 -lwiringPi soft_i2c.c 24lc128.c -I.

pcf8591: soft_i2c.c pcf8591.c
	gcc -o pcf8591 -lwiringPi soft_i2c.c pcf8591.c -I.

i2cli: soft_i2c.c i2cli.c
	gcc -o i2cli -lwiringPi soft_i2c.c i2cli.c -I.

all: test scan 24lc128 pcf8591 i2cli

