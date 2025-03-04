#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xe2c17b5d, "__SCT__might_resched" },
	{ 0xfe487975, "init_wait_entry" },
	{ 0x1000e51, "schedule" },
	{ 0x8c26d495, "prepare_to_wait_event" },
	{ 0x92540fbf, "finish_wait" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x14777360, "cdev_init" },
	{ 0xaa5419b1, "cdev_add" },
	{ 0x169675e2, "class_create" },
	{ 0x92649ec3, "device_create" },
	{ 0xd9a5ea54, "__init_waitqueue_head" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x288176f5, "class_destroy" },
	{ 0x38655e10, "device_destroy" },
	{ 0x39d2b405, "cdev_del" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x122c3a7e, "_printk" },
	{ 0xe2964344, "__wake_up" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x6ad2b3e, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "0B3D8BEB8331AB94C12C468");
