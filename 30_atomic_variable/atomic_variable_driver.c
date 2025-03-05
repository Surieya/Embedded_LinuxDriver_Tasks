#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/atomic.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#define NUM_THREADS 3  // Number of threads

static atomic_t shared_counter = ATOMIC_INIT(0);  // Atomic variable
static struct task_struct *threads[NUM_THREADS];  // Thread pointers

// Function executed by kernel threads
static int thread_function(void *arg) {
    int thread_id = *(int *)arg;
    
    while (!kthread_should_stop()) {  // Loop until the thread is stopped
        atomic_inc(&shared_counter);  // Atomically increment counter
        printk(KERN_INFO "Thread %d incremented counter: %d\n", thread_id, atomic_read(&shared_counter));
        msleep(1000);  // Simulate work (1 second delay)
    }
    
    printk(KERN_INFO "Thread %d exiting\n", thread_id);
    return 0;
}

// Module initialization
static int __init atomic_thread_init(void) {
    int i;
    
    printk(KERN_INFO "Atomic Thread Example Loaded\n");

    // Create and start threads
    for (i = 0; i < NUM_THREADS; i++) {
        int *thread_id = kmalloc(sizeof(int), GFP_KERNEL);
        if (!thread_id) return -ENOMEM;
        
        *thread_id = i;  // Assign thread ID
        threads[i] = kthread_run(thread_function, (void *)thread_id, "atomic_thread_%d", i);
        
        if (IS_ERR(threads[i])) {
            printk(KERN_ERR "Failed to create thread %d\n", i);
            kfree(thread_id);
            return PTR_ERR(threads[i]);
        }
    }
    
    return 0;
}

// Module exit
static void __exit atomic_thread_exit(void) {
    int i;

    printk(KERN_INFO "Stopping threads...\n");
    
    for (i = 0; i < NUM_THREADS; i++) {
        if (threads[i]) {
            kthread_stop(threads[i]);  // Stop thread
        }
    }

    printk(KERN_INFO "Atomic Thread Example Unloaded\n");
}

module_init(atomic_thread_init);
module_exit(atomic_thread_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surieya");
MODULE_DESCRIPTION("Kernel module using atomic variable with threads");
