# Software emulated I2C for Raspberry Pi

This is a basic software implementation of I2C protocol using WiringPi library for GPIO.

## Getting Started

You will need to install Gordon's WiringPi library first. 

Download soft_i2c.c and soft_i2c.h and place them in your project directory.

If you are willing to use pins 8 and 9, notice these are hardware enabled I2C pins. So please make sure you unloaded i2c kernel modules before using these routines.

```
rmmod i2c_dev 
rmmod i2c_bcm2708
```

### Example

See file [pcf8591.c](pcf8591.c) for a quick start guide.


## Usage

Include headers file in your main program:
```
#include "soft_i2c.h"
```

Since this is not a library, you need to add the source file to gcc compile line.
```
gcc -lwiringPi soft_i2c.c pcf8591.c -o pcf8591
```

### Functions

#### i2c_t i2c_init(int scl, int sda);

Initializes a new i2c bus. SCL and SDA parameters are SCL and SDA pins. Use WiringPi numeration. 
It returns a new i2c structure and set the indicated pins as inputs with pull-up enabled.

```
i2c_t my_bus1 = i2c_init(10, 11);
```

#### void i2c_start(i2c_t port);

#### void i2c_stop(i2c_t port);

#### void i2c_send_bit(i2c_t port, int bit);

#### int i2c_read_bit(i2c_t port);

#### int i2c_send_byte(i2c_t port, uint8_t byte);

#### uint8_t i2c_read_byte(i2c_t port);


## Known bugs and limitations

* Delays are CPU loops.
* Low speed, 100kHz frequency.
* Only master mode.
* Does not check the bus before start.
* Clock stretching is not implemented.

## See also

[El bus I2C a bajo nivel](http://electronicayciencia.blogspot.com/) (Spanish). Not yet.

## Authors

* **Reinoso Guzmán** - *Initial work* - [Electronica y Ciencia](http://electronicayciencia.blogspot.com/)

See also the list of [contributors](https://github.com/electronicayciencia/wPi_soft_i2c/graphs/contributors) who participated in this project.

## License

This is free and unencumbered software released into the public domain. See the [LICENSE.md](LICENSE.md) file for details.

