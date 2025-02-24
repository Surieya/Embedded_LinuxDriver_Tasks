#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>

#define KEYBOARD_IRQ 1  // IRQ 1 is typically used for keyboards

// Interrupt handler for the keyboard
static irqreturn_t keyboard_irq_handler(int irq, void *dev_id) {
    printk(KERN_INFO "Keyboard Interrupt Detected on IRQ %d!\n", irq);
    return IRQ_HANDLED;
}

static int __init my_driver_init(void) {
    int ret;

    // Request IRQ 1 (keyboard interrupt)
    ret = request_irq(KEYBOARD_IRQ, keyboard_irq_handler, IRQF_SHARED, "keyboard_irq_driver", (void *)keyboard_irq_handler);
    if (ret) {
        printk(KERN_ERR "Failed to request IRQ %d\n", KEYBOARD_IRQ);
        return ret;
    }

    printk(KERN_INFO "Keyboard IRQ handler registered for IRQ %d\n", KEYBOARD_IRQ);
    return 0;
}

static void __exit my_driver_exit(void) {
    free_irq(KEYBOARD_IRQ, (void *)keyboard_irq_handler);
    printk(KERN_INFO "Keyboard IRQ handler unregistered\n");
}

module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surieya");
MODULE_DESCRIPTION("Keyboard Interrupt Handler in Linux Kernel Module");
