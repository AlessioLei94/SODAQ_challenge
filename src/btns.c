/*
 * File representing a BTN driver.
 * Simple implemetation because only two buttons are handled with basic "on press" callback
 */
#include <btns.h>

/*
 * Get buttons configuration from the devicetree sw0/sw1 alias. This is mandatory.
 */
#define SW0_NODE	DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif

#define SW1_NODE	DT_ALIAS(sw1)
#if !DT_NODE_HAS_STATUS(SW1_NODE, okay)
#error "Unsupported board: sw1 devicetree alias is not defined"
#endif

static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});
static const struct gpio_dt_spec button2 = GPIO_DT_SPEC_GET_OR(SW1_NODE, gpios, {0});

int register_btn1_cb(gpio_callback_handler_t cb) {
	static struct gpio_callback button0_cb_data;
    static bool registered = false;

    if(registered) {
        printk("Button0 cn already registered!\n");
        return -1;
    }

    gpio_init_callback(&button0_cb_data, cb, BIT(button1.pin));
    int ret = gpio_add_callback(button1.port, &button0_cb_data);
    if(ret != 0) {
        printk("gpio_add_callback failed (%d)\n", ret);
        return ret;
    }

    registered = true;

    return ret;
}

int register_btn2_cb(gpio_callback_handler_t cb) {
	static struct gpio_callback button1_cb_data;
    static bool registered = false;

    if(registered) {
        printk("Button1 cn already registered!\n");
        return -1;
    }

    gpio_init_callback(&button1_cb_data, cb, BIT(button2.pin));
    int ret = gpio_add_callback(button2.port, &button1_cb_data);
    if(ret != 0) {
        printk("gpio_add_callback failed (%d)\n", ret);
        return ret;
    }

    registered = true;

    return ret;
}

int btns_init(void) {
    int ret;

	//Initialize button 0
	if (!device_is_ready(button1.port)) {
		printk("Error: button1 device %s is not ready\n",
		       button1.port->name);
		return -1;
	}

	ret = gpio_pin_configure_dt(&button1, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, button1.port->name, button1.pin);
		return ret;
	}

	ret = gpio_pin_interrupt_configure_dt(&button1,
					      GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, button1.port->name, button1.pin);
		return ret;
	}

	//Initialize button 1
	if (!device_is_ready(button2.port)) {
		printk("Error: button1 device %s is not ready\n",
		       button2.port->name);
		return -1;
	}

	ret = gpio_pin_configure_dt(&button2, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, button2.port->name, button2.pin);
		return ret;
	}

	ret = gpio_pin_interrupt_configure_dt(&button2,
					      GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, button2.port->name, button2.pin);
		return ret;
	}

	printk("Set up button2 at %s pin %d\n", button2.port->name, button2.pin);

    return ret;
}