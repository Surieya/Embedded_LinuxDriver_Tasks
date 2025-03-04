/***************************************************************************//**
*  \file       key_interrupt_driver.c
*
*  \details    Linux device driver using keyboard interrupt (IRQ 1)
*              Workqueue executes only when the "I" key is pressed.
*
*  \author     Generic Developer
*
*******************************************************************************/

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>        // kmalloc()
#include <linux/uaccess.h>     // copy_to/from_user()
#include <linux/sysfs.h> 
#include <linux/kobject.h> 
#include <linux/interrupt.h>   // For interrupts
#include <linux/workqueue.h>   // Workqueues
#include <linux/err.h>
#include <asm/io.h>            // For inb()

#define IRQ_NO 1               // Keyboard Interrupt
#define KEY_I_SCANCODE 0x17    // Scancode for "I" key

void workqueue_fn(struct work_struct *work);

/* Creating workqueue statically */
DECLARE_WORK(workqueue, workqueue_fn);

/* Workqueue Function */
void workqueue_fn(struct work_struct *work) {
    printk(KERN_INFO "Workqueue Executed: 'I' key was pressed!\n");
}

/* Interrupt handler for IRQ 1 (Keyboard) */
static irqreturn_t irq_handler(int irq, void *dev_id) {
    unsigned char scancode;
    
    // Read the scancode from I/O port 0x60
    scancode = inb(0x60);

    // Check if the scancode corresponds to the "I" key
    if (scancode == KEY_I_SCANCODE) {
        printk(KERN_INFO "Detected 'I' key press: Scheduling Workqueue\n");
        //scheduling the work
        schedule_work(&workqueue);
    }

    return IRQ_HANDLED;
}

dev_t dev = 0;
static struct class *dev_class;
static struct cdev key_cdev;
struct kobject *kobj_ref;

/*
** Function Prototypes
*/
static int __init key_driver_init(void);
static void __exit key_driver_exit(void);

/*************** Driver Functions **********************/
static int key_open(struct inode *inode, struct file *file);
static int key_release(struct inode *inode, struct file *file);
static ssize_t key_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t key_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);

/*
** File operation structure
*/
static struct file_operations fops =
{
    .owner   = THIS_MODULE,
    .read    = key_read,
    .write   = key_write,
    .open    = key_open,
    .release = key_release,
};

/*
** Open function
*/  
static int key_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device File Opened\n");
    return 0;
}

/*
** Release function
*/  
static int key_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device File Closed\n");
    return 0;
}

/*
** Read function
*/
static ssize_t key_read(struct file *filp, char __user *buf, size_t len, loff_t *off) {
    printk(KERN_INFO "Read function: Press 'I' to trigger workqueue\n");
    return 0;
}

/*
** Write function
*/
static ssize_t key_write(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
    printk(KERN_INFO "Write Function\n");
    return len;
}

/*
** Module Init function
*/
static int __init key_driver_init(void) {
    /* Allocating Major number */
    if ((alloc_chrdev_region(&dev, 0, 1, "key_driver")) < 0) {
        printk(KERN_INFO "Cannot allocate major number\n");
        return -1;
    }
    printk(KERN_INFO "Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

    /* Creating cdev structure */
    cdev_init(&key_cdev, &fops);

    /* Adding character device to the system */
    if ((cdev_add(&key_cdev, dev, 1)) < 0) {
        printk(KERN_INFO "Cannot add the device to the system\n");
        goto r_class;
    }

    /* Creating struct class */
    if (IS_ERR(dev_class = class_create("key_class"))) {
        printk(KERN_INFO "Cannot create the struct class\n");
        goto r_class;
    }

    /* Creating device */
    if (IS_ERR(device_create(dev_class, NULL, dev, NULL, "key_device"))) {
        printk(KERN_INFO "Cannot create the Device\n");
        goto r_device;
    }

    /* Request IRQ 1 (Keyboard Interrupt) */
    if (request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "key_device", (void *)(irq_handler))) {
        printk(KERN_INFO "Cannot register IRQ 1\n");
        goto irq;
    }

    printk(KERN_INFO "Device Driver Inserted Successfully!\n");
    return 0;

irq:
    free_irq(IRQ_NO, (void *)(irq_handler));

r_device:
    class_destroy(dev_class);

r_class:
    unregister_chrdev_region(dev, 1);
    cdev_del(&key_cdev);
    return -1;
}

/*
** Module Exit function
*/ 
static void __exit key_driver_exit(void) {
    free_irq(IRQ_NO, (void *)(irq_handler));
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&key_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "Device Driver Removed Successfully!\n");
}

module_init(key_driver_init);
module_exit(key_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Generic Developer");
MODULE_DESCRIPTION("Linux Device Driver using Keyboard Interrupt (IRQ 1) and Workqueue for 'I' Key");
MODULE_VERSION("1.0");
