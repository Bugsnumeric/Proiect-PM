#include "usart.h"

/*
 * Functie de initializare a controllerului USART
 */
void USART0_init(unsigned int ubrr)
{
    /* seteaza baud rate-ul */
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;

    /* porneste transmitatorul */
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);

    /* seteaza formatul frame-ului: 8 biti de date, 2 biti de stop, fara paritate */
    UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}


/*
 * Functie ce transmite un caracter prin USART
 *
 * @param data - caracterul de transmis
 */
void USART0_transmit(char data)
{
    /* asteapta pana bufferul e gol */
    while(!(UCSR0A & (1<<UDRE0)));

    /* pune datele in buffer; transmisia va porni automat in urma scrierii */
    UDR0 = data;
}

/*
 * Functie ce primeste un caracter prin USART
 *
 * @return - caracterul primit
 */
char USART0_receive()
{
    /* asteapta cat timp bufferul e gol */
    while(!(UCSR0A & (1<<RXC0)));

    /* returneaza datele din buffer */
    return UDR0;
}

/*
 * Functie ce transmite un sir de caractere prin USART
 *
 * @param data - sirul (terminat cu '\0') de transmis
 */
void USART0_print(const char *data)
{
    while(*data != '\0')
        USART0_transmit(*data++);
}

void led_off()
{
    PORTD |= (1<<PD5);
    PORTD |= (1<<PD7);
    PORTB |= (1<<PB3);
}

void USART_exec(const char *cmd) {
    if (strcmp(cmd, "on") == 0) {
        PORTD &= ~(1<<PD5);
        PORTD &= ~(1<<PD7);
        PORTB &= ~(1<<PB3);
    }

    if (strcmp(cmd, "off") == 0) {
        led_off();
    }

    if (strcmp(cmd, "red") == 0) {
        led_off();
        PORTD &= ~(1<<PD5);
    }

    if (strcmp(cmd, "green") == 0) {
        led_off();
        PORTD &= ~(1<<PD7);
    }

    if (strcmp(cmd, "blue") == 0) {
        led_off();
        PORTB &= ~(1<<PB3);
    }
}
