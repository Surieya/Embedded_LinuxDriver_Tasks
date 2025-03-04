#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/rwlock.h>

rwlock_t my_rwlock;    // Declare the Read-Write Spinlock
unsigned long shared_variable = 0;  // Shared Resource

static struct task_struct *reader_thread;
static struct task_struct *writer_thread;

/*
** Reader Thread Function
*/
int reader_function(void *data)
{
    while (!kthread_should_stop()) {
        read_lock(&my_rwlock);  // Acquire Read Lock
        pr_info("Reader Thread: Read Value = %lu\n", shared_variable);
        read_unlock(&my_rwlock);  // Release Read Lock
        msleep(500);  // Sleep for 500ms
    }
    return 0;
}

/*
** Writer Thread Function
*/
int writer_function(void *data)
{
    while (!kthread_should_stop()) {
        write_lock(&my_rwlock);  // Acquire Write Lock
        shared_variable++;
        pr_info("Writer Thread: Updated Value = %lu\n", shared_variable);
        write_unlock(&my_rwlock);  // Release Write Lock
        msleep(1000);  // Sleep for 1 second
    }
    return 0;
}

/*
** Module Initialization
*/
static int __init my_driver_init(void)
{
    pr_info("Read-Write Spinlock Example Loaded\n");
    
    rwlock_init(&my_rwlock);  // Initialize Read-Write Spinlock

    reader_thread = kthread_run(reader_function, NULL, "reader_thread");
    if (!reader_thread)
        pr_err("Failed to create Reader Thread\n");

    writer_thread = kthread_run(writer_function, NULL, "writer_thread");
    if (!writer_thread)
        pr_err("Failed to create Writer Thread\n");

    return 0;
}

/*
** Module Exit
*/
static void __exit my_driver_exit(void)
{
    kthread_stop(reader_thread);
    kthread_stop(writer_thread);
    pr_info("Read-Write Spinlock Example Unloaded\n");
}

module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anonymous");
MODULE_DESCRIPTION("Read-Write Spinlock Example");
MODULE_VERSION("1.0");

