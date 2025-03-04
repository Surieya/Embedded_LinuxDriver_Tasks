#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/spinlock.h>

DEFINE_SPINLOCK(my_spinlock);
unsigned long shared_variable = 0;

static struct task_struct *thread1;
static struct task_struct *thread2;

int thread_function1(void *data)
{
    while (!kthread_should_stop()) {
        spin_lock(&my_spinlock);
        shared_variable++;
        pr_info("Thread 1: %lu\n", shared_variable);
        spin_unlock(&my_spinlock);
        msleep(1000);
    }
    return 0;
}

int thread_function2(void *data)
{
    while (!kthread_should_stop()) {
        spin_lock(&my_spinlock);
        shared_variable++;
        pr_info("Thread 2: %lu\n", shared_variable);
        spin_unlock(&my_spinlock);
        msleep(1000);
    }
    return 0;
}

static int __init my_driver_init(void)
{
    pr_info("Spinlock Example Module Loaded\n");

    thread1 = kthread_run(thread_function1, NULL, "Thread1");
    if (!thread1)
        pr_err("Failed to create Thread 1\n");

    thread2 = kthread_run(thread_function2, NULL, "Thread2");
    if (!thread2)
        pr_err("Failed to create Thread 2\n");

    return 0;
}

static void __exit my_driver_exit(void)
{
    kthread_stop(thread1);
    kthread_stop(thread2);
    pr_info("Spinlock Example Module Unloaded\n");
}

module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surieya");
MODULE_DESCRIPTION("Simple Spinlock Example");
MODULE_VERSION("1.0");
