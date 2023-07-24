typedef unsigned long u64;

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

//set all RSB entries to A+9?
void template();
asm(
        "template:\n\t"
        "A:\n\t"
        "add $8, %rsp\n\t"
        "call A\n\t"
);

int main() {

    //run process on cpu 11
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(11, &mask);
    sched_setaffinity(getpid(), sizeof(mask), &mask);
    
    unlink("/dev/shm/my_shm");
    int fd_shm = open("/dev/shm/my_shm", O_CREAT | O_RDWR | O_EXCL, S_IRWXU);
    if (fd_shm < 0) {
        err(1, "open");
    }
    ftruncate(fd_shm, 10);

    //map template
    mmap((void *)CODE_PTR, 12, PROT_EXEC |  PROT_WRITE, MAP_PRIVATE | MAP_FIXED, fd_shm,0);
    memcpy((void *)CODE_PTR, template, 12);

    //call template
    asm("callq *%0"
    :
    :
    "r"(PTR)
    );
}

