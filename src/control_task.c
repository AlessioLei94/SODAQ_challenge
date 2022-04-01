/* 
 * File containing the implementation of a control task.
 * Controls a LED, feeds the WDT and can be stopped pressing a button
 */
#include <zephyr.h>
#include <sys/reboot.h>
#include <task_wdt/task_wdt.h>

#include <leds.h>

#define SLEEP_TIME_MS   	200U
#define WDT_RESET_TIME_MS	500U

/*
 * Task WDT callback function
 * 
 * - channel_id: id of the channel the timeout refers to
 * - user_data: parameter used to pass the thread id in order to print it
 */
void task_wdt_cb(int channel_id, void *user_data) {
	printk("Task watchdog channel %d callback, thread: %s\n",
		channel_id, k_thread_name_get((k_tid_t)user_data));

	/*
	 * Here is possible to call task_wdt_feed(channel_id) in
	 * case the problem can be resolved.
	 *
	 * Otherwise we can perform some cleanup if needed and reset the device.
	 */

	printk("Resetting device...\n");

	sys_reboot(SYS_REBOOT_COLD);
}

/*
 * Control task function.
 * Initialize led, add a new channel to the task WDT then enter infinite loop
 * in which the LED status is switched between ON and OFF and the WDT is fed.
 */
void control_task(void) {
	bool led_status = true;
	int ret = 0;

	printk("control_task started!");

	if(led_init() < 0) {
		return;
	}

	int wdt_id = task_wdt_add(WDT_RESET_TIME_MS, task_wdt_cb, (void *)k_current_get());
	if(wdt_id < 0) {
		printk("task_wdt_add() failed (%d)", wdt_id);
		return;
	}

	while(1) {
		//TODO: Put some condition that if true blocks the task here (BTN pressed, time based, else)
		ret = led_set_status(led_status);
		if(ret != 0) {
			printk("Failed to set LED to status %d, trying again", led_status);
		} else {
			led_status = !led_status;
		}

		task_wdt_feed(wdt_id);
		k_sleep(K_MSEC(SLEEP_TIME_MS));
	}
}

/*
 * Define task at compile time and starts it 1 sec after boot
 */
K_THREAD_DEFINE(led_task, 1024, control_task, NULL, NULL, NULL, 0, 0, 1000U);
