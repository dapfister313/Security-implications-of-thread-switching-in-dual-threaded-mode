typedef unsigned long u64;
typedef unsigned char u8;

#define _GNU_SOURCE //CPU_ZERO, CPU_SET
#include <sched.h> //sched_setaffinity, cpu_set_t, CPU_ZERO ...
#include <unistd.h> //getpid
#include <stdio.h> //printf
#include <sys/mman.h> //mmap
#include <string.h> //memccpy
 
#define CODE_PTR 0x200000000
#define PTR ((u64*)CODE_PTR)

#define RB 0x100000000
#define RB_PTR ((u64)RB)
#define RB_SZ 0x1000
#define RB_ENTRIES 0x20

#define HIT_THRESHOLD 300
#define ITERATIONS 10000

void template();
asm (
        "template:\n\t"
        ".skip 4096, 0x90\n"
        "mov (0x100000000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x100001000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x100002000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x100003000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x100004000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x100005000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x100006000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x100007000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x100008000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x100009000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x10000a000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x10000b000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x10000c000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x10000d000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x10000e000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x10000f000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x100010000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x100011000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x100012000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x100013000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x100014000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x100015000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x100016000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x100017000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x100018000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x100019000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x10001a000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x10001b000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x10001c000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x10001d000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x10001e000), %rax\n\t"
        "int3\n\t"
        ".skip 4085, 0x90\n"
        "mov (0x10001f000), %rax\n\t"
        "int3\n\t"
        #
        
);

// start measure.
static __always_inline u64 rdtsc(void) {
    u64 lo, hi;
    asm volatile ("CPUID\n\t"
            "RDTSC\n\t"
            "movq %%rdx, %0\n\t"
            "movq %%rax, %1\n\t" : "=r" (hi), "=r" (lo)::
            "%rax", "%rbx", "%rcx", "%rdx");
    return (hi << 32) | lo;
}

// stop meassure.
static __always_inline u64 rdtscp(void) {
    u64 lo, hi;
    asm volatile("RDTSCP\n\t"
            "movq %%rdx, %0\n\t"
            "movq %%rax, %1\n\t"
            "CPUID\n\t": "=r" (hi), "=r" (lo):: "%rax",
            "%rbx", "%rcx", "%rdx");
    return (hi << 32) | lo;
}

int main() {
    u64 results[RB_ENTRIES];
    for(int j = 0; j < RB_ENTRIES; j++) {
        results[j] = 0;
    }
    
    //run process on cpu 4
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(4, &mask);
    sched_setaffinity(getpid(), sizeof(mask), &mask);
    
    //map template
    mmap((void *)CODE_PTR, 1ul<<21, PROT_EXEC |  PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE | MAP_FIXED, -1,0);
    memccpy((void *)CODE_PTR, template, '#' , 1ul << 21);
    
    //map RB
    mmap((void *)RB_PTR, 1ul<<17, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE | MAP_FIXED, -1, 0);
    
    for(int j = 0; j < ITERATIONS; j++) {

        //flush every RB entry
        for (u64 k = 0; k < RB_ENTRIES; ++k) {
            asm("mfence");

            volatile void *p = (u8 *)RB_PTR + (RB_SZ * k);
            __asm__ volatile("clflushopt (%0)\n"::"r"(p));
        
            asm("lfence");
        }
        
        //return 100 times
        asm volatile(
            "mov $100, %r8\n\t"
            "jmp obviously\n"
            
            "A:\n"
            "add $2, %rdi\n\t"
            "push %rdi\n"
            "clflush (%rsp)\n\t"
            "ret\n"
            
            "obviously:\n"
            "lea (%rip), %rdi\n"
            "jmp A\n" 

            "cmp $0, %r8\n\t"
            "jz exit\n\t"
            "sub $1, %r8\n\t"
            "sub $8, %rsp\n\t"
            "clflush (%rsp)\n\t"
            "mfence\n\t"
            ".skip 1111, 0x90\n\t"
            "ret\n"

            "exit:\n\t"
        );
        
        //relaod every RB entry
        for (u64 k = 0; k < RB_ENTRIES; ++k) {
            u64 c = (k*13+9)&(RB_ENTRIES-1);
            unsigned volatile char *p = (u8 *)RB_PTR + (RB_SZ * c);
            u64 t0 = rdtsc();
            *(volatile unsigned char *)p;
            u64 dt = rdtscp() - t0;
            if (dt < HIT_THRESHOLD) results[c]++;
        }
    }

    //print results
    for (int i = 0; i < RB_ENTRIES; ++i) {
        printf("%ld ", results[i]);
    }
    printf("\n");
}


