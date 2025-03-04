#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include<linux/sched.h>
#include<linux/delay.h>


static struct task_struct *kthread1;
static struct task_struct *kthread2;
static int t1 = 1, t2 = 2;

int thread_func1(void *);

/*  
   @brief function executed by the thread
   @param thread_nr => ptr to the number of the thread
*/
int thread_func1(void *thread_nr){
  unsigned int counter = 0;
  int t_nr = *(int *) thread_nr;

  /*Working loop*/

  while(!kthread_should_stop()){
      printk("kthread => thread %d under execution => counter = %d\n",t_nr,counter++);   
      msleep(t_nr * 1000);
  }

  printk("kthread => thread %d finished the execution\n",t_nr);
  return 0;

}



static int __init initFunc(void){
    printk("Kernel thread init");
    kthread1 = kthread_create(thread_func1,&t1,"kThread_1");
    if(kthread1 != NULL){
        /*start the thread*/

        wake_up_process(kthread1);
        printk("Kthread 1 was created and is running now!\n");
    }else{
        printk("Kthread could'nt be created");
        return -1;
    }
    
    kthread2 = kthread_run(thread_func1,&t2,"Kthread_2");
    if(kthread2 != NULL){
        printk("kthread => Thread 2 was created and is running now \n");
    }else{
        printk("Kthread => Thread 2 could not be created ! \n");
        kthread_stop(kthread1);
        return -1;
    }

    printk("Both the Kthreads are running successfully");

    return 0;
}

static void __exit exitFunc(void){
    printk("Exit Kernel thread driver module");
    kthread_stop(kthread1);
    kthread_stop(kthread2);

}


module_init(initFunc);
module_exit(exitFunc);





MODULE_LICENSE("GPL");
MODULE_AUTHOR("Surieya");
MODULE_DESCRIPTION("Kernel threads driver module");
