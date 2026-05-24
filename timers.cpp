#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include <timers.h>
#include <usart.h>

/* Allocates the system ticks counter (milliseconds since boot). */
volatile uint32_t systicks = 0;
volatile uint8_t pwmCounter = 0;
volatile uint8_t motorDuty = 0;
volatile uint8_t tickDivider = 0;
/* ======================================== Timer0 ======================================== */

/* Initialize Timer0 for Fast PWM mode (8 bits). */
void Timer0_init_pwm(void)
{
    /* TODO Task 2: initialize timer 0 (different from timer 2, look in datasheet) */

    /* Clear previous settings */
    TCCR0A = 0;
    TCCR0B = 0;

    /* Set Fast PWM (8 bits) */
    TCCR0A |= (1 << WGM00);
    TCCR0A |= (1 << WGM01);

    /* Set inverting output for OC0A */
    TCCR0A |= (1 << COM0A0);
    TCCR0A |= (1 << COM0A1);
    

    /* Set prescaler to 64 */
    TCCR0B |= (1 << CS01) | (1 << CS00);

    /* Set duty cycle to 0%; TOP = 255 */
    OCR0A = 0;
}

/* Initialize Timer0 for CTC mode. */
void Timer0_init_ctc(void)
{
    /* Clear previous settings */
    TCCR0A = 0;
    TCCR0B = 0;

    /* Set CTC mode */
    TCCR0A |= (1 << WGM01);

    /* Set prescaler to 256 */
    TCCR0B |= (1 << CS02);

    /* Activate Compare A interrupt */
    TIMSK0 |= (1 << OCIE0A);
}

ISR(TIMER0_COMPA_vect)
{
    /* TODO Task 3: implement Compare A interrupt for buzzer */
    PIND |= (1 << PIND4);
}

/* ======================================== Timer1 ======================================== */

/*
 * Initializes Timer1 for systick interrupt once every 1ms,
 * plus any custom behavior you require.
 */
void Timer1_init_systicks(void)
{
    /* Clear previous settings */
    TCCR1A = 0;
    TCCR1B = 0;

    /* Set CTC mode */
    TCCR1B |= (1 << WGM12);

    /* Set prescaler to 8 */
    TCCR1B |= (1 << CS11);

    /* Activate Compare A interrupt */
    TIMSK1 |= (1 << OCIE1A); 

    /* 12MHz / 8 => 1500 kHz */
    OCR1A = 100;

    /* TODO Task 2: don't forget something */
    //TIMSK1 |= (1 << OCIE1B);

}

// preluat din lab, schimbat pentru proiect (motor)
ISR(TIMER1_COMPA_vect)
{
    pwmCounter++;

    if (pwmCounter < motorDuty)
        PORTB |= (1 << PB2);
    else
        PORTB &= ~(1 << PB2);

    tickDivider++;

    if (tickDivider >= 15)
    {
        tickDivider = 0;
        systicks++;
    }
}

ISR(TIMER1_COMPB_vect)
{
    /* TODO Task 2: manual PWM */
    PORTD |= (1 << PD5);

}

/* ======================================== Timer2 ======================================== */

/* Initialize Timer2 for Fast PWM mode (8 bits). */
void Timer2_init_pwm(void)
{
    /* Clear previous settings */
    TCCR2A = 0;
    TCCR2B = 0;

    /* TODO Task 1: set Fast PWM (8 bits) */
    TCCR2A |= (3 << WGM20);


    /* TODO Task 1: set inverting output for OC2A */
    TCCR2A |= (3 << COM2A0);


    /* TODO Task 1: set prescaler to 64 */
    TCCR2B |= (1 << CS22);


    /* Set duty cycle to 0%; TOP = 255 */
    OCR2A = 0;
}
