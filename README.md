# STC DIY Calculator Firmware

This is a replacement firmware for the diyleyuan calculator using the STC IAP15W413AS microcontroller (8051-compatible). This firmware replaces the pre-programmed firmware with a RPN calculator firmware.
![calculator](./calc.jpg)

# Building
- Makefile is for building the project for the actual calculator using SDCC.
	- type `make` to build
	- this will create a main.hex file
- CMakeLists.txt is for building the Qt desktop application.

# Installing
The STC microcontroller used has a bootloader permanently built-in to rom that allows downloading new firmware over a serial port. You can re-program it using a USB to logic level (5V) serial dongle, and the stcgal program. WARNING: a lot of USB to logic level serial dongles are for 3.3V logic levels. The diyleyuan calculator runs at 5V to make it easier to power/drive the LCD display. You have a couple of options:

1. get a USB to logic level serial dongle that supports 5V operation (there may be a jumper you have to set to switch between 3.3V and 5V). This is the best option.
	- here is one that works: https://www.amazon.com/gp/product/B00N4MCS1A/
1. if you have an adjustable power supply, power the diyleyuan calculator at 3.3-3.5V for programming, instead of 5V, and use a 3.3V USB to logic level serial dongle.
	- You won't be able to see the LCD screen at 3.3V well.
	- After programming, remove the USB dongle, and set the voltage back up to 5V.
	- This is an ok option, but is cumbersome to switch between 3.3V and 5V, and requires that you have an adjustable power supply.
	- Also you run the risk of forgetting to switch between the 2 voltages.
1. run the calculator at 5V, and use a 3.3V USB dongle anyways.
	- NOT RECOMMENDED, don't do this
	- this violates the absolute maximum ratings for most of these USB to 3.3V logic level dongles, and may damage them or your computer

## Hardware connections
- Pin 15 of the STC microcontroller is RX to the microcontroller (green wire in the picture).
- Pin 16 is TX from the microcontroller (purple wire in the picture).
You must "cross over" TX/RX going between the microcontroller and the USB dongle (i.e. RX on the microcontroller goes to TX on the USB dongle, and TX on the microcontroller goes to RX on the USB dongle).
![connections](./connections.jpg)


## Programming with stcgal

See below, for the stcgal output. Replace stc_rpncalc/main.hex with the actual path to the main.hex build output. This example is done at a high line rate of 230,400 bits/s. You may want to try at a slower speed 1st to get things working (omit the `-b 230400` option).

~~~~
$ ./stcgal.py -P stc15 -b 230400 stc_rpncalc/main.hex
Waiting for MCU, please cycle power: done
Target model:
  Name: IAP15W413AS
  Magic: F562
  Code flash: 13.0 KB
  EEPROM flash: 0.0 KB
Target frequency: 11.567 MHz
Target BSL version: 7.2.5T
Target wakeup frequency: 37.261 KHz
Target options:
  reset_pin_enabled=False
  clock_source=internal
  clock_gain=high
  watchdog_por_enabled=False
  watchdog_stop_idle=True
  watchdog_prescale=256
  low_voltage_reset=True
  low_voltage_threshold=3
  eeprom_lvd_inhibit=True
  eeprom_erase_enabled=False
  bsl_pindetect_enabled=False
  por_reset_delay=long
  rstout_por_state=high
  uart2_passthrough=False
  uart2_pin_mode=normal
  cpu_core_voltage=unknown
Loading flash: 13307 bytes (Intel HEX)
Trimming frequency: 11.597 MHz
Switching to 230400 baud: done
Erasing flash: done
Writing flash: 13376 Bytes [00:03, 4046.93 Bytes/s]
Finishing write: done
Setting options: done
Target UID: F56201500C1752
Disconnected!
~~~~

The name for `stcgal` is probably a play on words of the `avrdude` programming software used to program AVR microcontrollers.

# Usage
The calculator uses RPN notation. To calculate (2+3)/(9^2), enter:

- 2
- Enter (=)
- 3
- +
- 9
- Enter (=)
- *
- ÷

There is automatic stack lift so that `9`, `Enter`, `*` is equivalent to 9^2.

The keys on the original calculator map as follows:

- =: Enter
- <-: Negate (+/-: change sign)
	- Note: for implementation simplicity, this is a postfix operator
	- pressing this key will terminate digit entry and immediately negate the number
- sqrt: Swap `x <-> y`
- .: ./(-)Exp: The `.` key works similarly to old Sinclair Scientific calculators that have a limited number of keys:
	- The 1st press inserts a decimal point.
	- The 2nd press begins exponent entry.
	- The 3rd and subsequent presses negates the current exponent being entered.

# Bugs
1. Currently, when displaying numbers, the exponent may be cut off if the number is too long.
1. Currently, trying to display numbers between `[0.1, 1)` causes the calculator to crash.
1. There are probably more bugs waiting to be discovered.

# Internals
## Number Format
The original firmware that came with this calculator used a fixed point format, which limited the range of numbers. Also, the implementation was slightly buggy (e.g. `3,162.277*3,162.28` gives `10,000,010` instead of the correct `10,000,005.31156`).

This calculator firmware uses decimal floating point, using base-100 to store numbers and do calculations. Base-100 allows efficient storage into 8-bit bytes, and is easier to work with than packed-BCD. Unlike binary representations, base-100 is still fairly easy to display in decimal. Also unlike binary representations, there is no conversion error from binary/decimal (e.g. numbers like `0.1` can be represented exactly).

Each `uint8_t` stores a base-100 "`digit100`", referred to as an "`lsu`" (least significant unit: the terminology is borrowed from the decNumber library). The format is as follows:

- `lsu[0]`: contains the most signifcant `digit100` (the most signifcant 2 decimal digits)
	- implicit decimal point between `lsu[0]/10` and `lsu[0]%10`
- `lsu[1-n]`: the reset of the array contains the next `digit100`s in order from most to least signifcant
- exponent: holds the exponent of the floating point number
	- stored directly in 2's complement binary
	- this is a base-10 exponent, not a base-100 exponent
		- allows all digits in the `lsu[]` array to be significant when normalized
		- otherwise numbers such as `1.35` would be stored as `lsu[0]=01`, `lsu[1]=35`, losing 1 digit of representation in `lsu[0]`
		- range of exponents only needs to be `+/-99`

For example, the number `13.5` is stored normalized (with no leading zeros in the representation) as follows:

- `lsu[0]`: 13
- `lsu[1]`: 50
- `lsu[2-n]`: all zero
- exponent: 1

There is an implicit decimal point between the 1 and 3 in `lsu[0]`, so the number is 1.350 * 10^1, which is equivalent to 13.5. Similarly, the number 1.35 would be stored the exact same way, except now the exponent is 0.

## Arithmetic
- Addition is done the same way as it's done by hand, although in base-100 instead of decimal.
- Subtraction is similarly done similar to how it's done by hand, although with carries instead of borrows using the "equal additions" algorithm, and also in base-100.
- Multiplication is done the same way it's done by hand, although in base-100. Partial sums are added up and shifted after each digit, instead of waiting until the very end to sum up all partial sums, as is common when doing multiplication by hand.
- Division is done by newton's method.

## Rounding
Currently, to save code space, there is no rounding being done, and numbers are instead truncated.

# Implementation
This was my 1st time using an 8051 microcontroller. The architecture is a bit limiting for programming in C compared to more modern architectures -- even compared to other 8-bit microcontrollers such as the AVR. Most significantly, there is no stack-pointer relative addressing, which makes C functions takes up a lot of code space, since they must emulate stack-pointer relative addressing. Unfortunately, the microcontroller used only has 13K of code space, which is pretty limiting. It's almost completely full just doing a 4 function decimal floating point calculator.

I've avoided relying on the functions being able to be reentrant, so that they do not depend on having a stack. Some "large" variables are declared as static in functions to save on the code space needed to emulate a stack.

Another weird thing about the 8051 is that not all of the memory is addressed the same way. On this microcontorller, there are 512 bytes of ram total, of which:

- only 128 bytes of it can be addressed directly
	- the start of this address space is also shared with general purpose registers
- (there are also 128 bytes of "special function registers" which can be addressed directly to set up the microcontroller and its peripherals)
- 128 bytes of it can be addressed indirectly (used in this firmware for the stack)
- 256 bytes of it can be addressed as external memory
	- on the original 8051 it would have actually been external, although on this microcontroller, the "external" ram is built in

Thus, there are special compiler directives to tell it where to place things in memory. Even for a simple calculator, there isn't enough directly addressable memory for everything.

Currently all of the code space is full, although there are ways to free some of it up. This would involve changing the code and build to not use `--stack-auto`. It might be possible to add back in square root (using newton's method), maybe logarithms/exponents (and thus arbitrary powers including square roots), and maybe if there's still space left, the original resistor value calculator and the decimal/hex converter.
