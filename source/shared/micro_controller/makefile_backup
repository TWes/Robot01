CFILES= main.c twislave.c timer.c adc.c sonar.c

all: main.hex

# From elf file to hex format
main.hex: main.elf	
	avr-objcopy -O ihex -j .text -j .data -j .bss main.elf main.hex

# Here the linking takes place
main.elf: $(CFILES)
	avr-gcc -Os -std=c99 -mmcu=atmega328p $(CFILES) -o main.elf

#phony clean
.PHONY: clean
clean:
	rm -f main.elf
	rm -f main.hex

#Upload to board
upload: main.hex
	sudo avrdude -p m328p -c avrispmkII -P /dev/ttyACM0 -U flash:w:main.hex:a

#clean rebuild ald upload
rebuild: clean main.hex
