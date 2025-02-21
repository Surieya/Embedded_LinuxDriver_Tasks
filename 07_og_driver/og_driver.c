#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include <linux/err.h>


#define memory_size        512           //Memory Size

dev_t maj_min_dev = 0;

static struct class *og_driver_class;
static struct cdev og_driver_device;

uint8_t *kernel_buffer;



static int      __init init_func(void);
static void     __exit exit_func(void);
static int      open_func(struct inode *deviceFile, struct file *file);
static int      release_func(struct inode *deviceFile, struct file *file);
//len is passed from the user space
//bytes_written = write(fd, message, strlen(message)); // Passes `len = strlen(message)
//off used in large drivers to keep track
static ssize_t  read_func(struct file *file,char __user *buf, size_t len,loff_t * off);
static ssize_t  write_func(struct file *file, const char __user *buf, size_t len, loff_t * off);



static struct file_operations fops = {
      .read = read_func,
      .write = write_func,
      .open = open_func,
      .release = release_func,
};

static int open_func(struct inode *deviceFile, struct file *file)
{
        pr_info("Device File Opened...!!!\n");
        return 0;
}
/*
** This function will be called when we close the Device file
*/
static int release_func(struct inode *deviceFile, struct file *file)
{
        pr_info("Device File Closed...!!!\n");
        return 0;
}





static ssize_t read_func(struct file *file, char __user *buf, size_t len, loff_t *off)
{
        //Copy the data from the kernel space to the user-space
        if( copy_to_user(buf, kernel_buffer, memory_size) )
        {
                pr_err("Data Read : Err!\n");
        }
        pr_info("Data Read : Done!\n");


        return memory_size;
}
/*
** This function will be called when we write the Device file
*/
static ssize_t write_func(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
        //Copy the data to kernel space from the user-space
        if(copy_from_user(kernel_buffer, buf, len) )
        {
                pr_err("Data Write : Err!\n");
        }
        pr_info("Data Write : Done!\n");
        return len;
}




static int __init init_func(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&maj_min_dev, 0, 1, "og_driver_devicefile")) <0){
                pr_info("Cannot allocate major number\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(maj_min_dev), MINOR(maj_min_dev));
 
        /*Creating cdev structure*/
        //this is required to map the fops -> driver
        cdev_init(&og_driver_device,&fops);
 
        /*Adding character device to the system*/
        if((cdev_add(&og_driver_device,maj_min_dev,1)) < 0){
            pr_info("Cannot add the device to the system\n");
            goto r_class;
        }
 
        /*Creating struct class*/
        //sys/class
        if(IS_ERR(og_driver_class = class_create("og_driver_class"))){
            pr_info("Cannot create the struct class\n");
            goto r_class;
        }
 
        /*Creating device*/
        // /dev
        if(IS_ERR(device_create(og_driver_class,NULL,maj_min_dev,NULL,"og_driver_device"))){
            pr_info("Cannot create the Device 1\n");
            goto r_device;
        }
        
        /*Creating Physical memory*/
        if((kernel_buffer = kmalloc(memory_size , GFP_KERNEL)) == 0){
            pr_info("Cannot allocate memory in kernel\n");
            goto r_device;
        }
        
        strcpy(kernel_buffer, "Hello_World");
        
        pr_info("Device Driver Insert...Done!!!\n");
        return 0;
 
r_device:
        class_destroy(og_driver_class);
r_class:
        unregister_chrdev_region(maj_min_dev,1);
        return -1;
}
/*
** Module exit function
*/
static void __exit exit_func(void)
{
  kfree(kernel_buffer);
        device_destroy(og_driver_class,maj_min_dev);
        class_destroy(og_driver_class);
        cdev_del(&og_driver_device);
        unregister_chrdev_region(maj_min_dev, 1);
        pr_info("Device Driver Remove...Done!!!\n");
}
 
module_init(init_func);
module_exit(exit_func);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surieya");
MODULE_DESCRIPTION("Simple Linux device driver (Real Linux Device Driver - THE OG)");

