F_CPU = 16000000
DEVICE = atmega328

AVRDUDE=avrdude -F -V
PROTOCOL=usbtiny
PORT=usb
BAUD=115200
PART=ATMEGA328
PROG=atmega_tree_final
SUDO=sudo

# Tools:
CC = avr-gcc

LIB = light_ws2812
DEP = /home/facelessloser/Dropbox/atmega/libraries/Light_WS2812/ws2812_config.h /home/facelessloser/Dropbox/atmega/libraries/Light_WS2812/light_ws2812.h

CFLAGS = -g2 -I. -I/home/facelessloser/Dropbox/atmega/libraries/Light_WS2812/ -mmcu=$(DEVICE) -DF_CPU=$(F_CPU) -std=gnu99
CFLAGS+= -Os -ffunction-sections -fdata-sections -fpack-struct -fno-move-loop-invariants -fno-tree-scev-cprop -fno-inline-small-functions  
CFLAGS+= -Wall -Wno-pointer-to-int-cast

LDFLAGS = -Wl,--relax,--section-start=.text=0,-Map=main.map

all:	$(PROG) 

$(LIB): $(DEP)
	@echo Building Library 
	@$(CC) $(CFLAGS) -o ./$@.o -c /home/facelessloser/Dropbox/atmega/libraries/Light_WS2812/$@.c 

$(PROG): $(LIB) 
	@echo Building $@
	@$(CC) $(CFLAGS) -o ./$@.o ./$@.c /home/facelessloser/Dropbox/atmega/libraries/Light_WS2812/$^.c
	@avr-size ./$@.o
	@avr-objcopy -j .text  -j .data -O ihex ./$@.o $@.hex
	@avr-objdump -d -S ./$@.o >./$@.lss

	${SUDO} $(AVRDUDE) -c $(PROTOCOL) -p $(PART) -P $(PORT) \
                -b $(BAUD) -U flash:w:${PROG}.hex
.PHONY:	clean

clean:
	rm -f *.hex *.o *.lss
