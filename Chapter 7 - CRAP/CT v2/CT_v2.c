#define _GNU_SOURCE
#include <sched.h>

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/ioctl.h>
#include "./common.h"
#include <err.h>
#include <string.h>
#include <stdlib.h>

void template();
asm(
        "template:\n\t"

        "pop %rax\n\t"
        "push %rax\n\t"
        ".skip 4089, 0x90\n"
        ".rept 32\n\t"
        "call 1f\n\t"
        ".skip 4091, 0x90\n"
        "1:\n\t"
        ".endr\n\t"
        "mfence\n\t"
        "lfence\n\t"
        "jmp *%rax\n\t"
        #
        
);

#define clflush(addr) asm volatile("clflush (%0)"::"r"(addr):"memory")

int main(int argc, char *argv[])
{   
    //run process on cpu 10
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(10, &mask);
    sched_setaffinity(getpid(), sizeof(mask), &mask);
    

    //check that kernel module is installed
    struct kmod_ret ret;
    int fd_kmod;
    fd_kmod = open("/proc/" PROC_KERNEL_MODULE, O_RDONLY);
    if (fd_kmod <= 0  ) {
        err(1, "You need to install the kernel module 'kernel_mod'\n");
    }
    
    unlink("/dev/shm/my_shm");
    int fd_shm = open("/dev/shm/my_shm", O_CREAT | O_RDWR | O_EXCL, S_IRWXU);
    if (fd_shm < 0) {
       err(1, "open");
    }
    ftruncate(fd_shm, 1ul<<21);
    
    //map template
    mmap((void *)CODE_PTR, 1ul<<24, PROT_EXEC |  PROT_WRITE, MAP_PRIVATE | MAP_FIXED, fd_shm,0);
    memccpy((void *)CODE_PTR, template, '#' , 1ul<<24);
    
    //call template
    asm(
    "callq *%0"
    :
    :
    "r"(PTR)
    );
    asm("add $264, %rsp\n\t");
    
    //switch to kernel space through ioctl call
    while(1) {
        ioctl(fd_kmod, PROC_VAR1, &ret);
    }
}