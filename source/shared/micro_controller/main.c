#define F_CPU 8000000

#include "util/delay.h"
//#include "avr/io.h"
//#include "avr/interrupt.h"

#include "twislave.h"
#include "timer.h"
#include "adc.h"
#include "sonar.h"

/****************************
 *  I2C Register Map
 *----------------------------
 *  0x00 - 0x01 Batterie Voltage (3 Zellen)
 *  0x02 - 0x03 Batterie Voltage (Spannungsteiler 6 Zellen)
 *  0x04 - 0x05 Sonar Back Right
 *  0x06 - 0x07 Sonar Front Right
 *  0x08 - 0x09 Sonar Front
 *  0x0A - 0x0B Sonar Front Left
 *  0x0C - 0x0D Sonar Back Left
 *  0x0E        SW Watchdog Status Register
 *  0x0F        (empty)
 *  0x10 - 0x11 Right Wheel Rotations
 *  0x12 - 0x13 Right Wheel Sensor Resistance
 *  0x14 - 0x15 Left Wheel Rotations
 *  0x16 - 0x17 Left Wheel Sensor Resistance
 ****************************/

void main()
{
	cli();
	
	init_twi_slave( 0x24 << 1 );	

    init_timer_module();

    init_adc_module();

    init_sonar();

    sei();	

    for( int i = 0; i < i2c_buffer_size; i++ )
    {
        i2cdata[i] = 0x00;
    }

	while( 1 )
	{
    	_delay_ms(20);

    } // End of while loop
}
