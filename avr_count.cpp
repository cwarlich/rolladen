#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#define F_CPU 1000000UL          // 1 MHz (fuer delay.h)
#include <util/delay.h>
#include "avr_count.h"
#include <assert.h>
#define ID (OFFSET + 1)
#define ONTIME 125 * 60 / 2      // 60s until stop.
inline void up() {
    PORTB &= ~_BV(PB2);          // PB2=Low -> Rolladen runter aus
    _delay_ms(100);              // Warte 100ms
    PORTB |= _BV(PB3);           // PB3=High -> Rolladen rauf an
}
inline void down() {
    PORTB &= ~_BV(PB3);          // PB2=Low -> Rolladen rauf aus
    _delay_ms(100);              // Warte 100ms
    PORTB |= _BV(PB2);           // PB3=High -> Rolladen runter an
}
inline void stop() {
    PORTB &= ~_BV(PB3);          // PB2=Low -> Rolladen rauf aus
    PORTB &= ~_BV(PB2);          // PB2=Low -> Rolladen runter aus
}
inline void setup() {
}
inline void setupPorts() {
    DDRB = _BV(PB2) | _BV(PB3);  // Output on PB2 and PB3 
    PORTB = _BV(PB1) | _BV(PB0); // Pullup on PB1
}
inline void setupInts() {
    // Rising edge for INT0 and allow power down mode.
    //MCUCR = _BV(ISC00) | _BV(ISC01) | _BV(SE) | _BV(SM1);
    MCUCR = _BV(ISC00) | _BV(ISC01);
    //PCMSK = _BV(PCINT1);       // Enables pin change interupt on PB1.
    SREG = _BV(SREG_I);          // STI, gets cleared in ISR and restored on RTI.
    //GIMSK = _BV(PCIE);         // Enable pin change interrupts.
    GIMSK = _BV(INT0);           // Enable INT0.
}
inline void setupWdt() {
    wdt_reset();
    //WDTCR |= _BV(WDTIE) | _BV(WDP0) | _BV(WDP1) | _BV(WDP2); // 2s Timeout.
    WDTCR |= _BV(WDTIE);         // 16ms Timeout.
}
inline void powerDown() {
}
void abort() {
    PORTB |= _BV(PB3);           // PB2=Low -> Rolladen rauf an
    PORTB |= _BV(PB2);           // PB2=Low -> Rolladen runter an
    while(true) {};
}
int counter;
int onTime;
const int id = ID;
int main() {
    counter = -1;
    onTime = ONTIME;
    setupPorts();
    setupInts();
    setupWdt();
    while(true) {
        powerDown();
    }
}
ISR(WDT_vect) {
    wdt_reset();
    int c = counter;
    counter = -1;
    if(id == c / (oneMoreThanLastEnum * (MARGIN + 1))) {
        switch((c - (MARGIN + 1) * oneMoreThanLastEnum * id - 1) / oneMoreThanLastEnum) {
            case rauf: up(); break;
            case runter: down(); break;
            case halt: stop(); break;
            case init: setup(); break;
            default: assert(false);
        }
    }
    if(onTime) onTime--;
    else stop();
}
ISR(INT0_vect) {
    wdt_reset();
    counter++;
    onTime = ONTIME;
}
/* Just to wake up. */
ISR(PCINT0_vect) {
}
