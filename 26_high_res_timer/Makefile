obj-m += high_res_timer_driver.o



all:
	 make -C /lib/modules/$(shell uname -r)/build  M=$(shell pwd) modules
 
clean:
	 make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean