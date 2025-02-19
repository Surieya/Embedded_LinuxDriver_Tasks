/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple linux driver (Dynamically allocating the Major and Minor number)
*
*  \author     EmbeTronicX
*
* *******************************************************************************/
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
 
dev_t dev = 0;

/*
** Module Init function
*/
static int __init init_func(void)
{
        /*Allocating Major number*/
        //int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name);
        if((alloc_chrdev_region(&dev, 0, 1, "Surieya_Device")) <0){
                printk(KERN_INFO "Cannot allocate major number for device 1\n");
                return -1;
        }
        printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
        printk(KERN_INFO "Kernel Module Inserted Successfully...\n");
        
        return 0;
}

/*
** Module exit function
*/
static void __exit exit_func(void)
{       //***/
        unregister_chrdev_region(dev, 1);
        printk(KERN_INFO "Kernel Module Removed Successfully...\n");
}
 
module_init(init_func);
module_exit(exit_func);



 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surieya");




//commit message format should be followed

// Function added : feature modified
// File name : this variable and function modified.
// File name 2: this variable and function modified.
// change Id :
// name <mail id>