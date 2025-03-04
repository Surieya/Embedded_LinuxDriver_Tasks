#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define SYSFS_DIR "sysfs_driver"
#define SYSFS_FILE "sysfs_value"

static struct kobject *sysfs_kobj;
static int sysfs_value = 0;

/* Read function for sysfs file */
static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    pr_info("Sysfs - Read\n");
    return sprintf(buf, "%d\n", sysfs_value);
}

/* Write function for sysfs file */
static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    pr_info("Sysfs - Write\n");
    sscanf(buf, "%d", &sysfs_value);
    return count;
}



static struct kobj_attribute sysfs_attr = __ATTR(sysfs_value, 0660, sysfs_show, sysfs_store);

/* Module Init function */
static int __init sysfs_driver_init(void)
{
    /* Creating a directory in /sys/kernel */
    sysfs_kobj = kobject_create_and_add(SYSFS_DIR, kernel_kobj);
    if (!sysfs_kobj) {
        pr_err("Cannot create sysfs directory\n");
        return -ENOMEM;
    }

    /* Creating sysfs file */
    if (sysfs_create_file(sysfs_kobj, &sysfs_attr.attr)) {
        pr_err("Cannot create sysfs file\n");
        kobject_put(sysfs_kobj);
        return -ENOMEM;
    }

    pr_info("Sysfs Driver Inserted\n");
    return 0;
}

/* Module Exit functioEmbedded_LinuxDrivers_Tasks/10_wait_wake_queue/Makefilen */
static void __exit sysfs_driver_exit(void)
{
    sysfs_remove_file(sysfs_kobj, &sysfs_attr.attr);
    kobject_put(sysfs_kobj);
    pr_info("Sysfs Driver Removed\n");
}

module_init(sysfs_driver_init);
module_exit(sysfs_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surieya");
MODULE_DESCRIPTION("Simple Linux device driver using sysfs");
MODULE_VERSION("1.0");
