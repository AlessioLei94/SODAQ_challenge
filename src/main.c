/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <sys/printk.h>
#include <sys/reboot.h>
#include <drivers/watchdog.h>
#include <task_wdt/task_wdt.h>

/*
 * To use this sample, either the devicetree's /aliases must have a
 * 'watchdog0' property, or one of the following watchdog compatibles
 * must have an enabled node.
 *
 * If the devicetree has a watchdog node, we get the watchdog device
 * from there. Otherwise, the task watchdog will be used without a
 * hardware watchdog fallback.
 */
#if DT_NODE_HAS_STATUS(DT_ALIAS(watchdog0), okay)
#define WDT_NODE DT_ALIAS(watchdog0)
#elif DT_HAS_COMPAT_STATUS_OKAY(nordic_nrf_watchdog)
#define WDT_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(nordic_nrf_watchdog)
#endif

void task_wdt_cb(int channel_id, void *user_data) {
	printk("Task watchdog channel %d callback, thread: %s\n",
		channel_id, k_thread_name_get((k_tid_t)user_data));

	/*
	 * If the issue could be resolved, call task_wdt_feed(channel_id) here
	 * to continue operation.
	 *
	 * Otherwise we can perform some cleanup and reset the device.
	 */

	printk("Resetting device...\n");

	sys_reboot(SYS_REBOOT_COLD);
}

void main(void)
{
#ifdef WDT_NODE
	const struct device *hw_wdt = DEVICE_DT_GET(WDT_NODE);
#else
	const struct device *hw_wdt = NULL;
#endif

 	if(!device_is_ready(hw_wdt)) {
		printk("WDT %s not ready", hw_wdt->name);
		hw_wdt = NULL;
	}

	int ret = task_wdt_init(hw_wdt);
	if(ret != 0) {
		printk("task_wdt_init failed (%d)", ret);
		return;
	}

	//Add wdt channel for main task, passing NULL instead of cb triggers reset
	int wdt_id = task_wdt_add(1100U, NULL, NULL);
	if(wdt_id < 0) {
		printk("task_wdt_add failed (%d)", wdt_id);
		return;
	}

	/*
	 * Infinite loop that just prints a message to stdout
	 * and feeds the task wdt once every second
	*/
	while(1) {
		printk("Main task is alive :)");
		//task_wdt_feed(wdt_id);
		k_sleep(K_MSEC(1000));
	}
}

void control_task(void) {
	printk("control_task started!");

	int wdt_id = task_wdt_add(110U, task_wdt_cb, (void *)k_current_get());
	if(wdt_id < 0) {
		printk("task_wdt_add failed (%d)", wdt_id);
		return;
	}

	while(1) {
		//TODO: Put some condition that if true blocks the task here (BTN pressed, time based, else)

		task_wdt_feed(wdt_id);
		k_sleep(K_MSEC(50));
	}
}

//Priority -> lowest number is higher priority
K_THREAD_DEFINE(task_1, 1024, control_task, NULL, NULL, NULL, -1, 0, 1000U);