#include "timer.h"

void init_timer_module()
{
    // Disable PORT invokation
    TCCR1A = 0;

    // Set Prescaler to 64 - 0x03 (256 - 0x80)
    TCCR1B |= 0x03;

    // Overflow:
    // prescaler    period      overflow
    // 64           8µs         525ms
    // 8            1µs         65ms
    // 256          32µs        2097ms

    // Enable overflow interrupt
    TIMSK1 |= 0x01;
}


ISR( TIMER1_OVF_vect )
{
    static uint8_t last_sonar_measured = 0;
    static uint8_t error_counter = 0;

    // We might detected the same measurment twice
    if( last_sonar_measured == SW_watchdog.last_sonar_measured )
    {
        // We warned the software, so just stop now
        if( SW_watchdog.propagated_stop )
        {
            force_next_measurement( last_sonar_measured );

            error_counter++;

            uint8_t error_entry = (0x0f & last_sonar_measured);
            error_entry |= (error_counter << 4);
            i2cdata[0x0E] = error_entry;
        }

        // "Warn" the Software
        else
        {
            SW_watchdog.propagated_stop = 1;
        }
    }

    last_sonar_measured = SW_watchdog.last_sonar_measured;
}
