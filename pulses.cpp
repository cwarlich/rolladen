#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
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

void pin(bool value) {
    if(value) SET = 1 << 4;
    else  CLR = 1 << 4;
    usleep(100);
}
int main(int argc, char **argv) {
    if(argc != 2) {
        printf("Usage: %s <number>\n", argv[0]); 
        exit(-1);
    }
    int number = atoi(argv[1]);
    if(number == 0) {
        printf("Usage: %s <number>\n", argv[0]); 
        exit(-2);
    }
    OUT(4);
    sched_param param;
    param.sched_priority = 99;
    int ret = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
    if(ret) error(1, ret, "\n");
    for(int i = 0; i < number; i++) {
        pin(false);
        pin(true);
    }
    return 0;
}
