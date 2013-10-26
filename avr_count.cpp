#include <stdint.h>
#include <stdio.h>
#include "avr_count.h"
#define MAX_ACTIVE 0xffff
template<int N> struct Ln {enum {value = Ln<N >> 1>::value + 1};};
template<> struct Ln<1> {enum {value = 0};};
#define ADDRESS(count) (count >> Ln<STOP>::value + 1)
# define MASK (2^(Ln<STOP>::value + 1) - 1)
void setEeprom(int count) {}
static uint32_t active = 0;
void up() {}
void down() {}
void report() {}
void stop() {}
uint8_t portb;
int16_t eeprom; // set to -1 initially
uint8_t *in =&portb;
int16_t *myself = &eeprom;
int main() {
    long period;
    int count;
    printf("%d %d %d %d\n", MASK, ADDRESS(2), ADDRESS(3), ADDRESS(4));
    while(true) {
        if(active) active--;
        else stop();
        if(in) {
            count = 0;
            do {
                while(in) period++;
                count++;
                for(int i = 0; i < period; i++) {
                    if(!in) period--; else break;
                }
            } while(period);
            if(*myself == -1) setEeprom(ADDRESS(count));
            else if(*myself == ADDRESS(count)) {
                switch(count & MASK) {
                    case UP: up(); break;
                    case DOWN: down(); break;
                    case REPORT: report(); break;
                    default: stop(); break;
                }
                active = MAX_ACTIVE;
            }
        }
    }
}
