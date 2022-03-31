/* 
 * File containing a task that turns on and off a LED using different colors
 */
#include <zephyr.h>
#include <sys/reboot.h>
#include <task_wdt/task_wdt.h>
#include <drivers/gpio.h>

#define SLEEP_TIME_MS   	200U
#define WDT_RESET_TIME_MS	500U

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
#define LED0	DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN		DT_GPIO_PIN(LED0_NODE, gpios)
#define FLAGS	DT_GPIO_FLAGS(LED0_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led0 devicetree alias is not defined"
#define LED0	""
#define PIN		0
#define FLAGS	0
#endif

const struct device *dev;

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

int led_control_init(void) {
	dev = device_get_binding(LED0);
	if (dev == NULL) {
		printk("device_get_binding() failed");
		return -1;
	}

	int ret = gpio_pin_configure(dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
	if (ret < 0) {
		printk("gpio_pin_configure() failed (%d)", ret);
		return -1;
	}

	return 0;
}

void led_control_task(void) {
	bool led_status = true;

	printk("control_task started!");

	if(led_control_init() < 0) {
		return;
	}

	int wdt_id = task_wdt_add(WDT_RESET_TIME_MS, task_wdt_cb, (void *)k_current_get());
	if(wdt_id < 0) {
		printk("task_wdt_add() failed (%d)", wdt_id);
		return;
	}

	while(1) {
		//TODO: Put some condition that if true blocks the task here (BTN pressed, time based, else)
		gpio_pin_set(dev, PIN, (int)led_status);
		led_status = !led_status;

		task_wdt_feed(wdt_id);
		k_sleep(K_MSEC(SLEEP_TIME_MS));
	}
}

/*
 * Declare task at compile time
 * 1. Start 1sec after boot
 * 2. Priority -> lowest number is higher priority
 */
K_THREAD_DEFINE(led_task, 1024, led_control_task, NULL, NULL, NULL, 0, 0, 1000U);
