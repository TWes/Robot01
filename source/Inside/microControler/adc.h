#ifndef ADC_H
#define ADC_H

#include "avr/io.h"
#include "avr/interrupt.h"

#include "twislave.h"


/*******
 *  ADC Mux table
 *  Lower batterie cell     :   0b0011
 *  Higher batterie cell    :   0b0010
 *  Right Wheel encoder     :   0b0001
 *  Left Wheel encoder      :   0b0000
 *******/

typedef struct
{
    uint8_t mux_channel;
    uint16_t last_restistance;
    uint8_t TWI_buffer;
    enum{BLACK, WHITE, NO_WHEEL} last_state;

} adc_entry;

extern volatile adc_entry adc_table[4];
extern volatile uint16_t rotations[2];


void init_adc_module();

//ISR( ADC_vect )

#endif // ADC_H
