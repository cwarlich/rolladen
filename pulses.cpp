#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <error.h>
#include <unistd.h>
void pin(bool value) {
    if(value) printf("High\n");
    else printf("Low\n");
    usleep(1000000);
}
int main(int argc, char **argv) {
    if(argc != 2) {
        printf("Usage: %s <number>\n", argv[0]); 
    }
    int number = atoi(argv[1]);
    if(number == 0) {
        printf("Usage: %s <number>\n", argv[0]); 
    }
    sched_param param;
    param.sched_priority = 99;
    int ret = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
    if(ret) error(1, ret, "\n");
    for(int i = 0; i < number; i++) {
        pin(true);
        pin(false);
    }
    return 0;
}
