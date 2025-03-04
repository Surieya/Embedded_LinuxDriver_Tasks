#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/wait.h>   // For wait queues
#include <linux/sched.h>  // For TASK_INTERRUPTIBLE

#define DEVICE_NAME "wait_wake_dev"

static dev_t dev;
static struct cdev my_cdev;
static struct class *my_class;
static wait_queue_head_t wait_queue;
static int flag = 0;  // Condition flag

/* Open function */


/* Read function - puts process to sleep until flag is set */
static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    pr_info("Read called: Going to sleep...\n");

    // Wait until flag becomes nonzero (event occurs)
    wait_event_interruptible(wait_queue, flag != 0);

    pr_info("Read woke up! Returning data to user\n");

    flag = 0;  // Reset flag after wake-up
    return 0;
}

/* Write function - wakes up process sleeping in read */
static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *offset) {
    pr_info("Write called: Waking up readers...\n");
    
    flag = 1;  // Set condition to true
    wake_up_interruptible(&wait_queue);  // Wake up process waiting in read

    return len;
}

/* Close function */


/* File operations structure */
static struct file_operations fops = {
    .read = my_read,
    .write = my_write,
  
};

/* Module initialization */
static int __init my_driver_init(void) {
    // Allocate device number
    if (alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME) < 0) {
        pr_err("Failed to allocate device number\n");
        return -1;
    }

    // Initialize cdev
    cdev_init(&my_cdev, &fops);
    if (cdev_add(&my_cdev, dev, 1) < 0) {
        pr_err("Failed to add cdev\n");
        goto r_class;
    }

    // Create device class
    my_class = class_create(THIS_MODULE, "wait_wake_class");
    if (IS_ERR(my_class)) {
        pr_err("Failed to create class\n");
        goto r_class;
    }

    // Create device file in /dev
    if (IS_ERR(device_create(my_class, NULL, dev, NULL, DEVICE_NAME))) {
        pr_err("Failed to create device\n");
        goto r_device;
    }

    // Initialize wait queue
    init_waitqueue_head(&wait_queue);

    pr_info("Wait & Wake Queue Driver Loaded\n");
    return 0;

r_device:
    class_destroy(my_class);
r_class:
    unregister_chrdev_region(dev, 1);
    return -1;
}

/* Module cleanup */
static void __exit my_driver_exit(void) {
    device_destroy(my_class, dev);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Wait & Wake Queue Driver Removed\n");
}

module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surieya");
MODULE_DESCRIPTION("Simple Wait & Wake Queue Driver Without Kernel Thread");
