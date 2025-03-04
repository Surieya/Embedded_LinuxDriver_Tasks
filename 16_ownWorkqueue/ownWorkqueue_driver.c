#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>       // kmalloc()
#include <linux/uaccess.h>    // copy_to/from_user()
#include <linux/interrupt.h>
#include <linux/workqueue.h>  // Workqueue support
#include <linux/input.h>      // Keyboard input handling
#include <asm/io.h>


#define KEYBOARD_IRQ 1  // Keyboard IRQ line

/* Structure for workqueue data */
struct work_data {
    struct work_struct work;
    int keycode;
};

/* Workqueue */
static struct workqueue_struct *custom_wq;

/* Workqueue function */
static void workqueue_function(struct work_struct *work) {
    struct work_data *work_item = container_of(work, struct work_data, work);

    printk(KERN_INFO "Key Pressed: %d\n", work_item->keycode);

    /* Free allocated work item memory */
    kfree(work_item);
}

/* Interrupt handler */
static irqreturn_t irq_handler(int irq, void *dev_id) {
    struct work_data *work_item;
    unsigned char scancode;

    /* Read keyboard scan code */
    scancode = inb(0x60);  // Read from keyboard data port

    printk(KERN_INFO "Keyboard Interrupt: Scan Code = %d\n", scancode);

    /* Allocate memory for work item */
    work_item = kmalloc(sizeof(struct work_data), GFP_KERNEL);
    if (!work_item) {
        printk(KERN_ERR "Failed to allocate memory for work item\n");
        return IRQ_HANDLED;
    }

    /* Initialize work structure and set data */
    INIT_WORK(&work_item->work, workqueue_function);
    work_item->keycode = scancode;

    /* Queue work */
    queue_work(custom_wq, &work_item->work);

    return IRQ_HANDLED;
}

/* Module Initialization */
static int __init workqueue_driver_init(void) {
    int ret;

    /* Create workqueue */
    custom_wq = create_workqueue("custom_wq");
    if (!custom_wq) {
        printk(KERN_ERR "Failed to create workqueue\n");
        return -ENOMEM;
    }

    /* Request IRQ for the keyboard */
    ret = request_irq(KEYBOARD_IRQ, irq_handler, IRQF_SHARED, "keyboard_workqueue", (void *)(irq_handler));
    if (ret) {
        printk(KERN_ERR "Failed to request IRQ %d\n", KEYBOARD_IRQ);
        destroy_workqueue(custom_wq);
        return ret;
    }

    printk(KERN_INFO "Keyboard Workqueue Driver Loaded\n");
    return 0;
}

/* Module Cleanup */
static void __exit workqueue_driver_exit(void) {
    /* Free IRQ */
    free_irq(KEYBOARD_IRQ, (void *)(irq_handler));

    /* Destroy workqueue */
    if (custom_wq) {
        flush_workqueue(custom_wq);
        destroy_workqueue(custom_wq);
    }

    printk(KERN_INFO "Keyboard Workqueue Driver Unloaded\n");
}

module_init(workqueue_driver_init);
module_exit(workqueue_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surieya");
MODULE_DESCRIPTION("Keyboard Interrupt Workqueue Example");
MODULE_VERSION("1.0");
