#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mutex.h>
#include <linux/kthread.h>  // Kernel threads
#include <linux/delay.h>

static DEFINE_MUTEX(my_mutex);
static struct task_struct *thread1;
static struct task_struct *thread2;

int my_thread_fn(void *data) {
    while (!kthread_should_stop()) {
        mutex_lock(&my_mutex);
        printk(KERN_INFO "Thread %s: Acquired lock\n", current->comm);
        msleep(1000);  // Simulate work
        printk(KERN_INFO "Thread %s: Releasing lock\n", current->comm);
        mutex_unlock(&my_mutex);
        msleep(500);  // Simulate delay
    }
    return 0;
}

static int __init my_module_init(void) {
    printk(KERN_INFO "Mutex Example: Initializing\n");
    thread1 = kthread_run(my_thread_fn, NULL, "Thread_1");
    thread2 = kthread_run(my_thread_fn, NULL, "Thread_2");
    return 0;
}

static void __exit my_module_exit(void) {
    kthread_stop(thread1);
    kthread_stop(thread2);
    printk(KERN_INFO "Mutex Example: Exiting\n");
}

module_init(my_module_init);
module_exit(my_module_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Mutex Example in Linux Kernel");
MODULE_AUTHOR("Your Name");
