#include <Arduino.h>
#include <usart.h>
#include <timers.h>

#define SERVO_PIN 11

// senzor
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define OUT_PIN 2
#define LED_PIN 13
#define LED_CTRL 3

// usart
#define CLOCK_SPEED 12000000
#define BAUD 38400
#define MYUBRR CLOCK_SPEED/16/BAUD-1

// motor
#define IN1 8   // PB0
#define IN2 9   // PB1
#define ENA 10  // PB2

// timer
bool motorRunning = false;
uint32_t motorTime = 0;

// buton
bool sensorEnabled = true;

// Citeste frecventa pentru o anumita culoare
unsigned long readColor(bool s2, bool s3)
{
    digitalWrite(S2, s2);
    digitalWrite(S3, s3);

    delay(50);

    // masoara durata impulsului LOW
    cli();
    unsigned long duration = pulseIn(OUT_PIN, LOW, 50000UL);
    sei();

    if (duration == 0)
        return 0;

    // convertim perioada in frecventa
    return 1000000UL / duration;
}

void servoWriteAngle(int angle)
{
    int pulseWidth = map(angle, 0, 180, 1000, 2000);

    digitalWrite(SERVO_PIN, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(SERVO_PIN, LOW);
}

void motorStart()
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, 200);
}

void motorStop()
{
    analogWrite(ENA, 0);
}

void setup()
{
    Timer1_init_systicks();
    sei();
    pinMode(SERVO_PIN, OUTPUT);
    servoWriteAngle(0);
    USART0_init(MYUBRR);
    Serial.begin(9600);
    pinMode(LED_CTRL, OUTPUT);

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(ENA, OUTPUT);

    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);

    pinMode(OUT_PIN, INPUT);

    pinMode(LED_PIN, OUTPUT);

    // Scalare frecventa 20%
    digitalWrite(S0, HIGH);
    digitalWrite(S1, LOW);
    digitalWrite(LED_CTRL, HIGH);

    USART0_print("TCS3200 & MOTOR START\r\n");
}

void loop()
{
    // ================= SENZOR =================
    unsigned long red = readColor(LOW, LOW);
    unsigned long blue = readColor(LOW, HIGH);
    unsigned long green = readColor(HIGH, HIGH);
    bool isBlue = (blue > red && blue > green);
    bool isGreen = (green > red && green > blue);

    // DEBUG
    char buff[64];
    snprintf(buff, sizeof(buff),
    "RAW R:%lu G:%lu B:%lu\r\n",
    red, green, blue);
    USART0_print(buff);

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
        servoWriteAngle(0);
    }
    else
    {
        servoWriteAngle(90);
    }
    
    // ================= TIMER STOP (5s) =================
    if (motorRunning && SYSTICKS_PASSED(motorTime, 5000))
    {
        motorTime = systicks;
        motorRunning = false;
        sensorEnabled = true;
        motorStop();
        digitalWrite(LED_CTRL, HIGH);
    }

    // ================= BUTTON =================
    if (!(PINB & (1 << PB7)))
    {
        sensorEnabled = !sensorEnabled;
        USART0_print(sensorEnabled ? "SENSOR ON\r\n" : "SENSOR OFF\r\n");
        _delay_ms(300);
    }

    // ================= SENZOR OFF =================
    if (!sensorEnabled)
    {
        digitalWrite(LED_CTRL, LOW);
    }
}