# Proiect PM

## Sistem automat de sortare

In cadrul proiectul am realizat o banda rulanta care este miscata prin PWM (manual) de un motor (12V, primeste 10V din cauza l209n);

Sortarea se realizeaza printr-un senzor de culoare TCS3200 care la detectia de BLUE trimite semnal catre motor pentru a pune banda in miscare,
iar la detectia de RED || GREEN va trimite un semnal catre SG90 (servomotor) pentru a arunca de pe banda obiectul.

Componentele sunt cele de aici: https://ocw.cs.pub.ro/courses/pm/prj2026/florin.stancu/petru.radulescu?&#lista_de_piese.

## HARDWARE

PWM motor:
Am un counter si un duty care pe baza acestora imi verifica daca motorul este ON sau OFF;

Motor duty specifica viteza pe care o are motorul (0-255);

Senzor TCS3200:
Primeste un factor de scalare (20%) care face semnalul mai lent, ajuta la erori si stabilitate. Acesta produce frecventa digitala.

L298N - motor driver:
Legatura dintre MCU si motor, acesta controleaza curentul din sursa externa (12V in cazul meu) si semnalul mic de la MCU pentru a le trimite la motor;
Acest driver (cf. datasheet) mananca 2V, asadar ajung ~10V la motor.

Are rolul de a opera motorul (motoarele) prin porturile ENA / ENB si schimba directia in care se invarte motorul (invart motoarele).

## SOFTWARE

ReadColor() determina ce culoare sa citeasca senzorul (este un filtru), se stabilizeaza si masoara semnalul de iesire, asa decid ce culoare a citit senzorul;

ServoWriteAngle() transforma un unghi intr-un semnal PWM, durata_puls = pozitia SG90; acesta are un unghi de actiune de pana la 180 de grade.
