#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#define F_CPU 1000000UL      // 1 MHz (fuer delay.h)
#include <util/delay.h>
#include "avr_count.h"
#include <assert.h>
template<int N> struct Ln {enum {value = Ln<N >> 1>::value + 1};};
template<> struct Ln<1> {enum {value = 0};};
#define ADDRESS(count) (count >> Ln<STOP << MARGIN>::value + 1)
#define MASK ((1 << (Ln<STOP << MARGIN>::value + 1)) - (1 << MARGIN))
#define ID 10
#define ONTIME 20
inline void up() {
    PORTB &= ~_BV(PB2);        // PB2=Low -> Rolladen runter aus
    _delay_ms(100);            // Warte 100ms
    PORTB |= _BV(PB3);         // PB3=High -> Rolladen rauf an
}
inline void down() {
    PORTB &= ~_BV(PB3);        // PB2=Low -> Rolladen rauf aus
    _delay_ms(100);            // Warte 100ms
    PORTB |= _BV(PB2);         // PB3=High -> Rolladen runter an
}
inline void stop() {
    PORTB &= ~_BV(PB3);        // PB2=Low -> Rolladen rauf aus
    PORTB &= ~_BV(PB2);        // PB2=Low -> Rolladen runter aus
}
inline void setupPorts() {
    DDRB = _BV(PB2) | _BV(PB3); // Output on PB2 and PB3 
    PORTB = _BV(PB1) | _BV(PB0); // Pullup on PB1
}
inline void setupInts() {
    // Rising edge for INT0 and allow power down mode.
    //MCUCR = _BV(ISC00) | _BV(ISC01) | _BV(SE) | _BV(SM1);
    MCUCR = _BV(ISC00) | _BV(ISC01);
    //PCMSK = _BV(PCINT1); // Enables pin change interupt on PB1.
    SREG = _BV(SREG_I); // STI, gets cleared in ISR and restored on RTI.
    //GIMSK = _BV(PCIE); // Enable pin change interrupts.
    GIMSK = _BV(INT0); // Enable INT0.
}
inline void setupWdt() {
    wdt_reset();
    //WDTCR |= _BV(WDTIE) | _BV(WDP0) | _BV(WDP1) | _BV(WDP2);
    WDTCR |= _BV(WDTIE) | _BV(WDP0);
}
inline void powerDown() {
}
void abort() {
    PORTB |= _BV(PB3);        // PB2=Low -> Rolladen rauf an
    PORTB |= _BV(PB2);        // PB2=Low -> Rolladen runter an
    while(true) {};
}
int counter;
int onTime;
const int id = ID;
int main() {
    assert(MASK == 12);
    assert(id == ADDRESS(16 * id));
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
    if(id == ADDRESS(c)) {
        switch(c & MASK) {
            case UP << MARGIN: up(); break; // id * 4  pulses + 1
            case DOWN << MARGIN: down(); break; // id * 4 pulses + 2
            default: stop(); break; // id * 4 pulses + 3 or + 4
        }
    }
}
ISR(INT0_vect) {
    wdt_reset();
    counter++;
}
/* Just to wake up. */
ISR(PCINT0_vect) {
}
