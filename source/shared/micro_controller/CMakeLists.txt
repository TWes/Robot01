cmake_minimum_required(VERSION 2.8)

project( micro_controller )

find_program(AVR_CC avr-gcc)
find_program(AVR_OBJCOPY avr-objcopy)

set( MC_INCLUDE_DIRS
	.
)

include_directories( ${MC_INCLUDE_DIRS} )


set( MC_FILES
	main.c
	adc.c
	sonar.c
	timer.c
	twislave.c
)

# former Makefile
#main.elf: $(CFILES)
#	avr-gcc -Os -std=c99 -mmcu=atmega328p $(CFILES) -o main.elf

add_custom_target( main.elf )
add_custom_command( TARGET main.elf 
	DEPENDS ${CFILES}
	COMMAND avr-gcc -Os -std=c99 -mmcu=atmega328p ${MC_FILES} -o main.elf
)

#former Makefile
#main.hex: main.elf	
#	avr-objcopy -O ihex -j .text -j .data -j .bss main.elf main.hex
add_custom_target( main.hex 
	ALL
	DEPENDS main.elf
)

add_custom_command( 
	TARGET main.hex
	COMMAND avr-objcopy -O ihex -j .text -j .data -j .bss main.elf main.hex
)

# The old Makefile
#all: main.hex

#phony clean
#.PHONY: clean
#clean:
#	rm -f main.elf
#	rm -f main.hex

#Upload to board
#upload: main.hex
#	sudo avrdude -p m328p -c avrispmkII -P /dev/ttyACM0 -U flash:w:main.hex:a

#clean rebuild ald upload
#rebuild: clean main.hex		





