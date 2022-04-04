/*
 * File representing a LED driver.
 * Simple implemetation because only one LED is initialized and handled,
 * could be extended to handle more than one.
 */
#include <leds.h>

#include <drivers/gpio.h>

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

static const struct device *dev;

int led_set_status(bool status) {
    return gpio_pin_set(dev, PIN, (int)status);
}

/*
 * Init function for LED driver. Retrieve the device using its name and configure GPIO
 * 
 * Return 0 on success, -1 for failure (could be customised returning different negative numbers)
 */
int led_init(void) {
	dev = device_get_binding(LED0);
	if (dev == NULL) {
		printk("device_get_binding() failed\n");
		return -1;
	}

	int ret = gpio_pin_configure(dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
	if (ret < 0) {
		printk("gpio_pin_configure() failed (%d)\n", ret);
		return ret;
	}

	return ret;
}