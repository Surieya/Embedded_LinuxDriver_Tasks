/***************************************************************************//**
*  \file       main.c
*
*  \details    Simple driver
*
*  \author     Surieya
*
* *******************************************************************************/
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
 
/*
** Module Init function
*/
static int __init initFunc(void)
{   
    printk("Inside init function which acts as an Constructor");
    printk(KERN_INFO "Kernel Module Inserted Successfully...\n");
    return 0;
}

/*
** Module Exit function
*/
static void __exit exitFunc(void)
{  
    printk("Inside exit function which acts as an Destructor");
    printk(KERN_INFO "Kernel Module Removed Successfully...\n");
}
 
module_init(initFunc);
module_exit(exitFunc);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surieya");