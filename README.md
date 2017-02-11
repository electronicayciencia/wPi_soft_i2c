# Software emulated I2C for Raspberry Pi

This is a basic software implementation of I2C protocol using WiringPi library for GPIO.

## Getting Started

You will need to install Gordon's WiringPi library first. 

Download soft_i2c.c and soft_i2c.h and place them into your project directory.

If you are willing to use pins 8 and 9, notice these are hardware enabled I2C pins. So please make sure you unloaded i2c kernel modules before using these routines.

```
rmmod i2c_dev 
rmmod i2c_bcm2708
```

### Examples

These are some files that you can use as a quick start guide.

#### [scan.c](scan.c)

This is a I2C scanner. It just tries every I2C address from 0 to 128 and test the acknowledge.

Just modify the *i2c_init* line to set up your ports. Remember to use WiringPi numbers. Run the scan and...

```
$ ./scan
I2C scan for soft_i2c project
 * Device found at 50h
```


#### [pcf8591.c](pcf8591.c)

This is a small software to read PCF8591 ADC/DAC. It setup the device to read channel 1 and 
reads it in continuous mode until the user interrupts.


## Usage

Include headers file in your main program:
```c
#include "soft_i2c.h"
```

Since this is not a library, you need to add the source file to gcc compile line.
```c
gcc -lwiringPi soft_i2c.c pcf8591.c -o pcf8591
```


### Functions

#### i2c_t i2c_init(int scl, int sda);

Initializes a new i2c bus. The parameters SCL and SDA are SCL and SDA pin numbers. Please use WiringPi numeration. 
Returns a new i2c_t structure and set the indicated pins as inputs with pull-up enabled.

You can have as many busses defined as you need.

```c
i2c_t my_bus1 = i2c_init(9, 8);
```

#### void i2c_start(i2c_t bus);

First, wait until both lines goes up, this means the bus is not busy. Then it sends a I2C start signal to the bus
passed as parameter. As a result, it drives both lines low. It does not return anything.

```c
i2c_start(my_bus);
```

#### void i2c_stop(i2c_t bus);

Sends a I2C start signal to the bus passed as parameter. After that, both lines are released. It does not return anything.

```c
i2c_stop(my_bus);
```

#### void i2c_send_bit(i2c_t bus, int bit);

Sends one bit through the bus. If second argument is true, it sends one. Zero in other case. 
Can be used to send an ack from the master. It does not return anything.

This routine is clock stretching aware, it take into account that SCL line should go up before sending more data.

```c
i2c_send_bit(my_bus, 1);
```

You can also use the pre-defined constants:

```c
i2c_send_bit(my_bus, I2C_ACK);
```

Or

```c
i2c_send_bit(my_bus, I2C_NACK);
```


#### int i2c_read_bit(i2c_t bus);

Reads one bit and returns it. It is not much useful on its own but used by i2c_read_byte to receive the acknowledge bit.

```c
status = i2c_read_bit(my_bus);
```

#### int i2c_send_byte(i2c_t bus, uint8_t byte);

Sends 8 bits (1 byte) through the bus passed as argument. Returns I2C_ACK if the slave ack'ed the byte, 
and I2C_NACK in case of error. Remember I2C bytes are sent or received MSB first.

```c
ack = i2c_send_byte(my_bus, address_byte);
```

For example, to test if there is a device with address 0x48 you can do this:

```c
i2c_t my_bus1 = i2c_init(9, 8);

i2c_start(my_bus);

if (i2c_send_byte(my_bus, 0x48 << 1 | I2C_READ) == I2C_ACK)
	puts("Device found!");

i2c_stop(my_bus);
```

Note the use of I2C_READ and I2C_WRITE to indicate the bus operation.


#### uint8_t i2c_read_byte(i2c_t bus);

Reads a byte from the bus and returns it. But NOT ack the byte. You must ack the byte using i2c_send_bit as you wish.
Remember I2C bytes are sent or received MSB first.

```c
int byte = i2c_read_byte(i2c);

if (we_liked(byte)) {
	i2c_send_bit(i2c, I2C_ACK); // 0 is a positive ack
	printf("We read: %d\n", byte);
}
else {
	i2c_send_bit(i2c, I2C_NACK); // 1 is NOT acknowledge
	printf("We didn't like the byte.\n");
}
```

## Known bugs and limitations

* Currently only bits and 8 bit registers are supported.
* Delays are CPU loops.
* Low speed, 100kHz frequency only.
* Only master mode.

## See also

[El bus I2C a bajo nivel](http://electronicayciencia.blogspot.com/) (Spanish). Extensive description of this code in spanish. Not yet

## Authors

* **Reinoso Guzmán** - *Initial work* - [Electronica y Ciencia](http://electronicayciencia.blogspot.com/)

See also the list of [contributors](https://github.com/electronicayciencia/wPi_soft_i2c/graphs/contributors) who participated in this project.

## License

This is free and unencumbered software released into the public domain. See the [LICENSE.md](LICENSE.md) file for details.

