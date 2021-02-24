//must change the makefile to jiffies.o
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/hash.h>
#include <linux/gcd.h>
#include <linux/jiffies.h>

#define BUFFER_SIZE 128
#define PROC_NAME "jiffies"
#define MESSAGE "Jiffies is\n"

/**
 * Function prototypes
 */
ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
        .owner = THIS_MODULE,
        .read = proc_read,
};

int proc_init(void) {
        proc_create(PROC_NAME, 0, NULL, &proc_ops);
        printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
	return 0;
}

void proc_exit(void) {
        remove_proc_entry(PROC_NAME, NULL);
        printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
        int rv = 0;
        char buffer[BUFFER_SIZE];
        static int completed = 0;

        if (completed) {
                completed = 0;
                return 0;
        }

        completed = 1;
        rv = sprintf(buffer, "jiffies is %lu\n" , jiffies);

        // copies the contents of buffer to userspace usr_buf
        copy_to_user(usr_buf, buffer, rv);

        return rv;
}


/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("jiffies Module");
MODULE_AUTHOR("SGG");

//output of jiffies with ./insrem.sh
// jiffies is 4294903840
// seconds is 17179615
// [   46.406149] hello: module verification failed: signature and/or required key missing - tainting kernel
// [   46.406827] /proc/hello created
// [   46.406828] GOLDEN_RATIO_PRIME is: 11400862456688148481
// [   46.406830] In proc_init(), Jiffies is: 4294903835 and HZ is: 250
// [   46.412534] The gcd of 3,300 and 24 is: 12
// [   46.412536] In proc_exit(), jiffies is: 4294903836
// [   46.412538] /proc/hello removed
// [   46.423701] /proc/jiffies created
// [   46.430437] /proc/jiffies removed
// [   46.451145] /proc/seconds created
// [   46.456973] /proc/seconds removed

// ...end of script