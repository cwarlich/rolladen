/**********************/
/* Change as desired. */
/**********************/
#define ONTIME 60                // 60s until stop.
#define UPDOWN_DELAY 100         // 100ms between changing direction.
//#define NDEBUG                   // Disable assert.
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
}
inline void down() {
    PORTB &= ~_BV(PB3);          // PB2=Low -> Rolladen rauf aus.
    _delay_ms(UPDOWN_DELAY);              
    PORTB |= _BV(PB2);           // PB3=High -> Rolladen runter an.
}
inline void stop() {
    PORTB &= ~_BV(PB3);          // PB2=Low -> Rolladen rauf aus.
    PORTB &= ~_BV(PB2);          // PB2=Low -> Rolladen runter aus.
}
inline void setupPorts() {
    DDRB = _BV(PB2) | _BV(PB3);  // Output on PB2 and PB3 .
    PORTB = _BV(PB1) | _BV(PB0); // Pullup on PB1.
}
inline void setupInts() {
    // Rising edge for INT0 and allow power down mode.
    MCUCR = _BV(ISC00) | _BV(ISC01) | _BV(SE) | _BV(SM1);
    //MCUCR = _BV(ISC00) | _BV(ISC01);
    //PCMSK = _BV(PCINT1);       // Enables pin change interupt on PB1.
    SREG = _BV(SREG_I);          // STI, gets cleared in ISR and restored on RTI.
    //GIMSK = _BV(PCIE);         // Enable pin change interrupts.
    GIMSK = _BV(INT0);           // Enable INT0.
}
inline void setupWdt() {
    //wdt_reset();
    //WDTCR |= _BV(WDTIE) | _BV(WDP0) | _BV(WDP1) | _BV(WDP2); // 2s Timeout.
    WDTCR |= _BV(WDTIE);         // 16ms Timeout.
}
int32_t counter;
uint16_t onTime;
uint16_t eepromId EEMEM;
uint16_t id;
int main() {
    id = eeprom_read_word(&eepromId);
    counter = -1;
    setupPorts();
    setupWdt();
    setupInts();
    set_sleep_mode(SLEEP_MODE_PWR_SAVE);
    while(true) sleep_mode();
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
/* Just to wake up. */
ISR(PCINT0_vect) {
}
