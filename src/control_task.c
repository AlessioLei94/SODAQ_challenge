/* 
 * File containing the implementation of a control task.
 * Controls a LED, feeds the WDT and can be stopped pressing a button
 */
#include <zephyr.h>
#include <sys/reboot.h>
#include <task_wdt/task_wdt.h>

#include <leds.h>
#include <btns.h>

#define SLEEP_TIME_MS   	200U
#define WDT_RESET_TIME_MS	210U

static bool stop_task = false;

/*
 * Task WDT callback function
 * 
 * - channel_id: id of the channel the timeout refers to
 * - user_data: parameter used to pass the thread id in order to print it
 */
void task_wdt_cb(int channel_id, void *user_data) {
	printk("Task watchdog channel %d callback, thread: %s didn't reset WDT in time\n",
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
 * BTN pressed callback used to flip a booleanflag that if true is going to block
 * the control task forever triggering the WDT
 */
void button0_pressed_cb(const struct device *port, struct gpio_callback *cb, uint32_t pins) {
	printk("Button0 pressed cb\n");

	stop_task = !stop_task;
}

/*
 * Control task function.
 * Initialize led, add a new channel to the task WDT then enter infinite loop
 * in which the LED status is switched between ON and OFF and the WDT is fed.
 */
void control_task_fun(void) {
	bool led_status[LEDS_NUMBER] = {true, true, true, true};
	uint8_t counter = 0;
	int ret = 0;

	printk("control_task started!\n");

	//Register button pressed cb
	if(register_btn1_cb(button0_pressed_cb) < 0) {
		return;
	}

	//Add wdt channel for control task, giving a callback to call when wdt resets
	//and passing the thread id to the callback as an argument
	int wdt_id = task_wdt_add(WDT_RESET_TIME_MS, task_wdt_cb, (void *)k_current_get());
	if(wdt_id < 0) {
		printk("task_wdt_add() failed (%d)\n", wdt_id);
		return;
	}

	while(1) {
		if(stop_task) {
			k_sleep(K_FOREVER);
		}

		ret = led_set_status(counter, led_status[counter]);
		if(ret != 0) {
			printk("Failed to set LED to status %d, trying again\n", led_status[counter]);
		} else {
			led_status[counter] = !led_status[counter];
		}

		counter = (counter + 1) % LEDS_NUMBER;

		task_wdt_feed(wdt_id);
		k_sleep(K_MSEC(SLEEP_TIME_MS));
	}
}

/*
 * Define task at compile time and starts it 1 sec after boot
 */
K_THREAD_DEFINE(control_task, 1024, control_task_fun, NULL, NULL, NULL, 0, 0, 1000U);
