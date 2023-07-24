typedef unsigned long u64;
typedef unsigned char u8;

#define _GNU_SOURCE //CPU_ZERO, CPU_SET
#include <sched.h> //sched_setaffinity, cpu_set_t, CPU_ZERO ...
#include <unistd.h> //getpid
#include <stdio.h> //printf
#include <err.h> //err
#include <sys/mman.h> //mmap, PROT_EXEC, ...
#include <string.h> //memccpy

#define HIT_THRESHOLD 300
#define ITERATIONS 10000

#define CODE_PTR 0x200000000
#define PTR ((u64*)CODE_PTR)

#define RB 0x100000000
#define RB_PTR ((u64)RB)
#define RB_SZ 0x1000
#define RB_ENTRIES 0x1f

void template();
asm(    
        "template:\n\t"
        "pop %rax\n\t"
        "push %rax\n\t"
        "call C0\n"
        "mov (0x100000000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C0:\n"
        "call C1\n"
        "mov (0x100001000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C1:\n"
        "call C2\n"
        "mov (0x100002000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C2:\n"
        "call C3\n"
        "mov (0x100003000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C3:\n"
        "call C4\n"
        "mov (0x100004000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C4:\n"
        "call C5\n"
        "mov (0x100005000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C5:\n"
        "call C6\n"
        "mov (0x100006000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C6:\n"
        "call C7\n"
        "mov (0x100007000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C7:\n"
        "call C8\n"
        "mov (0x100008000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C8:\n"
        "call C9\n"
        "mov (0x100009000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C9:\n"
        "call C10\n"
        "mov (0x10000a000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C10:\n"
        "call C11\n"
        "mov (0x10000b000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C11:\n"
        "call C12\n"
        "mov (0x10000c000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C12:\n"
        "call C13\n"
        "mov (0x10000d000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C13:\n"
        "call C14\n"
        "mov (0x10000e000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C14:\n"
        "call C15\n"
        "mov (0x10000f000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C15:\n"
        "call C16\n"
        "mov (0x100010000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C16:\n"
        "call C17\n"
        "mov (0x100011000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C17:\n"
        "call C18\n"
        "mov (0x100012000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C18:\n"
        "call C19\n"
        "mov (0x100013000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C19:\n"
        "call C20\n"
        "mov (0x100014000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C20:\n"
        "call C21\n"
        "mov (0x100015000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C21:\n"
        "call C22\n"
        "mov (0x100016000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C22:\n"
        "call C23\n"
        "mov (0x100017000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C23:\n"
        "call C24\n"
        "mov (0x100018000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C24:\n"
        "call C25\n"
        "mov (0x100019000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C25:\n"
        "call C26\n"
        "mov (0x10001a000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C26:\n"
        "call C27\n"
        "mov (0x10001b000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C27:\n"
        "call C28\n"
        "mov (0x10001c000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C28:\n"
        "call C29\n"
        "mov (0x10001d000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C29:\n"
        "call C30\n"
        "mov (0x10001e000), %rax\n"
        "int3\n"
        ".skip 4091, 0x90\n"
        "C30:\n"
        "jmp *%rax\n\t"
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
    
    //count how many times we are in 1T mode
    u64 successes = 0;

    //run process on cpu 4
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(4, &mask);
    sched_setaffinity(getpid(), sizeof(mask), &mask);
    
    //map the template
    if (mmap((void *)CODE_PTR, 1ul<<21, PROT_EXEC | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE | MAP_FIXED_NOREPLACE, -1, 0) == MAP_FAILED) err(1, "mmap");
    if (madvise((void *)CODE_PTR, 1ul<<21, MADV_HUGEPAGE) != 0) {
	    err(2, "madv %p", (void *)CODE_PTR);
    }
    memccpy((void *)CODE_PTR, template, '#' , 1ul << 21);
    
    //map reload buffer entries
    mmap((void *)RB_PTR, 1ul<<17, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE | MAP_FIXED, -1, 0);

    for(int j = 0; j < ITERATIONS; j++) {

        //call template
        asm("callq *%0"
        :
        :
        "r"(PTR)
        );
        asm("add $256, %rsp\n\t");

        //flush every RB entry
        for (u64 k = 0; k < RB_ENTRIES; ++k) {
            asm("mfence");

            volatile void *p = (u8 *)RB_PTR + (RB_SZ * k);
            __asm__ volatile("clflushopt (%0)\n"::"r"(p));
        
            asm("lfence");
        }

        //return 31 times
        asm volatile(
            "mov $30, %r8\n\t"
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
        
        //reload RB entries
        for (u64 k = 0; k < RB_ENTRIES; ++k) {
            u64 c = (k*7+9)%(RB_ENTRIES);
            unsigned volatile char *p = (u8 *)RB_PTR + (RB_SZ * c);
            u64 t0 = rdtsc();
            *(volatile unsigned char *)p;
            u64 dt = rdtscp() - t0;
            if (dt < HIT_THRESHOLD) results[c]++;
        }
    
        u64 sum = 0;
        for (int i = 0; i < RB_ENTRIES/2; ++i) {
            sum += results[i];
        }
        if (sum != 0) {
            successes++;
        }

    }

    //print success rate
    printf("Success rate: %f %%\n", (double)successes/ITERATIONS*100);
}