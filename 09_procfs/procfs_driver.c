#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define PROC_DIR "myprocdir"
#define PROC_MESSAGE "message"
#define PROC_STATUS "status"

static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_message_entry;
static struct proc_dir_entry *proc_status_entry;

#define MSG_SIZE 100
static char message[MSG_SIZE] = "Hello from Kernel!";
static int message_len = 0;

/* Read function for /proc/myprocdir/message */
static ssize_t message_read(struct file *file, char __user *user_buf, size_t len, loff_t *offset) {
    if (*offset > 0) return 0;  // EOF

    if (copy_to_user(user_buf, message, message_len))
        return -EFAULT;

    *offset += message_len;
    return message_len;
}

/* Write function for /proc/myprocdir/message */
static ssize_t message_write(struct file *file, const char __user *user_buf, size_t len, loff_t *offset) {
    if (len > MSG_SIZE - 1) return -EINVAL;

    if (copy_from_user(message, user_buf, len))
        return -EFAULT;

    message[len] = '\0';
    message_len = len;

    pr_info("Proc Write: New Message = %s\n", message);
    return len;
}

/* Read function for /proc/myprocdir/status */
static ssize_t status_read(struct file *file, char __user *user_buf, size_t len, loff_t *offset) {
    char status_msg[] = "Kernel Module is running!\n";
    int status_len = sizeof(status_msg);

    if (*offset > 0) return 0;  // EOF

    if (copy_to_user(user_buf, status_msg, status_len))
        return -EFAULT;

    *offset += status_len;
    return status_len;
}

/* File operations structure */
static const struct proc_ops message_fops = {
    .proc_read  = message_read,
    .proc_write = message_write,
};

static const struct proc_ops status_fops = {
    .proc_read  = status_read,
};

/* Module Init Function */
static int __init proc_example_init(void) {
    // Create /proc/myprocdir
    proc_dir = proc_mkdir(PROC_DIR, NULL);
    if (!proc_dir) {
        pr_err("Failed to create /proc/%s\n", PROC_DIR);
        return -ENOMEM;
    }

    // Create /proc/myprocdir/message (read/write)
    proc_message_entry = proc_create(PROC_MESSAGE, 0666, proc_dir, &message_fops);
    if (!proc_message_entry) {
        remove_proc_entry(PROC_DIR, NULL);
        pr_err("Failed to create /proc/%s/%s\n", PROC_DIR, PROC_MESSAGE);
        return -ENOMEM;
    }

    // Create /proc/myprocdir/status (read-only)
    proc_status_entry = proc_create(PROC_STATUS, 0444, proc_dir, &status_fops);
    if (!proc_status_entry) {
        remove_proc_entry(PROC_MESSAGE, proc_dir);
        remove_proc_entry(PROC_DIR, NULL);
        pr_err("Failed to create /proc/%s/%s\n", PROC_DIR, PROC_STATUS);
        return -ENOMEM;
    }

    pr_info("Procfs Created: /proc/%s/{%s, %s}\n", PROC_DIR, PROC_MESSAGE, PROC_STATUS);
    return 0;
}

/* Module Exit Function */
static void __exit proc_example_exit(void) {
    remove_proc_entry(PROC_MESSAGE, proc_dir);
    remove_proc_entry(PROC_STATUS, proc_dir);
    remove_proc_entry(PROC_DIR, NULL);
    pr_info("Procfs Removed: /proc/%s\n", PROC_DIR);
}

module_init(proc_example_init);
module_exit(proc_example_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surieya");
MODULE_DESCRIPTION("Advanced Procfs Example with Multiple Files");
