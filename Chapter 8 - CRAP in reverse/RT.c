typedef unsigned long u64;
typedef unsigned char u8;

#define _GNU_SOURCE //CPU_ZERO, CPU_SET
#include <sched.h> //sched_setaffinity, cpu_set_t, CPU_ZERO ...
#include <unistd.h> //getpid
#include <stdio.h> //printf
#include <sys/mman.h> //mmap
#include <string.h> //memccpy
#include <fcntl.h> //open
#include <err.h> //err
 
#define CODE_PTR 0x20000000
#define PTR ((u64*)CODE_PTR)

#define _RB_PTR 0x100000000
#define RB_PTR ((u64)_RB_PTR)

#define MONITOR 0x300000000

#define HIT_THRESHOLD 300

void template();
asm (
        "template:\n\t"
        "call A\n\t"
        "A:\n\t"
        "add $8, %rsp\n\t"
        "mov (0x100000000), %rax\n\t"
        "int3\n\t"
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

    //run process on cpu 10
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(10, &mask);
    sched_setaffinity(getpid(), sizeof(mask), &mask);
    
    //map template
    int fd_shm = open("/dev/shm/my_shm", O_RDWR);
    mmap((void *)CODE_PTR, 128, PROT_EXEC |  PROT_WRITE, MAP_PRIVATE | MAP_FIXED, fd_shm,0);
    memcpy((void *)CODE_PTR, template, 128);
    
    //map RB entry
    mmap((void *)RB_PTR, 1ul<<12, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE | MAP_FIXED, -1, 0);
    *(char*)(void *)RB_PTR=0x1; 
    
    //prepare for use of mwait/monitor
    mmap((void *)MONITOR, 1ul<<12, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE | MAP_FIXED, -1, 0);

    for(int j = 0; j < 1; j++) {
      asm volatile(
          //access and flush RB entry
          "mov $0x100000000, %rax\n\t"
          "mfence\n\t"
          "clflush (%rax)\n\t"
          "lfence\n\t"
          
          //using mwait/monitor
          "mov $0x300000000, %rax\n\t"
          "mov $0, %ecx\n\t"
          "mov $0, %edx\n\t"
          "monitor %rax, %ecx, %edx\n\t"  
          "lfence\n\t"    
          "mov $0, %eax\n\t"
          "mov $0x00, %eax\n\t"
          "mfence\n\t"           
          "mwait\n\t"

          //return 31 times
          "mov $31, %r8\n\t"
          "jmp obviously\n"
          
          "A313:\n"
          "add $2, %rdi\n\t"
          "push %rdi\n"
          "clflush (%rsp)\n\t"
          "ret\n"
          
          "obviously:\n"
          "lea (%rip), %rdi\n"
          "jmp A313\n" 

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
    }
    
    //reload RB entry
    volatile unsigned char *d = (u8 *)0x100000000;
    u64 t0 = rdtsc();
    *d;
    u64 dt = rdtscp() - t0;

    //print results
    if(dt < HIT_THRESHOLD) {
      printf("Success\n");
    } else {
      printf("Failure\n");
    }
}