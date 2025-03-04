#include <linux/module.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

#define TIMEOUT_MS 1000  // Timeout in milliseconds (1 second)

static struct hrtimer my_hrtimer;
static ktime_t kt_period;

// Timer callback function
enum hrtimer_restart my_hrtimer_callback(struct hrtimer *timer) {
    pr_info("High-Resolution Timer expired! Restarting...\n");

    // Restart the timer
    hrtimer_forward_now(timer, kt_period);
    return HRTIMER_RESTART;
}

// Module initialization
static int __init my_hrtimer_init(void) {
    pr_info("Initializing High-Resolution Timer Module\n");

    // Set timer period in nanoseconds (1 ms = 1,000,000 ns)
    kt_period = ktime_set(0, TIMEOUT_MS * 1000000);  

    // Initialize and start the high-resolution timer
    hrtimer_init(&my_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    my_hrtimer.function = my_hrtimer_callback;
    hrtimer_start(&my_hrtimer, kt_period, HRTIMER_MODE_REL);

    pr_info("High-Resolution Timer started for %d ms.\n", TIMEOUT_MS);
    return 0;
}

// Module exit function
static void __exit my_hrtimer_exit(void) {
    pr_info("Stopping High-Resolution Timer Module\n");

    // Cancel the timer if it's still running
    hrtimer_cancel(&my_hrtimer);
}

module_init(my_hrtimer_init);
module_exit(my_hrtimer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surieya");
MODULE_DESCRIPTION("Simple High-Resolution Timer Example in Linux Kernel");
