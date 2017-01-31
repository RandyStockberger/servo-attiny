#
# Simple makefile for avr-gcc projects
#
# button.c led.c rom.c servo.c servoturnout.c
# button.h led.h rom.h servo.h servoturnout.h
#
PROG = servoturnout
MCU = attiny4313
COPT = -Os -std=c11

all:	$(PROG).elf $(PROG).hex size

$(PROG).hex:		$(PROG).elf
	avr-objcopy -j .text -j .data -O ihex $(PROG).elf $(PROG).hex

servoturnout.elf:	$(PROG).o button.o led.o rom.o servo.o
	avr-gcc -g -mmcu=$(MCU) -o $(PROG).elf $(PROG).o button.o led.o rom.o servo.o

servoturnout.o:		$(PROG).c $(PROG).h servo.h button.h led.h rom.h
	avr-gcc -g $(COPT) -mmcu=$(MCU) -c $(PROG).c

button.o:	button.c button.h servoturnout.h
	avr-gcc -g $(COPT) -mmcu=$(MCU) -c button.c

led.o:		led.c led.h
	avr-gcc -g $(COPT) -mmcu=$(MCU) -c led.c

rom.o:		rom.c rom.h servo.h
	avr-gcc -g $(COPT) -mmcu=$(MCU) -c rom.c

servo.o:	servo.c servo.h servoturnout.h
	avr-gcc -g $(COPT) -mmcu=$(MCU) -c servo.c

size:	$(PROG).elf
	avr-size -C --mcu=$(MCU) $(PROG).elf

clean:
	rm -rf *.o $(PROG).elf $(PROG).hex

prog: $(PROG).hex
	avrdude -q -cavrispmkii -p$(MCU) -Ulfuse:w:0xE4:m -Uhfuse:w:0xDF:m -Uefuse:w:0xFF:m -Uflash:w:$(PROG).hex


