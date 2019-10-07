# STC DIY Calculator Firmware

This is a replacement firmware for the [diyleyuan calculator kit](http://www.diyleyuan.com/jc/L8Q.html). The calculator kit is available for purchase for less than $13 shipped from eBay by searching for "diy calculator kit". You will have to solder the kit yourself (see "hardware connections" below). The calculator uses an STC IAP15W413AS microcontroller (an 8051 instruction set-compatible microcontroller) with a built-in serial-port bootloader. This project uses SDCC to compile the C code and [stcgal](https://github.com/grigorig/stcgal) to load the new firmware.

The replacement firmware supports floating-point calculations (using 18 decimal digits plus exponent for arithmetic) with a 4-level RPN stack. Functions include basic arithmetic as well as log(), exp(), y^x, 1/x and sqrt(), all in floating point. (The original firmware supported only fixed-point calculations in chain mode.) I have not added in the resistor value calculator or the decimal/hexadecimal converter features from the original firmware.

Note that once you change the firmware on the calculator, there's no way to go back to the original firmware (the original firmware isn't posted for download anywhere). STC's bootloader on the microcontroller deliberately prevents readback of the microcontroller's content, and STC considers this to be a "feature".

Here's a picture of the assembled calculator kit running the new firmware (it's impossible to keep the glossy black acrylic clean):

![calculator](./calc.jpg)

And here's a picture of the GUI emulator with functions labeled:

![Qt GUI](./qt_gui.png)

# Building
- Use the Makefile for building a new firmware for the calculator.
	- type `make` to build
		- (you must already have SDCC installed and set up so it can be found in your PATH)
		- this will create a `main.hex` output file
	- there is also a prebuilt binary checked in at `binaries/main.hex`
- CMakeLists.txt is for building the Qt desktop application, and also the decimal-number-library test application.
	- build similarly to other cmake projects:
		- `mkdir build_qt && cd build_qt`
		- `cmake -DCMAKE_BUILD_TYPE=Debug -G "Eclipse CDT4 - Ninja" ..`
			- (you can choose a different generator, I prefer using Ninja to build, because it's fast)
		- `ninja`

# Installing
Note that once you change the firmware on the calculator, it isn't possible to go back to the original firmware. The STC microcontroller used has a bootloader permanently stored in ROM that allows downloading new firmware over a serial port (but not reading the existing firmware). You can re-program it using a USB-to-logic-level-serial (5V) dongle, and the stcgal program. WARNING: a lot of USB-to-logic-level-serial dongles are for 3.3V logic levels instead of the 5V needed. Also note that this is a "logic-level" serial port, and not RS232 levels, which are generally a high negative voltage. The diyleyuan calculator runs at 5V to make it easier to power/drive the LCD display. You have a couple of options:

1. Buy a USB to logic-level serial dongle that supports 5V operation (these dongles may have a jumper you need to set to switch between 3.3V and 5V). This is the best option.
	- Here is one that works: https://www.amazon.com/gp/product/B00N4MCS1A/
1. If you have an adjustable power supply, power the diyleyuan calculator at 3.3-3.5V for programming, instead of 5V, and use a 3.3V USB-to-logic-level-serial dongle.
	- You won't be able to see the LCD screen at 3.3V well, though.
	- After programming, remove the USB dongle, and set the voltage back up to 5V to make the LCD readable.
	- This is an ok option, but is cumbersome to switch between 3.3V and 5V, and requires that you have an adjustable power supply.
	- Also you run the risk of forgetting to switch between the 2 voltages.
1. Run the calculator at 5V, and use a 3.3V USB dongle anyways.
	- NOT RECOMMENDED, don't do this
	- this violates the absolute maximum ratings for most of these USB to 3.3V logic level dongles, and may damage either the dongle or your computer.

## Hardware connections
- Pin 15 of the STC microcontroller is Rx to the microcontroller (green wire in the picture).
Connect to Tx of the USB dongle.
- Pin 16 is Tx from the microcontroller (purple wire in the picture). Connect to Rx of the USB dongle.

I recommend soldering Tx and Rx wires into the plated through holes on the PCB while soldering up the kit, so that the connections are more permanent. (In the picture, I just soldered the wire directly to the microcontroller pins, but this isn't as reliable as if they were soldered into a through hole.) Note that you must "cross over" Tx/Rx going between the microcontroller and the USB dongle (i.e. Rx on the microcontroller goes to Tx on the USB dongle, and Tx on the microcontroller goes to Rx on the USB dongle). You must also connect ground to the dongle. A good point to use is either pin 1 of U1, or the header P1.

Be careful when working on the calculator. The 7550 voltage regulator used has no short circuit protection. It does have a low quiescent current and extremely low dropout voltage though, and you must match the low dropout voltage if replacing the regulator. If you do end up damaging the regulator (like I did), a good replacement is the Microchip MCP1700-5002E/TO. (In the picture though, I just removed the 7550 voltage regulator, shorted pins 2 and 3, and added a capacitor between pin 1 and pins 2/3. I am powering the calculator externally, instead of with batteries.)

![connections](./connections.jpg)

Here is the schematic from the diyleyuan website. Note that the schematic symbol for the microcontroller mistakenly labels P5.4 as P0.0, and mistakenly labels P5.5 as P0.1. The net name labels are correct.

![schematic](./schematic.gif)

The soft-latching power switch works as follows: Initially the calculator is off. Both Q1 and Q2 are off. Pressing the On key (S4) turns on Q1 through R1 and D2. Q1 then supplies 5V to the system. Once the microcontroller has power and starts running, it turns on Q2, which keeps Q1 on through R1. To turn off, the microcontroller turns off Q2, which in turn will turn off Q1.

Here is the component layout from the diyleyuan website.

![component layout](./component.gif)

The switches used are a knockoff of the Omron B3F series. A good replacement is the B3F-5050 (the switches included with the kit take quite a bit of force to depress). The LCD used is a fairly standard LCD based on a HD44780-compatible controller. The hole spacing for the screw holes on the LCD is 31mm x 75mm. There are many replacements available, including ones that don't need the backlight on to be readable. I recommend a positive FSTN type, although the one included is very usable with the backlight on.

## Programming with stcgal

See below for the stcgal output. Replace `stc_rpncalc/main.hex` with the actual path to the main.hex you built. There are also prebuilt binaries in the `binaries` directory. In this example, I'm programming at a relatively high line rate of 230,400 bits/s. You may want to try at a slower speed first to get things working (omit the `-b 230400` option).

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

(The name for `stcgal` is probably a play on words from the `avrdude` programming software used to program AVR microcontrollers.)

# Usage
## Calculation
The calculator uses RPN. Calculate (2+3)/(9^2) by pressing the following keys:

- 2
- Enter (=)
- 3
- `+`
- 9
- Enter (=)
- `*`
- `÷`

The = key is used for Enter. There is automatic stack lift so that `9`, `Enter`, `*` is equivalent to 9^2. The stack is a classic 4-level RPN stack, where the T register automatically duplicates.

## Keys
Some of the keys have slightly different functions, see the picture of the emulator Qt GUI.

![Qt GUI](./qt_gui.png)

The keys on the *original* calculator map as follows:

- `=   `: Enter
- `<-  `: Negate (+/-: change sign)
	- Note: for implementation simplicity, this is a postfix operator.
	- Pressing this key will immediately terminate digit entry and negate the number.
	- Acts as sqrt() when shifted
- `sqrt`: Swap `x <-> y`
	- Acts as 1/x when shifted
- `.   `: decimal point/(-)Exp: The `.` key works similarly to old Sinclair Scientific calculators that also have a limited number of keys, and combines entering the decimal point and starting exponent entry (both positive and negative) onto one key:
	- The 1st press inserts a decimal point.
	- The 2nd press begins exponent entry.
	- The 3rd and subsequent presses negates the current exponent being entered.
- `mode `: acts as a shift key
- `ON/AC`: acts as a backspace key during digit entry, acts as `Clear X` when digit entry is finished (e.g. after an operator key is pressed)
	- acts as `Clear X` when shifted
- `7    `: acts as y^x when shifted
- `8    `: acts as ln(x) when shifted
- `9    `: acts as log(x) when shifted
- `5    `: acts as e^x when shifted
- `6    `: acts as 10^x when shifted


## Floating Point
The exponent display is fixed at 2 digits, although the calculator doesn't prevent you from doing certain operations (e.g. basic arithmetic) which results in numbers with larger exponents. (A 10 in the ten's place of the exponent will then be displayed as a '`:`'. This just so happens to be the next character after '`9`' in the 1602 LCD's character map). The calculator can internally represent larger exponents than +/-99, although calculations with larger exponents aren't guaranteed to be correct.

## Turning off
Hold `Shift` (the `mode` key on the physical calculator) and `0` *at the same time* to turn off. NOTE: There is no auto power off.

# Bugs
1. The calculator does not properly check for underflow or overflow sometimes.
1. After division by 0, ln(-), over/underflow, or other operations which give an `Error`, it's possible to still do certain operations on `Error`. Many functions do check, and will not operate on `Error`, but not all of them yet. This is somewhat similar to old soviet Elektronika calculators where `Error` is just a number, and there wasn't enough ROM space to check for errors. There are people who explore the inner-workings of these calculators by manipulating the `Error` number.
1. There are probably more bugs waiting to be discovered.

# Internals
## Number Format
The original firmware that came with this calculator used a fixed-point format, which significantly limited the range of numbers usable. Additionally, the implementation doesn't actually have enough digits for all displayable results, which can cause errors (e.g. `3,162.277*3,162.28` gives `10,000,010` instead of the correct `10,000,005.31156`).

This replacement calculator firmware uses decimal floating point, using base-100 to store numbers and do calculations. Base-100 allows for efficient storage into 8-bit bytes, and is easier to work with than packed-BCD. Unlike straight binary representations, base-100 is still fairly easy to display as decimal. Also unlike binary representations, there is no conversion error from binary/decimal (e.g. numbers like `0.1` can be represented exactly).

Each `uint8_t` stores a base-100 "`digit100`", referred to as an "`lsu`", for least significant unit. (The LSU terminology is borrowed from the decNumber library: I originally considered using the decNumber library similar to the WP-34S calculator, but just the library itself takes several times more flash than is available on this calculator. I also considered using the BigNu mber arduino library, but that library uses C++ and lots of pointers passed to functions, which are extremely expensive on the 8051 architecture.) The number format is as follows:

- `lsu[0]`: contains the most significant `digit100` (the most significant 2 decimal digits)
	- implicit decimal point between `lsu[0]/10` and `lsu[0]%10`
- `lsu[1]` to `lsu[n-1]`: the rest of the array contains the next `digit100`s in order from most to least significant
	- currently there are 9 LSUs, giving an 18 decimal digit significand
- exponent: the bottom 15 bits hold the exponent of the floating point number, the most-significant bit holds the sign
	- the exponent (the bottom 15 bits) is stored directly in 2's complement binary
	- this is a base-10 exponent, not a base-100 exponent
		- allows all digits in the `lsu[]` array to be significant when normalized
		- otherwise numbers such as `1.35` would be stored as `lsu[0]=01`, `lsu[1]=35`, losing 1 digit of representation in `lsu[0]`
		- range of exponents only needs to be `+/-99`
			- using 15 bits for the exponent (instead of e.g. 7 bits) prevents certain intermediate results from prematurely causing overflow

For example, the number `13.5` is stored normalized (with no leading zeros in the representation) as follows:

- `lsu[0]`: 13
- `lsu[1]`: 50
- `lsu[2]` to `lsu[n-1]`: all 0
- exponent: 1

There is an implicit decimal point between the 1 and 3 in `lsu[0]`, so the number is 1.350 * 10^1, which is equivalent to `13.5`. Similarly, the number `1.35` would be stored the exact same way, except now the exponent is 0.

The number `0.135` would be stored the same way, except now the exponent is `0x7FFF` (note that the sign bit is 0, and the bottom 15 bits are the 2's complement representation of -1). The number `-13.5` would be stored the same way, except now the exponent is `0x8001` (the sign bit is now 1 which means the number as a whole is negative, but the exponent itself is positive).

## Arithmetic
- Addition is done the same way as it's done by hand, although in base-100 instead of decimal.
- Subtraction is similarly done similar to how it's done by hand, also in base-100. It uses carries instead of (the more widely taught) borrows using the "equal additions" algorithm.
- Multiplication is done the same way it's done by hand, although in base-100. Partial sums are added up and shifted after each digit, instead of waiting until the very end to sum up all partial sums (as is common when doing multiplication by hand).
- Reciprocals are calculated using a fixed number of Newton-Raphson iterations, and division is calculated by taking the reciprocal of the divisor and multiplying by the dividend.
	- the iteration for 1/x is new_estimate = estimate + (1 - estimate * x)*estimate
	- see `src/decn/proto/div_mfp.cpp` for initial prototyping development work

## Transcendental Functions
- Logarithms are calculated similar to how it's described by the HP Journal article "Personal Calculator Algorithms IV: Logarithmic Functions" by William Egbert.
	- see `src/decn/proto/ln_mfp.cpp` for initial prototyping development work
- Exponentials are calculated similar to the HP 35 algorithm, as described [here](http://www.jacques-laporte.org/expx.htm) using the same constants as the logarithm algorithm.
	- see `src/decn/proto/exp.cpp` for initial prototyping development work
- Powers are calculated using the identity y^x = e^(x*ln(y))
- Square roots are calculated using the identity sqrt(x) = e^(0.5*ln(x))

## TODO
- Rounding: currently, to save code space, there is no rounding being done (even for intermediate steps), and numbers are instead truncated.
- Square roots could be more-accurately implemented using digit-by-digit methods similar to those described in the HP Journal article "Personal Calculator Algorithms I: Square Roots" by William Egbert.
	- calculating using Newton-Raphson iterations for the reciprocal square root 1/sqrt(x), and then multiplying by the original value would probably also be more accurate, and definitely much faster
		- the iteration for the reciprocal square root is new_estimate = 0.5 * estimate * (3 - x * estimate * estimate)
- Reciprocal/division could also be more-accurately implemented using digit-by-digit methods (the Newton-Raphson iterations currently used are quite fast though).
- Trigonometric functions could be implemented with algorithms similar to those described in the HP Journal articles "Personal Calculator Algorithms II: Trigonometric Functions" and "Personal Calculator Algorithms III: Inverse Trigonometric Functions", both by William Egbert.
	- will probably assign to the shifted `1`, `2`, and `3` keys, and `0` for calculating inverse trig functions.
- The stack rotate function is currently unimplemented
	- will probably assign to the shifted `4` key
- STO and RCL are currently unimplemented
	- will probably assign to the shifted `.` and `Enter` (`=`) keys
- Special cases, such as taking the logarithms of numbers near 1, negative number raised to integer powers, etc. could be implemented separately, similar to what is described in the HP Journal note "The New Accuracy: Making 2^3 = 8" by Dennis Harms.
- The display blanking for trailing 0s assumes that 16 digits will actually be displayed, but this might not be the case if the negative sign, decimal point, or exponents are displayed
- Would be nice to have the `hex <=> dec` converter from the original firmware if there is more flash space
- Would be nice to have the resistor color band decoder if there is more flash space

# Key Debouncing
The keyboard matrix is scanned once every 5ms. The keyboard debouncing is based on the quick draw/integrator hybrid algorithm described [here](https://summivox.wordpress.com/2016/06/03/keyboard-matrix-scanning-and-debouncing/). This algorithm combines the advantages of both methods:

1. It signals a key press immediately, the very first instant a keyboard matrix scan detects a key is pressed (similar to the "quick-draw" method).
1. It has an "integrator" to determine both when a key is fully pressed and when a key is fully released. This prevents the mechanically bouncy keys from registering multiple times when pressed.

In practice, the keyboard debouncing works much better than the original firmware (which would occasionally miss keystrokes).

# Implementation on an STC 8051 Microcontroller
This was my 1st time using an 8051 microcontroller. The architecture is a bit limiting for programming in "high-level" languages such as C compared to more modern architectures -- even compared to other 8-bit architectures such as the AVR (used in the arduino). Most significantly, there is no stack-pointer-relative addressing, which makes C functions takes up a lot of code space, since they have to emulate stack-pointer-relative addressing. Unfortunately, the microcontroller used only has 13K of code space. The compiler used (SDCC) also does not support using a 2nd data pointer, even though STC's implementation of the 8051 has one.

I've avoided relying on the functions being able to be re-entrant, so that they do not depend on having a stack. SDCC is *not* set to use `--stack-auto` to reduce code size (this means functions are not re-entrant). Some "large" local variables are declared as static in functions to save on the code space needed to emulate a stack. I used a lot more globals than I what I would typically like to have used, and a lot less pointers passed to functions, since these are extremely expensive (to account for the 3 different memory types).

Another weird thing about the 8051 is that not all of the memory is addressed the same way. On this microcontroller, there are 512 bytes of ram total, of which:

- only 128 bytes can be addressed directly (or indirectly)
	- the start of this address space is also shared with general purpose registers, so you don't actually have the full 128 bytes of directly addressable memory
- (there are also 128 bytes of "special function registers" which can be addressed directly to set up the microcontroller and its peripherals)
- 128 bytes can be addressed only indirectly (used in this firmware for the stack, and intermediate working registers)
- 256 bytes can be addressed as external memory
	- on the original 8051, this memory would have actually been external, but on this microcontroller the "external" ram is built in
	- addressing this memory is more difficult than addressing indirect memory, which is itself more difficult than addressing memory directly

Thus, there are special compiler directives to tell it what address space to place variables in memory. (Even for a simple calculator, there isn't enough directly addressable memory (128 bytes) to store everything.) General-purpose pointers and operations using general-purpose pointers are relatively expensive since the pointers must encode the memory type.


# Licensing
This code is licensed under GPLv3.
