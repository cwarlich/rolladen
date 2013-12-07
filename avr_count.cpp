/**********************/
/* Change as desired. */
/**********************/
#define ONTIME 60                // 60s until stop.
#define UPDOWN_DELAY 100         // 100ms between changing direction.
#define SWITCH_DELAY 500         // 500ms to stop
#define NDEBUG                   // Disable assert.
/*****************************************************************************/
/* Don't change anything below if you don't exactly know what you are doing! */
/*****************************************************************************/
#define F_CPU 1000000UL          // 1 MHz (fuer delay.h).
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "avr_count.h"
#include <assert.h>
#define OT 125 * ONTIME / 2      // 60s until stop.
void abort() {                   // Called by assert().
    PORTB |= _BV(PB3);           // PB2=Low -> Rolladen rauf an.
    PORTB |= _BV(PB2);           // PB2=Low -> Rolladen runter an.
    while(true) {};
}
inline void up() {
    PORTB &= ~_BV(PB2);          // PB2=Low -> Rolladen runter aus.
    _delay_ms(UPDOWN_DELAY);              
    PORTB |= _BV(PB3);           // PB3=High -> Rolladen rauf an.
    _delay_ms(UPDOWN_DELAY);              
}
inline void down() {
    PORTB &= ~_BV(PB3);          // PB2=Low -> Rolladen rauf aus.
    _delay_ms(UPDOWN_DELAY);              
    PORTB |= _BV(PB2);           // PB3=High -> Rolladen runter an.
    _delay_ms(UPDOWN_DELAY);              
}
inline void stop() {
    PORTB &= ~_BV(PB3);          // PB2=Low -> Rolladen rauf aus.
    PORTB &= ~_BV(PB2);          // PB2=Low -> Rolladen runter aus.
    //_delay_ms(UPDOWN_DELAY);              
}
inline void noop() {}
inline void setupPorts() {
    DDRB = _BV(PB2) | _BV(PB3);  // Output on PB2 and PB3 .
    PORTB = _BV(PB1) | _BV(PB0) | _BV(PB4); // Pullup on PB1.
}
inline void setupInts() {
    // Rising edge for INT0 and allow power down mode.
    //MCUCR = _BV(ISC00) | _BV(ISC01) | _BV(SE) | _BV(SM1);
    MCUCR = _BV(ISC00) | _BV(ISC01);
    //PCMSK = _BV(PCINT0) | _BV(PCINT4); // Enables pin change interupt on PB1 and PB4.
    SREG = _BV(SREG_I);                // STI, gets cleared in ISR and restored on RTI.
    //GIMSK = _BV(INT0) | _BV(PCIE);     // Enable INT0 and PCI.
    GIMSK = _BV(INT0);                 // Enable INT0.
}
inline void setupWdt() {
    //WDTCR |= _BV(WDTIE) | _BV(WDP0) | _BV(WDP1) | _BV(WDP2); // 2s Timeout.
    WDTCR |= _BV(WDTIE);         // 16ms Timeout.
}
int32_t counter = -1;
uint16_t onTime = OT;
uint16_t eepromId EEMEM;
uint16_t id;
bool upSwitch = false, downSwitch = false;
uint8_t pb, oldPb;
inline void setSwitch() {
    if((pb & _BV(PB0)) != (oldPb & _BV(PB0))) upSwitch = true;
    if((pb & _BV(PB4)) != (oldPb & _BV(PB4))) downSwitch = true;
    oldPb = pb;
}
int main() {
    PORTB = _BV(PB2) | _BV(PB3);
    if((PINB & (_BV(PB2) | _BV(PB3))) != (_BV(PB2) | _BV(PB3))) {
        eeprom_write_word(&eepromId, 0); 
    }
    id = eeprom_read_word(&eepromId);
    setupPorts();
    setupWdt();
    setupInts();
    set_sleep_mode(SLEEP_MODE_PWR_SAVE);
    oldPb = PINB & (_BV(PB0) | _BV(PB4));
    while(true) {
        pb = PINB & (_BV(PB0) | _BV(PB4));
        if(pb != oldPb) {
            setSwitch();
            _delay_ms(UPDOWN_DELAY);
            pb = PINB & (_BV(PB0) | _BV(PB4));
            setSwitch();
            void (*f)(void) = noop;
            cli();
            if(upSwitch && downSwitch) f = stop;
            else {
                onTime = OT;
                if(downSwitch) f = down;
                else if(upSwitch) f = up;
            }
            sei();
            f();
            upSwitch = downSwitch = false;
        }
    }
}
ISR(WDT_vect) {
    wdt_reset();
    uint16_t receivedId = counter / (oneMoreThanLastEnum * M);
    if(id == receivedId || (id == -1 && receivedId >= OFFSET + 1)) {
        switch((counter % (oneMoreThanLastEnum * M)) / M) {
            case rauf: up(); break;
            case runter: down(); break;
            case halt: stop(); break;
            case init:
                if(id == -1) {
                    id = receivedId;
                    eeprom_write_word(&eepromId, receivedId); 
                }
                break;
            default: assert(false);
        }
        upSwitch = downSwitch = false;
    }
    counter = -1;
    if(onTime) onTime--;
    else stop();
}
ISR(INT0_vect) {
    wdt_reset();
    counter++;
    onTime = OT;
}
//ISR(PCINT0_vect) {/* Check which port caused PCI. */}
