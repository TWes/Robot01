#ifndef SONAR_H
#define SONAR_H

#include "avr/io.h"
#include "avr/interrupt.h"
#include "twislave.h"

#define F_CPU 8000000

#include<util/delay.h>


/**************
 *  Pin Map Sonar
 *
 *  Front:
 *      trig - PD4
 *      echo - PD5
 *
 *  Right Back:
 *      trig - PB6
 *      echo - PB7
 *
 *  Right Front:
 *      trig - PD6
 *      echo - PD7
 *
 *  Left Back:
 *      trig - PD2
 *      echo - PD3
 *
 *  Left Front:
 *      trig - PD0
 *      echo - PD1
 *
 *************/

/************************************************************
 *  Strukture of the Watchdog register
 * | 4 bit              | 4 bit                             |
 * | Number of errors   | Sonar that caused the last error  |
 ************************************************************/

typedef struct {
    uint16_t meas_start;
    uint16_t meas_end;
    uint8_t TWI_buffer;
} sonar_entry;

typedef struct {
    uint8_t last_sonar_measured;
    uint8_t propagated_stop;
} SW_watchdog_t;

void init_sonar();

void activate_next_measurement( uint8_t act_meas );
void force_next_measurement( uint8_t act_meas );

extern volatile uint8_t state_of_PD;
extern volatile uint8_t state_of_PB;

extern volatile sonar_entry sonar_table[5];
extern volatile uint8_t interrupt_occured;

extern volatile SW_watchdog_t SW_watchdog;

// ISR( PCINT0_vect );
// ISR( PCINT2_vect );

#endif
