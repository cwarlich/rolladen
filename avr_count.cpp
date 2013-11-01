#include <avr/io.h>
#define F_CPU 8000000UL      // 8 MHz (fuer delay.h)
#include <util/delay.h>
#include "avr_count.h"
#define MAX_ACTIVE 0xfffffff
template<int N> struct Ln {enum {value = Ln<N >> 1>::value + 1};};
template<> struct Ln<1> {enum {value = 0};};
// Shift count down by 2.
#define ADDRESS(count) (count >> Ln<STOP>::value + 1)
// Mask is 0x3.
#define MASK (2^(Ln<STOP>::value + 1) - 1)
void setEeprom(int count) {}
static uint32_t active = 0;
void up() {
    PORTB &= ~_BV(PB1);        // PB1=Low -> Rolladen runter aus
    _delay_ms(100);            // Warte 100ms
    PORTB |= _BV(PB3);         // PB3=High -> Rolladen rauf an
}
void down() {
    PORTB &= ~_BV(PB3);        // PB1=Low -> Rolladen rauf aus
    _delay_ms(100);            // Warte 100ms
    PORTB |= _BV(PB1);         // PB3=High -> Rolladen runter an
}
void report() {}
void stop() {
    PORTB &= ~_BV(PB3);        // PB1=Low -> Rolladen runter aus
    PORTB &= ~_BV(PB1);        // PB1=Low -> Rolladen rauf aus
}
bool in() {return PINB & _BV(PB0);}
int16_t eeprom = 10; // set to -1 initially
int main() {
    uint16_t period;
    uint16_t count;
    DDRB = _BV(PB1) | _BV(PB3); // Output on PB1 and PB3 
    PORTB = _BV(PB0); // Pullup on PB0
    stop();
    while(true) {
        if(active) active--;
        else stop();
        if(!in()) {
            count = 0;
            do {
                while(!in()) period++;
                count++;
                for(int i = 0; i < period; i++) {
                    if(in()) period--; else break;
                }
            } while(period);
            up();
            active = MAX_ACTIVE;
#if 0
            if(eeprom == -1) setEeprom(ADDRESS(count));
            else if(eeprom == ADDRESS(count)) {
                switch(count & MASK) {
                    case UP: up(); break; // eepprom * 4 pulses
                    case DOWN: down(); break; // eeprom * 4 pulses + 1
                    case REPORT: report(); break; // eeprom * 4 pulses + 2
                    default: stop(); break; // eeprom * 4 pulses + 3
                }
                active = MAX_ACTIVE;
            }
        }
#endif
    }
}
