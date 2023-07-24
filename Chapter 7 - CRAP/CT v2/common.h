typedef unsigned long m64;
typedef unsigned char m8;

#define PROC_KERNEL_MODULE "kmod_v2"

#define PROC_VAR1 100

#define BUF_SZ 0x1000
#define CODE_PTR 0x200000000
#define PTR ((m64*)CODE_PTR)

#define MONITOR 0x10000000

#define RB 0x100000000
#define RB_PTR ((m64)RB)
#define RB_SZ 0x1000
#define RB_ENTRIES 0x20

struct kmod_ret {
  m64 time;
};

// start measure.
static __always_inline m64 rdtsc_h(void) {
    m64 lo, hi;
    asm volatile ("CPUID\n\t"
            "RDTSC\n\t"
            "movq %%rdx, %0\n\t"
            "movq %%rax, %1\n\t" : "=r" (hi), "=r" (lo)::
            "%rax", "%rbx", "%rcx", "%rdx");
    return (hi << 32) | lo;
}

// stop meassure.
static __always_inline m64 rdtscp_h(void) {
    m64 lo, hi;
    asm volatile("RDTSCP\n\t"
            "movq %%rdx, %0\n\t"
            "movq %%rax, %1\n\t"
            "CPUID\n\t": "=r" (hi), "=r" (lo):: "%rax",
            "%rbx", "%rcx", "%rdx");
    return (hi << 32) | lo;
}