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
    adc_table[0].change_thresh = 10;
    adc_table[0].last_state = BLACK;

    rotations[0] = 0;

    // Set up ADC1 (left wheel)
    adc_table[1].mux_channel = 0b0001;
    adc_table[1].last_restistance = 0;
    adc_table[1].TWI_buffer = 0x10;
    adc_table[1].change_thresh = 4;
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
    // set prescaler to 128
    ADCSRA |= ( 0b111 << 0);

    // Activate Free running Mode
    ADCSRB |= (0b000 << 0);

    // Start ADC Module
    ADCSRA |= (1 << 7);
    // Start conversion
    ADCSRA |= (1 << 6);
}



ISR( ADC_vect )
{
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

      uint16_t last_resistance = adc_table[mux_field].last_restistance;
      int res_diff = adc_value - last_resistance;
      if( res_diff < 0 ) res_diff * -1;	

      // Threshhold found out by test
      int threshhold = adc_table[mux_field].change_thresh;
      if( res_diff > threshhold )
      {
	      // Interpret the values
	      // Ranges found out by test
	      uint16_t lower_thresh = 613;
	      uint16_t upper_thresh = 818;
	
	      wheel_encoder_state act_state = MIDDLE;
	      if( adc_value < lower_thresh )
	      {
	         act_state = WHITE;
	      }
	      else if( adc_value > upper_thresh )
	      {
	         act_state = BLACK;
	      }
	
	      wheel_encoder_state last_state = adc_table[mux_field].last_state;
	
	      if( last_state == BLACK && act_state == MIDDLE )
	      {
		adc_table[mux_field].last_state = MIDDLE; //act_state;
	      }     
	      else if( last_state == WHITE && act_state == MIDDLE )
	      {
		adc_table[mux_field].last_state = MIDDLE; // act_state;
	      }
	      else if( last_state == MIDDLE && act_state == BLACK )
	      {
	          // Incement rotations
	          rotations[mux_field]++;
	          i2cdata[adc_table[mux_field].TWI_buffer] = rotations[mux_field];
	          i2cdata[adc_table[mux_field].TWI_buffer + 1] = rotations[mux_field] >> 8;
	
	          adc_table[mux_field].last_state = act_state;
	      }
	      else if( last_state == MIDDLE && act_state == WHITE )
	      {
		   // Incement rotations
	          rotations[mux_field]++;
	          i2cdata[adc_table[mux_field].TWI_buffer] = rotations[mux_field];
	          i2cdata[adc_table[mux_field].TWI_buffer + 1] = rotations[mux_field] >> 8;
	
	          adc_table[mux_field].last_state = act_state;
      	}
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

