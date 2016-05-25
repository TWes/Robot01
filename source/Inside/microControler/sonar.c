#include "sonar.h"

extern volatile uint8_t txbuffer[30];

volatile uint8_t state_of_PD = 0;
volatile uint8_t state_of_PB = 0;

volatile sonar_entry sonar_table[5] = {{0,0,0}};
volatile uint8_t interrupt_occured = 0;

volatile SW_watchdog_t SW_watchdog = {0,0};

void init_sonar()
{
    // init the sonar_table
    sonar_table[0].TWI_buffer = 0x0A; // PD1 - LF
    sonar_table[1].TWI_buffer = 0x0C; // PD3 - LB
    sonar_table[2].TWI_buffer = 0x08; // PD5 - F
    sonar_table[3].TWI_buffer = 0x06; // PD7 - RF
    sonar_table[4].TWI_buffer = 0x04; // PB7 - RB


    DDRD |= 0b01010101; // Set trigger lines as output
    DDRD &= 0b01010101; // Set echo lines as input

    PORTD |= 0b01010101; //Setze ausgänge auf high

    DDRB |= 0b01000000;
    DDRB &= 0b01111111;

    PORTB |= 0b01000000;


    // We need to ser up the folowing
    // Pin change interrupts:
    // PCI2:    PD1 PCINT17
    //          PD3 PCINT19
    //          PD5 PCINT21
    //          PD7 PCINT23
    // PCI0:    PB7 PCINT7

    // Set up corresponding pins:
    PCMSK0 |= 0b10000000;
    PCMSK2 |= 0b10101010;

    // Now enable interrupts:
    PCICR |= 0b00000101;

    // Beginn the whole process

    _delay_us( 20 );
    activate_next_measurement( 4 );
}


void activate_next_measurement( uint8_t act_meas )
{
    SW_watchdog.last_sonar_measured = (act_meas + 1)%5;
    SW_watchdog.propagated_stop = 0;

    switch( act_meas )
    {
    case 0: // PD1 - LF was measured
        PORTD &= (0xff ^ 0x04); // PD2
        break;

    case 1: // PD3 - LB was measured
        PORTD &= (0xff ^ 0x10); //PD4
        break;

    case 2: // PD5 - F was measured
        PORTD &= (0xff ^ 0x40); // PD6
        break;

    case 3: // PD7 - RF was measured
        PORTB &= (0xff ^ 0x40); // PB6
        break;

    case 4: // PB7 - RB was measured
    default:
        PORTD &= (0xff ^ 0x01); // PD0
        break;
    }
}

void force_next_measurement( uint8_t act_meas )
{
    SW_watchdog.last_sonar_measured = (act_meas + 1)%5;
    SW_watchdog.propagated_stop = 0;

    switch( act_meas )
    {
    case 0: // PD1 - LF was measured
        PORTD |= (1 << 0 ); // Setze Trigger wieder auf high
        PORTD &= (0xff ^ 0x04); // PD2
        break;

    case 1: // PD3 - LB was measured
        PORTD |= (1 << 2 ); // Setze Trigger wieder auf high
        PORTD &= (0xff ^ 0x10); //PD4
        break;

    case 2: // PD5 - F was measured
        PORTD |= (1 << 4 ); // Setze Trigger wieder auf high
        PORTD &= (0xff ^ 0x40); // PD6
        break;

    case 3: // PD7 - RF was measured
        PORTD |= (1 << 6 ); // Setze Trigger wieder auf high
        PORTB &= (0xff ^ 0x40); // PB6
        break;

    case 4: // PB7 - RB was measured
    default:
        PORTB |= (1 << 6 ); // Setze Trigger wieder auf high
        PORTD &= (0xff ^ 0x01); // PD0
        break;
    }
}


ISR( PCINT0_vect )
{
    uint8_t port_b = PINB;
    uint8_t change_vector = port_b ^ state_of_PB;

    if( change_vector & (1 << 7)) // PB7 was changed
    {
        if( port_b & (1 << 7)) // PD1 is high now
        {
            // Now start the measurement
            sonar_table[4].meas_start = TCNT1;

            PORTB |= (1 << 6 ); // Setze trigger wieder high

        }
        else // PB7 is low now
        {
            // measurement is finished
            // only use, if no twi interrupt
            // blocked since the beginning
            if( !interrupt_occured )
            {
                sonar_table[4].meas_end = TCNT1;

                uint16_t meas_length = 0;

                // overflow
                if( sonar_table[4].meas_end < sonar_table[4].meas_start )
                {
                    meas_length = sonar_table[4].meas_start - sonar_table[4].meas_end;
                    meas_length = 0xffff - meas_length;
                }
                else // no overflow
                {
                    meas_length = sonar_table[4].meas_end - sonar_table[4].meas_start;
                }

                txbuffer[sonar_table[4].TWI_buffer] = meas_length;
                txbuffer[sonar_table[4].TWI_buffer +1 ] = (meas_length >> 8);
            }


            // start next measurement
            activate_next_measurement( 4 );
        }
    }

    state_of_PB = port_b;

}


ISR( PCINT2_vect )
{
    // Check there is a change
    uint8_t port_d = PIND;
    uint8_t change_vector = port_d ^ state_of_PD;


    //txbuffer[0x18] = state_of_PD;
    //txbuffer[0x19] = port_d;

    for( uint8_t active_entry = 0; active_entry <= 3; active_entry++)
    {
        uint8_t active_bit = (2*active_entry) + 1;

        if( change_vector & (1 << active_bit )) // PD* was changed
        {
            if( port_d & (1 << active_bit )) // PD* is high now
            {
                // Now start the measurement
                sonar_table[active_entry].meas_start = TCNT1;

                PORTD |= (1 << (2*active_entry) ); // Setze trigger wieder high
            }

            else // PD* is low now
            {
                // measurement is finished
                // only use, if no twi interrupt
                // blocked since the beginning
                if( !interrupt_occured )
                {
                    sonar_table[active_entry].meas_end = TCNT1;

                    uint16_t meas_length = 0;

                    // overflow
                    if( sonar_table[active_entry].meas_end < sonar_table[active_entry].meas_start )
                    {
                        meas_length = sonar_table[active_entry].meas_start - sonar_table[active_entry].meas_end;
                        meas_length = 0xffff - meas_length;
                    }
                    else // no overflow
                    {
                        meas_length = sonar_table[active_entry].meas_end - sonar_table[active_entry].meas_start;
                    }

                    txbuffer[sonar_table[active_entry].TWI_buffer] = meas_length;
                    txbuffer[sonar_table[active_entry].TWI_buffer +1 ] = (meas_length >> 8);
                }


                // start next measurement
                activate_next_measurement( active_entry );
            }

            break;
        }

         state_of_PD = port_d;
    }
}
