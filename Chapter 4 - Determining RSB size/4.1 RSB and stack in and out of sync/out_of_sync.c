//gcc out_of_sync_v2.c && ./a.out

typedef unsigned long u64;

#define _GNU_SOURCE //CPU_ZERO, CPU_SET
#include <sched.h> //sched_setaffinity, cpu_set_t, CPU_ZERO ...
#include <unistd.h> //getpid
#include <stdio.h> //printf

#define HIT_THRESHOLD 300
#define ITERATIONS 10

int main() {

    /*set thread to CPU 11, because it is less likely that an other 
    thread runs on its sibling, here 10*/
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(11, &mask);
    sched_setaffinity(getpid(), sizeof(mask), &mask);
    
    //variable to count the successful iterations 
    u64 successes = 0;

    for (int i = 0; i < ITERATIONS; i++) {

        //read values out of rdx and rax registers (RDTSC fills them)
        u64 hi1, hi2, lo1, lo2;
        
        asm volatile(
            //make sure we don't 'leak' time between iterations
            "cpuid\n"
            "mfence\n"
            "lfence\n"
            "sfence\n"

            //first call
            "call C1\n"
            //returning point, second timestamp
            "RDTSC\n\t"
            "movq %%rdx, %2\n\t"
            "movq %%rax, %3\n\t"
            "jmp exit\n"

            "C1:\n"
            //second call
            "call C2\n"
            /*if it would return here, you would see
            a message in the terminal like:
            Trace/breakpoint trap*/
            "int3\n"

            "C2:\n"
            //add make sure we return after call C1
            "add $8, %%rsp\n"
            //flush rsp
            "clflush (%%rsp)\n"
            //wait for flush to finish
            "mfence\n"
            //first timestamp
            "RDTSCP\n\t"
            "movq %%rdx, %0\n\t"
            "movq %%rax, %1\n\t"
            //RSB and stack out of sync as RSB points to line 57 and stack points to line 46
            "ret\n"

            "exit:\n"
            : "=r" (hi1), "=r" (lo1), "=r" (hi2), "=r" (lo2)
            : 
            : "%rax", "%rdi", "%rsi", "%rdx"
        );

        //RDTSC fills rdx with the higher 32 bits and rax with lower 32 bits
        u64 result = ((hi2 << 32) | lo2) - ((hi1 << 32) | lo1);
        if(result < 300) {
            successes+=1;
        }
    }
    
    //print result in percent
    printf("Success rate: %f %%\n", (double)successes/ITERATIONS*100);
}