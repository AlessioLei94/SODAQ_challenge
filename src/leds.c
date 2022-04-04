/*
 * File representing a LED driver.
 * Simple implemetation because only one LED is initialized and handled,
 * could be extended to handle more than one.
 */
#include <leds.h>

#include <drivers/gpio.h>

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
#define LED0	DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN0	DT_GPIO_PIN(LED0_NODE, gpios)
#define FLAGS0	DT_GPIO_FLAGS(LED0_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led0 devicetree alias is not defined"
#define LED0	""
#define PIN0	0
#define FLAGS0	0
#endif

#if DT_NODE_HAS_STATUS(LED1_NODE, okay)
#define LED1	DT_GPIO_LABEL(LED1_NODE, gpios)
#define PIN1	DT_GPIO_PIN(LED1_NODE, gpios)
#define FLAGS1	DT_GPIO_FLAGS(LED1_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led1 devicetree alias is not defined"
#define LED1	""
#define PIN1	0
#define FLAGS1	0
#endif

#if DT_NODE_HAS_STATUS(LED2_NODE, okay)
#define LED2	DT_GPIO_LABEL(LED2_NODE, gpios)
#define PIN2	DT_GPIO_PIN(LED2_NODE, gpios)
#define FLAGS2	DT_GPIO_FLAGS(LED2_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led2 devicetree alias is not defined"
#define LED2	""
#define PIN2	0
#define FLAGS2	0
#endif

#if DT_NODE_HAS_STATUS(LED3_NODE, okay)
#define LED3	DT_GPIO_LABEL(LED3_NODE, gpios)
#define PIN3	DT_GPIO_PIN(LED3_NODE, gpios)
#define FLAGS3	DT_GPIO_FLAGS(LED3_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led3 devicetree alias is not defined"
#define LED3	""
#define PIN3	0
#define FLAGS3	0
#endif

static const struct device *devs[LEDS_NUMBER];
static char *dev_names[LEDS_NUMBER] = {LED0, LED1, LED2, LED3};
static gpio_pin_t pins[LEDS_NUMBER] = {PIN0, PIN1, PIN2, PIN3};
static gpio_flags_t flags[LEDS_NUMBER] = {FLAGS0, FLAGS1, FLAGS2, FLAGS3};

int led_set_status(uint8_t dev_index, bool status) {
	if(dev_index >= LEDS_NUMBER) {
		printk("Invalid LED index %d\n", dev_index);
		return -1;
	}

    return gpio_pin_set(devs[dev_index], pins[dev_index], (int)status);
}

/*
 * Init function for LED driver. Retrieve the device using its name and configure GPIO
 * 
 * Return 0 on success, -1 for failure (could be customised returning different negative numbers)
 */
int led_init(void) {
	int ret = 0;

	for (int i = 0; i < LEDS_NUMBER ; i++) {
		devs[i] = device_get_binding(dev_names[i]);
		if (devs[i] == NULL) {
			printk("device_get_binding() failed\n");
			return -1;
		}

		ret = gpio_pin_configure(devs[i], pins[i], GPIO_OUTPUT_ACTIVE | flags[i]);
		if (ret < 0) {
			printk("gpio_pin_configure() failed (%d)\n", ret);
			return ret;
		}
	}

	return ret;
}