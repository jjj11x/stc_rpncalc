SDCC ?= sdcc
STCCODESIZE ?= 13312
SDCCOPTS ?= --code-size $(STCCODESIZE) --xram-size 256 --idata-loc 0x80
#SDCCOPTS ?= --code-size $(STCCODESIZE) --xram-size 256 --stack-auto --model-large
FLASHFILE ?= main.hex
LARGE_LDFLAGS += -L/usr/share/sdcc/lib/large/
# CFLAGS += -DSTACK_DEBUG -DSHOW_STACK # write the stack pointer to P3_4

SRC = src/lcd.c src/key.c src/utils.c src/decn/decn.c src/calc.c src/stack_debug.c

OBJ=$(patsubst src%.c,build%.rel, $(SRC))

all: main

build/%.rel: src/%.c src/%.h
	mkdir -p $(dir $@)
	$(SDCC) $(SDCCOPTS) $(CFLAGS) -o $@ -c $<

main: $(OBJ)
	$(SDCC) -o build/ src/$@.c $(SDCCOPTS) $(CFLAGS) $^
#	$(SDCC) -o build/ src/$@.c $(SDCCOPTS) $(CFLAGS) $(LARGE_LDFLAGS) $^
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
