#include <linux/module.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>

#define TIMEOUT 5  // Timeout in seconds

static struct timer_list my_timer;  // Timer structure

// Timer callback function
void my_timer_callback(struct timer_list *t) {
    pr_info("Timer expired! Running callback function.\n");

    // Re-arm the timer for another 5 seconds (optional)
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMEOUT * 1000));
}

// Module initialization
static int __init my_timer_init(void) {
    pr_info("Initializing Timer Module\n");

    // Initialize the timer
    timer_setup(&my_timer, my_timer_callback, 0);

    // Set the timer to expire after TIMEOUT seconds
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMEOUT * 1000));

    pr_info("Timer is set for %d seconds.\n", TIMEOUT);
    return 0;
}

// Module exit function
static void __exit my_timer_exit(void) {
    pr_info("Exiting Timer Module\n");

    // Delete the timer
    del_timer(&my_timer);
}

module_init(my_timer_init);
module_exit(my_timer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surieya");
MODULE_DESCRIPTION("Simple Linux Kernel Timer Example");
