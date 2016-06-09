#include "adc.h"

volatile adc_entry adc_table[4];
volatile uint16_t rotations[2];


void init_adc_module()
{
    // initialise adc_table
    // Set up ADC0 (right wheel)
    adc_table[0].mux_channel = 0b0000;
    adc_table[0].last_restistance = 0;
    adc_table[0].TWI_buffer = 0x14;
    adc_table[0].last_state = BLACK;

    rotations[0] = 0;

    // Set up ADC1 (left wheel)
    adc_table[1].mux_channel = 0b0001;
    adc_table[1].last_restistance = 0;
    adc_table[1].TWI_buffer = 0x10;
    adc_table[1].last_state = BLACK;

    rotations[1] = 0;

    // Set up ADC2 (both batterie cells)
    adc_table[2].mux_channel = 0b0010;
    adc_table[2].last_restistance = 0;
    adc_table[2].TWI_buffer = 0x00;
    adc_table[2].last_state = NO_WHEEL;

    // Set up ADC3 (low batterie cell)
    adc_table[3].mux_channel = 0b0011;
    adc_table[3].last_restistance = 0;
    adc_table[3].TWI_buffer = 0x02;
    adc_table[3].last_state = NO_WHEEL;

    // Set up Multiplexer to ADC0
    // and reference to AREF
    ADMUX |= 0b01000000;

    // enable auto trigger
    ADCSRA |= (1 << 5);
    // enable interrupt
    ADCSRA |= (1 << 3);
    // set prescaler to 64
    ADCSRA |= ( 0b110 << 0);

    // Activate Free running Mode
    ADCSRB |= (0b000 << 0);

    // Start ADC Module
    ADCSRA |= (1 << 7);
    // Start conversion
    ADCSRA |= (1 << 6);
}



ISR( ADC_vect )
{
    i2cdata[0x00] = 0xbe;

    // handle the ADC Value
    uint16_t adc_value = ADC;

   // Interpret the values;
   uint8_t mux_field = ADMUX & 0x0f;

   // interpretate the values
   if( adc_table[mux_field].last_state != NO_WHEEL )
   {
       // Kopiere Wert in transmiter buffer
      i2cdata[adc_table[mux_field].TWI_buffer + 2] = adc_value;
      i2cdata[adc_table[mux_field].TWI_buffer + 3] = (adc_value >> 8);

      // Interpret the values
      // A good threshhold is 3 V (= 0,6 * max = 0x265 )
      // adc < 0x265 => White
      // adc >= 0x265 => Black

      // New value white, and old value black;
      if( adc_value < 0x265 &&  adc_table[mux_field].last_state == BLACK )
      {
          // Incement rotations
          rotations[mux_field]++;
          i2cdata[adc_table[mux_field].TWI_buffer] = rotations[mux_field];
          i2cdata[adc_table[mux_field].TWI_buffer + 1] = rotations[mux_field] >> 8;

          // Change State
          adc_table[mux_field].last_state = WHITE; 
      }

      // Old Value white, new value black
      else if( adc_value >= 0x265 &&  adc_table[mux_field].last_state == WHITE )
      {
          // Incement rotations
          rotations[mux_field]++;
          i2cdata[adc_table[mux_field].TWI_buffer] = rotations[mux_field];
          i2cdata[adc_table[mux_field].TWI_buffer + 1] = rotations[mux_field] >> 8;

          // Change State
          adc_table[mux_field].last_state = BLACK;
      }

      else
      {
      }
   }

   // The measurements commes from the batterie cells
   else
   {
       // Set the transmiter buffer value;
       i2cdata[adc_table[mux_field].TWI_buffer] = adc_value;
       i2cdata[adc_table[mux_field].TWI_buffer + 1] = (adc_value >> 8);
   }

   // Until now, this valuie is not needed
   adc_table[mux_field].last_restistance = adc_value;

   // safely change the multiplexer
   // Datasheet p. 255, 23.5 c)

   // Todo: Find a good algorithm, to
   // schedule the multiplexes
   // because ADC0/1 freq (100 Hz) >>>>>>>>>>>>>>>> ADC2/3 freq (0.5 Hz)
   static uint8_t schedule_counter = 1;
   if( schedule_counter == 200 )
   {
       //switch to ADC2
       ADMUX &= 0xf0;
       ADMUX |= 0b10;

   }
   else if( schedule_counter >= 201 )
   {
       // switch to ADC3
       ADMUX &= 0xf0;
       ADMUX |= 0b11;

       // Next increment will overflow, so
       // schedule_counter = 0
       schedule_counter = 0xff;
   }
   else
   {
        ADMUX &= 0xf0;
        ADMUX |= (0b1 & schedule_counter );
   }

   schedule_counter++;

}

