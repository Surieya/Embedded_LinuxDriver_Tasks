#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define DEVICE_NAME "ioctl_device"
#define CLASS_NAME "ioctl_class"

#define IOCTL_SET_VALUE _IOW('a', 1, int)  // User → Kernel (Write)
#define IOCTL_GET_VALUE _IOR('a', 2, int)  // Kernel → User (Read)

static dev_t dev;
static struct cdev cdev;
static struct class *dev_class;
static struct device *dev_device;

static int stored_value = 0;  // Value stored in the driver

/* IOCTL Function */
static long ioctl_func(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case IOCTL_SET_VALUE:
            if (copy_from_user(&stored_value, (int __user *)arg, sizeof(stored_value)))
                return -EFAULT;
            pr_info("IOCTL: Set Value = %d\n", stored_value);
            break;

        case IOCTL_GET_VALUE:
            if (copy_to_user((int __user *)arg, &stored_value, sizeof(stored_value)))
                return -EFAULT;
            pr_info("IOCTL: Get Value = %d\n", stored_value);
            break;

        default:
            return -EINVAL;
    }
    return 0;
}

/* File operations structure */
static struct file_operations fops = {
    .unlocked_ioctl = ioctl_func,
};

/* Module Init Function */
static int __init ioctl_driver_init(void) {
    // Step 1: Allocate major and minor numbers dynamically
    if (alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME) < 0) {
        pr_err("Failed to allocate major number\n");
        return -1;
    }
    pr_info("Major = %d, Minor = %d\n", MAJOR(dev), MINOR(dev));

    // Step 2: Initialize and add character device
    cdev_init(&cdev, &fops);
    if (cdev_add(&cdev, dev, 1) < 0) {
        unregister_chrdev_region(dev, 1);
        pr_err("Failed to add cdev\n");
        return -1;
    }
    pr_info("Initialized and added character device ");

    // Step 3: Create class for automatic device file creation
    dev_class = class_create(CLASS_NAME);
    if (IS_ERR(dev_class)) {
        cdev_del(&cdev);
        unregister_chrdev_region(dev, 1);
        pr_err("Failed to create class\n");
        return PTR_ERR(dev_class);
    }

    // Step 4: Create the device file (/dev/ioctl_device)
    dev_device = device_create(dev_class, NULL, dev, NULL, DEVICE_NAME);
    if (IS_ERR(dev_device)) {
        class_destroy(dev_class);
        cdev_del(&cdev);
        unregister_chrdev_region(dev, 1);
        pr_err("Failed to create device\n");
        return -1;
    }

    pr_info("IOCTL Driver Initialized Successfully\n");
    return 0;
}

/* Module Exit Function */
static void __exit ioctl_driver_exit(void) {
    device_destroy(dev_class, dev);  // Remove /dev/ioctl_device
    class_destroy(dev_class);
    cdev_del(&cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("IOCTL Driver Removed\n");
}

module_init(ioctl_driver_init);
module_exit(ioctl_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surieya");
MODULE_DESCRIPTION("Simple IOCTL Driver Example with Dynamic Device File Creation");
