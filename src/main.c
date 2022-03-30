/*
 * Main file of the project
 * Containing the WDT initialization and an infinite loop that just prints and feeds thw wdt
 */
#include <zephyr.h>
#include <device.h>
#include <sys/printk.h>
#include <sys/reboot.h>
#include <drivers/watchdog.h>
#include <drivers/hwinfo.h>
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

void main(void)
{
	uint32_t reset_cause = 0;

	/*
	 * Get info about last reset's cause.
	 * Do not return in case this operation is not supported (ret == -ENOTSUP)
	 */
	int ret = hwinfo_get_reset_cause(&reset_cause);
	if(ret == -ENOTSUP) {
		printk("hwinfo_get_reset_cause() not supported, value will be zero");
	} else if(ret != 0) {
		printk("hwinfo_get_reset_cause() failed (%d)", ret);
		return;
	}

	//Clear reset flags after reading them, so they won't accumulate from different resets
	hwinfo_clear_reset_cause();

	printk("SODAQ Challenge fw booted\nLast reset cause was %d\n", reset_cause);

#ifdef WDT_NODE
	const struct device *hw_wdt = DEVICE_DT_GET(WDT_NODE);
#else
	const struct device *hw_wdt = NULL;
#endif

 	if(!device_is_ready(hw_wdt)) {
		printk("WDT %s not ready", hw_wdt->name);
		hw_wdt = NULL;
	}

	ret = task_wdt_init(hw_wdt);
	if(ret != 0) {
		printk("task_wdt_init() failed (%d)", ret);
		return;
	}

	//Add wdt channel for main task, passing NULL instead of cb triggers reset
	int wdt_id = task_wdt_add(1100U, NULL, NULL);
	if(wdt_id < 0) {
		printk("task_wdt_add() failed (%d)", wdt_id);
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
