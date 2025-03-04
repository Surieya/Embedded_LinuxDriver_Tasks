#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>       // kmalloc()  // copy_to/from_user()
#include <linux/interrupt.h> // Workqueue support
#include <linux/input.h>      // Keyboard input handling


#define KEYBOARD_IRQ 1  // IRQ number for keyboard

void tasklet_fn(unsigned long data);
// DEFINE_TASKLET(my_tasklet, my_tasklet_function, 0);
struct tasklet_struct *tasklet = NULL;

// Interrupt handler for keyboard
irqreturn_t keyboard_irq_handler(int irq, void *dev_id) {
    printk(KERN_INFO "Keyboard interrupt triggered!\n");

    // Schedule tasklet to handle keypress processing
    tasklet_schedule(tasklet);
    
    return IRQ_HANDLED;
}

// Tasklet function to process keypress event
void tasklet_fn(unsigned long data) {
    printk(KERN_INFO "Tasklet executed: Key processing completed\n");
}

// Module initialization function
static int __init my_module_init(void) {
    int ret;
    
    // Request IRQ for keyboard
    ret = request_irq(KEYBOARD_IRQ, keyboard_irq_handler, IRQF_SHARED, "keyboard_tasklet", (void *)(keyboard_irq_handler));
    if (ret) {
        printk(KERN_ERR "Failed to request IRQ %d\n", KEYBOARD_IRQ);
        return ret;
    }
    tasklet  = kmalloc(sizeof(struct tasklet_struct),GFP_KERNEL);
        if(tasklet == NULL) {
            printk(KERN_INFO "tasklet_device: cannot allocate Memory");
            free_irq(KEYBOARD_IRQ,(void *)(keyboard_irq_handler));
        }
        tasklet_init(tasklet,tasklet_fn,0);
    
    printk(KERN_INFO "Keyboard tasklet module loaded\n");
    return 0;
}

// Module exit function
static void __exit my_module_exit(void) {
    // Free IRQ and kill tasklet before unloading module
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_irq_handler));
    tasklet_kill(tasklet);
    
    printk(KERN_INFO "Keyboard tasklet module unloaded\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surieya");
MODULE_DESCRIPTION("Keyboard IRQ Handler with Tasklet");
