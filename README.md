
# Dependencies

The project loolou depends on the avr-gcc and avrdude.

## Ubuntu

```
$ sudo apt-get install gcc-avr avrdude
```

# Build
## compile

```
$ make
```

## clean
```
$ make clean
```

## flash to attiny2313

```
# use default
# programmer: avrispv2
# usb device: /dev/ttyACM0
$ sudo make burn
```

```
# overwrite defaults
# AVRDUDE_PROGRAMMER override programmer type
# AVRDUDE_PORT overrides usb device
$ sudo make AVRDUDE_PROGRAMMER=avrispv2 AVRDUDE_PORT=/dev/ttyACM0 burn
```


## Useful links

* https://startingelectronics.org/tutorials/AVR-8-microcontrollers/ATtiny2313-tutorial/
* http://www.atmel.com/webdoc/avrlibcreferencemanual/index.html
* http://www.avr-tutorials.com/interrupts/avr-external-interrupt-c-programming

## Datasheet
### Attiny2313

* http://www.atmel.com/Images/Atmel-2543-AVR-ATtiny2313_Datasheet.pdf
