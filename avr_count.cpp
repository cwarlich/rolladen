#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#define F_CPU 8000000UL      // 8 MHz (fuer delay.h)
#include <util/delay.h>
#include "avr_count.h"
template<int N> struct Ln {enum {value = Ln<N >> 1>::value + 1};};
template<> struct Ln<1> {enum {value = 0};};
// Shift count down by 2.
#define ADDRESS(count) (count >> Ln<STOP>::value + 1)
// Mask is 0x3.
#define MASK (2^(Ln<STOP>::value + 1) - 1)
#define ID 10
#define ONTIME 20
inline void up() {
    PORTB &= ~_BV(PB1);        // PB1=Low -> Rolladen runter aus
    _delay_ms(100);            // Warte 100ms
    PORTB |= _BV(PB3);         // PB3=High -> Rolladen rauf an
}
inline void down() {
    PORTB &= ~_BV(PB3);        // PB1=Low -> Rolladen rauf aus
    _delay_ms(100);            // Warte 100ms
    PORTB |= _BV(PB1);         // PB3=High -> Rolladen runter an
}
inline void stop() {
    PORTB &= ~_BV(PB3);        // PB1=Low -> Rolladen runter aus
    PORTB &= ~_BV(PB1);        // PB1=Low -> Rolladen rauf aus
}
inline void setPb1AndPb3ToOutputLowAndPb0ToInputPullUp() {
    DDRB = _BV(PB1) | _BV(PB3); // Output on PB1 and PB3 
    PORTB = _BV(PB0) | _BV(PB2); // Pullup on PB0 and PB2
}
inline void enablePb0RisingPcInt() {
    MCUCR = _BV(ISC01) | _BV(ISC00); // Generate interrupts on rising edge.
    // Generate interrupts on rising edge and allow power down mode.
    //MCUCR = _BV(ISC01) | _BV(ISC00) | _BV(SE) | _BV(SM1);
    PCMSK = _BV(PCINT0); // Enable intterupts on PB0.
    SREG = _BV(SREG_I); // STI, gets cleared in ISR and restored on RTI.
    GIMSK = _BV(PCIE); // Enable interrupts globally.
}
inline void setupAndEnableWdtInt() {
    wdt_enable(WDTIE | WDTO_30MS); // Enable WDT interrupts after 32ms.
}
inline void powerDown() {
}
int counter;
int onTime;
const int id = ID;
int main() {
    counter = -1;
    onTime = ONTIME;
    setPb1AndPb3ToOutputLowAndPb0ToInputPullUp();
    enablePb0RisingPcInt();
    setupAndEnableWdtInt();
    while(true) powerDown();
}
ISR(WDT_vect) {
    if(counter == -1 && onTime--) stop();
    else if(id == ADDRESS(counter)) {
        switch(counter & MASK) {
            case UP: up(); break; // id * 4 pulses
            case DOWN: down(); break; // id * 4 pulses + 1
            default: stop(); break; // id * 4 pulses + 3 or + 4
        }
    }
    counter = -1;
    onTime = 10;
}
ISR(PCINT0_vect) {
    wdt_reset();
    counter++;
}
