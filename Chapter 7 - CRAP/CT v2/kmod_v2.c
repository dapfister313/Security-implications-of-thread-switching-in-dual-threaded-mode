#include <linux/fs.h>
#include "linux/sysctl.h"
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/uaccess.h>
#include "./kmod.h"

static struct proc_dir_entry *procfs_file;

struct kmod_ret ret;

//ioctl handler
static long handle_ioctl(struct file *filp, unsigned int request, unsigned long proc_arg) {
    printk(KERN_INFO "Ioctl call successfull!\n");
    if (request == PROC_VAR1) {

        //call template
        asm(
        "callq *%0"
        :
        :
        "r"(PTR)
        );
        asm("add $264, %rsp\n\t");

        //halt thread
        asm("HLT");
    }
    return 0;
}


static struct proc_ops pops = {
    .proc_ioctl = handle_ioctl,
    .proc_open = nonseekable_open,
    .proc_lseek = no_llseek,
};

static int kmod_init(void) {
    printk(KERN_INFO "Module initialized\n");
    procfs_file = proc_create(PROC_KERNEL_MODULE, 0, NULL, &pops);
    return 0;
}

static void kmod_exit(void) {
    printk(KERN_INFO "Module removed\n");
    proc_remove(procfs_file);
}

module_init(kmod_init);
module_exit(kmod_exit);

MODULE_LICENSE("GPL");