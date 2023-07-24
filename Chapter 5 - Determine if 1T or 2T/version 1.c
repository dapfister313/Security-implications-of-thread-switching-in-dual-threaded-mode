typedef unsigned long u64;
#define __stringify_1(x...)	#x
#define __stringify(x...)	__stringify_1(x)

#define _GNU_SOURCE //CPU_ZERO, CPU_SET
#include <sched.h> //sched_setaffinity, cpu_set_t, CPU_ZERO ...
#include <unistd.h> //getpid
#include <stdio.h> //printf

#define HIT_THRESHOLD 300
#define ITERATIONS 10000

int main() {

    /*set thread to CPU 11, because it is less likely that an other 
    thread runs on its sibling, here 10*/
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(4, &mask);
    sched_setaffinity(getpid(), sizeof(mask), &mask);
    
    //variable to count the successful iterations 
    u64 successes = 0;

    for(int i = 0; i < ITERATIONS; i++) {

        //read values out of rdx and rax registers (RDTSC fills them)
        u64 hi1, hi2, lo1, lo2;
        
        /* call N times */
        asm volatile(

            //make sure we don't 'leak' time between iterations
            "cpuid\n"
            "mfence\n"
            "lfence\n"
            "sfence\n"

            "call C1\n"
            //second timestamp
            "RDTSC\n\t"
            "movq %%rdx, %2\n\t"
            "movq %%rax, %3\n\t"
            "jmp exit\n"

            "C1:\n"
            "call C2\n"
            //flush rsp
            "clflush (%%rsp)\n"
            //wait for flush to finish
            "mfence\n"
            //first timestamp
            "RDTSCP\n\t"
            "movq %%rdx, %0\n\t"
            "movq %%rax, %1\n\t"
            "ret\n"
            
            "C2:\n"
            ".rept 15\n" 
            "call 1f\n"
            "mfence\n"          
            "ret\n"            
            "1:\n" 
            ".endr\n"  
                     
            "ret\n"
            
            "exit:\n"
            : "=r" (hi1), "=r" (lo1), "=r" (hi2), "=r" (lo2)
            : 
            : "%rax", "%rdi", "%rsi", "%rdx"
        );

        //RDTSC fills rdx with the higher 32 bits and rax with lower 32 bits
        u64 result = ((hi2 << 32) | lo2) - ((hi1 << 32) | lo1);
        
        if(result < HIT_THRESHOLD) {
            successes+=1;
        }
    }

    //print result in percent
    printf("Success rate: %f %%\n", (double)successes/ITERATIONS*100);
    
}