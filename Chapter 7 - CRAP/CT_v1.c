typedef unsigned long u64;

#define _GNU_SOURCE
#include <assert.h>
#include <sched.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/io.h>
#include <err.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#define BUF_SZ 0x1000
#define CODE_PTR 0x200000000
#define PTR ((u64*)CODE_PTR)

#define MONITOR  0x10000000

void template();
asm(    
        "template:\n\t"
        "pop %rax\n\t"
        "push %rax\n\t"
        ".skip 4089, 0x90\n"
        ".rept 32\n"
        "call 1f\n"
        ".skip 4091, 0x90\n"
        "1:\n"
        ".endr\n"
        "mfence\n\t"
        "lfence\n\t"
        "jmp *%rax\n\t"
);

int main() {
    //run process on cpu 10
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(10, &mask);
    sched_setaffinity(getpid(), sizeof(mask), &mask);
    
    //initialize template
    if (mmap((void *)CODE_PTR, 1ul<<21, PROT_EXEC | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE | MAP_FIXED_NOREPLACE, -1, 0) == MAP_FAILED) err(1, "mmap");
    if (madvise((void *)CODE_PTR, 1ul<<21, MADV_HUGEPAGE) != 0) {
	    err(2, "madv %p", (void *)CODE_PTR);
    }
    memccpy((void *)CODE_PTR, template, '#' , 1ul << 21);

    //prepare to use mwait/monitor
    mmap((void *)MONITOR, 1ul<<12, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE | MAP_FIXED, -1, 0);
    *(char*)(void *)MONITOR=0x1; 

    while(1) {

        //call template
        asm("callq *%0"
        :
        :
        "r"(PTR)
        );
        asm("add $264, %rsp\n\t");
        
        //using mwait and monitor
        asm volatile(
            "mov $0x10000000, %rax\n\t"
            "xor %ecx, %ecx    \n\t"
            "xor %edx, %edx    \n\t"
            "monitor %rax, %ecx, %edx  \n\t"  
            "lfence\n\t"    
            "xor %eax, %eax\n\t"            
            "mwait\n\t"
        );
    }
}