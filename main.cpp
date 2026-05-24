#include <Arduino.h>
#include <usart.h>
#include <timers.h>

#define SERVO_PIN PB3

// senzor
#define S0 PD4
#define S1 PD5
#define S2 PD6
#define S3 PD7
#define OUT_PIN_TCS PD2
#define LED_CTRL_TCS PD3

// usart
#define CLOCK_SPEED 12000000
#define BAUD 38400
#define MYUBRR CLOCK_SPEED/16/BAUD-1

// motor driver
#define IN1 PB0
#define IN2 PB1
#define ENA PB2

// timer
bool motorRunning = false;
uint32_t motorTime = 0;

// buton MCU + LED
#define LED_PIN_MCU PB5
#define BUTTON_MCU PB7
bool sensorEnabled = true;

// Citeste frecventa pentru o anumita culoare
unsigned long readColor(bool s2, bool s3)
{
    if (s2)
        PORTD |= (1 << S2);
    else
        PORTD &= ~(1 << S2);

    if (s3)
        PORTD |= (1 << S3);
    else
        PORTD &= ~(1 << S3);

    delay(50);

    // masoara durata impulsului
    unsigned long duration = pulseIn(OUT_PIN_TCS, LOW, 50000UL);

    if (duration == 0)
        return 0;

    // conversie perioada in frecventa
    return 1000000UL / duration;
}

void servoWriteAngle(int angle)
{
    int pulseWidth = map(angle, 0, 180, 1000, 2000);

    // HIGH
    PORTB |= (1 << SERVO_PIN);
    delayMicroseconds(pulseWidth);
    // LOW
    PORTB &= ~(1 << SERVO_PIN);
}

void motorStart()
{
    // backward HIGH + LOW
    PORTB |= (1 << IN1);
    PORTB &= ~(1 << IN2);
    // speed
    motorDuty = 255;
}

void motorStop()
{
    motorDuty = 0;
    PORTB &= ~(1 << ENA);
}

void setup()
{
    // motor driver
    DDRB |= (1 << IN1);
    DDRB |= (1 << IN2);
    DDRB |= (1 << ENA);

    // timer manual
    Timer1_init_systicks();
    sei();

    // sg90
    DDRB |= (1 << SERVO_PIN);
    servoWriteAngle(180);

    USART0_init(MYUBRR);
    Serial.begin(9600);

    // senzor
    DDRD |= (1 << LED_CTRL_TCS);
    DDRD |= (1 << S0);
    DDRD |= (1 << S1);
    DDRD |= (1 << S2);
    DDRD |= (1 << S3);
    DDRD &= ~(1 << OUT_PIN_TCS);

    // led mcu
    DDRB |= (1 << LED_PIN_MCU);

    // init sg90
    PORTB &= ~(1 << SERVO_PIN);

    // Scalare frecventa
    PORTD |= (1 << S0);
    PORTD &= ~(1 << S1);

    PORTD |= (1 << PD3);
    USART0_print("START\r\n");
}

void loop()
{
    // ================= SENZOR =================
    unsigned long red = 0;
    unsigned long blue = 0;
    unsigned long green = 0;

    if (sensorEnabled) {
        red = readColor(LOW, LOW);
        blue = readColor(LOW, HIGH);
        green = readColor(HIGH, HIGH);

        char buff[64];
        snprintf(buff, sizeof(buff), "R:%lu G:%lu B:%lu\r\n", red, green, blue);
        USART0_print(buff);
    }

    bool isBlue = (blue > red + 1000 && blue > green + 1000);
    bool isGreen = (green > red + 1000 && green > blue + 1000);
    bool isRed = (red > green + 1000 && red > blue + 1000);

    int servoAngle = 0;
    // ================= BLUE =================
    if (isBlue && sensorEnabled)
    {
        if (!motorRunning)
        {
            motorRunning = true;
            sensorEnabled = false;
            motorTime = systicks;

            motorStart();
            USART0_print("BLUE -> MOTOR START\r\n");
        }
        servoAngle = 180;
    }
    else if (isGreen || isRed)
    {
        servoAngle = 0;
    } else {
        servoAngle = 180;
    }
    servoWriteAngle(servoAngle);
    
    // ================= TIMER STOP (5s) =================
    if (motorRunning && SYSTICKS_PASSED(motorTime, 5000))
    {
        motorTime = systicks;
        motorRunning = false;
        sensorEnabled = true;
        motorStop();
        PORTD |= (1 << LED_CTRL_TCS);
    }

    // ================= BUTTON =================
    if (!(PINB & (1 << BUTTON_MCU)))
    {
        sensorEnabled = !sensorEnabled;
        PORTD ^= (1 << LED_CTRL_TCS);
        USART0_print(sensorEnabled ? "SENSOR ON\r\n" : "SENSOR OFF\r\n");
        _delay_ms(300);
    }

    // ================= SENZOR OFF =================
    if (!sensorEnabled)
    {
        PORTD &= ~(1 << LED_CTRL_TCS);
    }
}
