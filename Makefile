SDCC ?= sdcc
STCCODESIZE ?= 4089
SDCCOPTS ?= --std-c99 --code-size $(STCCODESIZE) --xram-size 0 --data-loc 0x30 
FLASHFILE ?= main.hex

SRC = src/lcd.c src/key.c src/utils.c

OBJ=$(patsubst src%.c,build%.rel, $(SRC))

all: main

build/%.rel: src/%.c src/%.h
	mkdir -p $(dir $@)
	$(SDCC) $(SDCCOPTS) -o $@ -c $<

main: $(OBJ)
	$(SDCC) -o build/ src/$@.c $(SDCCOPTS) $(CFLAGS) $^
	@ tail -n 5 build/main.mem | head -n 2
	@ tail -n 1 build/main.mem
	cp build/$@.ihx $@.hex
	
eeprom:
	sed -ne '/:..1/ { s/1/0/2; p }' main.hex > eeprom.hex

clean:
	rm -f *.ihx *.hex *.bin
	rm -rf build/*

cpp: SDCCOPTS+=-E
cpp: main
