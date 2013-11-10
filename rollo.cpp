#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "avr_count.h"

const char *names[] = {
    "Gäste",
    "Arbeit",
    "Pergola",
    "pergola",
    "Wohnen",
    "Küche",
    "Terasse",
    "terasse",
    "Rechts",
    "Links",
    "Kinder",
    "Flur",
    "Bad",
    "Nord",
    "West",
    "Schlafen",
    "Lukas",
    "Maja"
};
#define PIN 15
/* Calucate the size of an array. */
#define SIZE(name) (sizeof(name) / sizeof(name[0]))
/* Make an array item. */
#define NAME_ITEM(name) #name,
/* Make an array */
#define MAKE_NAME(name) static const char *name[] = {LIST_GENERATOR(name, NAME_ITEM)}
MAKE_NAME(commands);

#define BCM2708_PERI_BASE 0x20000000
#define GPIO_BASE         (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#define ADDRESS(g)        (*(gpio() + ((g) / 10)))
#define SHIFT(g)          (((g) % 10) * 3)

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(g) or SET_GPIO_ALT(g, a).
#define INP(g)    ADDRESS(g) &= ~(7 << SHIFT(g))
#define OUT(g)    INP(g); ADDRESS(g) |=  (1 << SHIFT(g))
#define ALT(g, a) INP(g); ADDRESS(g) |= (((a) <= 3 ? (a) + 4 : (a) == 4 ? 3 : 2) << SHIFT(g))
#define SET       *(gpio() + 7)  // sets   bits which are 1 ignores bits which are 0
#define CLR       *(gpio() + 10) // clears bits which are 1 ignores bits which are 0

volatile int *gpio() {
    static volatile void *ret = MAP_FAILED;
    int fd;
    if(ret == MAP_FAILED) {
        if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) error(-3, errno, "\n");
        ret = mmap(NULL,                   // Any adddress in our space will do
                   getpagesize(),          // Map length
                   PROT_READ | PROT_WRITE, // Enable reading & writing to mapped memory
                   MAP_SHARED,             // Shared with other processes
                   fd,                     // File to map
                   GPIO_BASE               // Offset to GPIO peripheral
                  );
        close(fd); // No need to keep mem_fd open after mmap
        if (ret == MAP_FAILED) error(-4, errno, "\n");
    }
    return (volatile int *) ret;
}

void usage(char *arg) {
    printf("Funktionsweise: %s <Name> <Kommando>\n", arg);
    printf("Verfügbare Namen:");
    for(int i = 0; i < SIZE(names); i++) printf(" %s", names[i]);
    printf("\nVerfügbare Kommandos:");
    for(int i = 0; i < SIZE(commands); i++) printf(" %s", commands[i]);
    printf("\n");
    exit(-1);
}

void pin(bool value) {
    if(value) SET = 1 << PIN;
    else  CLR = 1 << PIN;
    usleep(100);
}

int main(int argc, char **argv) {
    if(argc != 3) usage(argv[0]);
    int position, task;
    for(position = 0; position < SIZE(names); position++) if(!strcmp(argv[1], names[position])) break;
    if(position >= SIZE(names)) usage(argv[0]);
    for(task = 0; task < SIZE(commands); task++) if(!strcmp(argv[2], commands[task])) break;
    if(task >= SIZE(commands)) usage(argv[0]);
    int count = (((OFFSET + position) * (MARGIN + 1)) + task) * oneMoreThanLastEnum + 1;
    OUT(PIN);
    sched_param param;
    param.sched_priority = 99;
    int ret = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
    if(ret) error(1, ret, "\n");
    for(int i = 0; i < count; i++) {
        pin(false);
        pin(true);
    }
    printf("%d\n", count);
    return 0;
}
